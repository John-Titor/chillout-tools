// Chillout Quantum v3 protocol
// ============================
//
// Interface is RS-485 @ 115200n81. Other units (pro, etc.) look to use a similar
// protocol at 9600n81 but not yet verified / supported here.
//
// Circular connector pinout at the compressor unit (pin numbers are marked on the
// remote cable connector face):
//
//  1: +5V
//  2: RS-485 A
//  3: RS-485 B
//  4: GND
//
// Packet format:
// --------------
//
//          000102030405060708090a0b0c0d0e
// Example: c00e0100080002078e000200008401
//            LLAAPPPPPPPPPPPPPPPPPPPPSS
//                PPxxxxTTCCCCxxxxRRRR
//
// LL is packet length -1
// AA is sender address, 1, 2 = compressor, 3 = remote
// PP is packet data
// SS is CRC-8 with final Xor of PP bytes based on AA value; 1 = 0x30, 2 = 0xea, 3 = 0xe9
//
//
// Remote packet data:
// -------------------
//
// PPxxTTMM
//
// PP is power mode; 0x00 = off, 0x01 = eco, 0x03 = max
// TT is set temperature, 1 = min (1.7°C, units seem to be ~2.7°C) 10 = max
// MM is 0x00 when changing PP, 0x01 when changing TT
//
//
// Compressor packet data (address 1):
// -----------------------------------
//
// multi-byte fields are big-endian
//
// PPxxxxTTCCCCxxxxRRRR
//
// PP is power mode (see remote, 0x02 for off-in-max-mode)
// TT is set temperature (see remote)
// CCCC is coolant temperature in 1/100°C
// RRRR is compressor speed in rpm.
//
// Compressor packet data (address 2):
// -----------------------------------
// Unknown, ignored.
//

#pragma once
#include "input.h"

namespace Chillout
{
// Bare-bones compressor protocol parser.
// Limited sanity-checking, no attempt at CRC validation,
// just enough so that we know what the compressor is doing.
enum {
    RESET 			= -1,
    WAIT_HEADER 	= 0x00,
    WAIT_LENGTH 	= 0x01,
    WAIT_ADDRESS 	= 0x02,
    WAIT_MODE 		= 0x03,
    WAIT_SETPOINT 	= 0x06,
    WAIT_TRAILER 	= 0x0e,
};

enum {
    MODE_ON         = 1,
    MODE_MAX        = 2,
};

int parse_state = WAIT_HEADER;
uint8_t mode;
uint8_t setting;

// Bare-bones protocol parser
//
bool
recv(unsigned c)
{
    switch (parse_state) {
    case WAIT_HEADER:
        if (c != 0xc0) {
            parse_state = RESET;
        }
        break;

    case WAIT_LENGTH:
        if (c != 0x0e) {
            parse_state = RESET;
        }
        break;

    case WAIT_ADDRESS:
        if (c != 0x01) {
            parse_state = RESET;
        }
        break;

    case WAIT_MODE:
    	if ((c & (MODE_ON | MODE_MAX)) != c) {
    		parse_state = RESET;
    	} else {
        	mode = c;
        }
        break;

    case WAIT_SETPOINT:
        if ((c < 1) || (c >> 0x0a)) {
            parse_state = RESET;
        } else {
            setting = 11 - c;
        }
        break;

    case WAIT_TRAILER:
    	if (c != 0x01) {
    		parse_state = RESET;
    	} else {
        	parse_state = WAIT_HEADER;
        	return true;
        }
        break;

    default:
        break;
    }

    parse_state++;
    return false;
}

enum {
    SET_OFF,
    SET_ON,
    SET_NONE = 1000,
};

struct Command {
    uint8_t bytes[9];
};

// Table of useful commands.
// Much simpler / cheaper than the CRC algorithm
// necessary to generate and send arbitrary packets.
//
const Command cmd_table[] = {
    { .bytes = {0xc0, 0x08, 0x03, 0x00, 0x00, 0x0a, 0x00, 0x6b, 0x01} }, // off
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x0a, 0x00, 0x51, 0x01} }, // on max
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x0a, 0x01, 0x56, 0x01} }, // least cooling
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x09, 0x01, 0x69, 0x01} }, // ...
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x08, 0x01, 0x7c, 0x01} },
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x07, 0x01, 0xbf, 0x01} },
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x06, 0x01, 0xaa, 0x01} },
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x05, 0x01, 0x95, 0x01} },
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x04, 0x01, 0x80, 0x01} },
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x03, 0x01, 0xeb, 0x01} },
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x02, 0x01, 0xfe, 0x01} },
    { .bytes = {0xc0, 0x08, 0x03, 0x03, 0x00, 0x01, 0x01, 0xc1, 0x01} }, // most cooling
};

// Send compressor commands to adjust state to match target.
// Prioritise on/off commands over power settings.
//
unsigned
update_index(uint8_t target_setting)
{
    int cmd = SET_NONE;

    if (target_setting == Input::OFF) {
    	if (mode & MODE_ON) {
        	cmd = SET_OFF;
        }
    } else if (target_setting != Input::OFF) {
    	if (!(mode & MODE_ON)) {
        	cmd = SET_ON;
    	} else {
        	if (target_setting != setting) {
            	cmd = SET_ON + target_setting;
            }
        }
    }
    return cmd;
}

const Command *
update_command(uint8_t target_setting)
{
	auto cmd = update_index(target_setting);
    if (cmd != SET_NONE) {
        return &cmd_table[cmd];
    }
    return nullptr;
}
}