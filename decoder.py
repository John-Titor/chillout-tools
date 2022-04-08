#!/usr/bin/env python3
#
# Chillout Quantum v3 protocol analyser
#
# Interface is RS-485 @ 115200n81. Other units (pro, etc.) look to use a similar
# protocol at 9600n81 but not yet verified / supported here.
#
# Circular connector pinout at the compressor unit (pin numbers are marked on the
# remote cable connector face):
#
#  1: +5V
#  2: RS-485 A
#  3: RS-485 B
#  4: GND
#
# Packet format:
# =============
#
# Example: c00e0100080002078e000200008401
#            LLAAPPPPPPPPPPPPPPPPPPPPSS
#
# LL is packet length -1
# AA is sender address, 1, 2 = compressor, 3 = remote
# PP is packet data
# SS is CRC-8 with final Xor of PP bytes based on AA value; 1 = 0x30, 2 = 0xea, 3 = 0xe9
#
#
# Remote packet data:
# ==================
#
# PPxxTTMM
#
# PP is power mode; 0x00 = off, 0x01 = eco, 0x03 = max
# TT is set temperature, 1 = min (1.7°C, units seem to be ~2.7°C) 10 = max
# MM is 0x00 when changing PP, 0x01 when changing TT
#
#
# Compressor packet data:
# ======================
#
# multi-byte fields are big-endian
#
# PPxxxxTTCCCCxxxxRRRR
#
# PP is power mode (see remote)
# TT is set temperature (see remote)
# CCCC is coolant temperature in 1/100°C
# RRRR is compressor speed in rpm.
#

import serial
import crccheck
import struct

#ser = serial.Serial('/dev/cu.usbserial-A106TL0J', 115200)
ser = serial.Serial('/dev/cu.SLAB_USBtoUART', 115200)
crc_alg = {
    1: crccheck.crc.Crc(width=8,
                        poly=0x07,
                        initvalue=0x00,
                        reflect_input=False,
                        reflect_output=False,
                        xor_output=0x30,
                        check_result=0xf4),
    2: crccheck.crc.Crc(width=8,
                        poly=0x07,
                        initvalue=0x00,
                        reflect_input=False,
                        reflect_output=False,
                        xor_output=0xea,
                        check_result=0xf4),
    3: crccheck.crc.Crc(width=8,
                        poly=0x07,
                        initvalue=0x00,
                        reflect_input=False,
                        reflect_output=False,
                        xor_output=0xe9,
                        check_result=0xf4),
    }

power_modes = ['off', 'eco', 'off', 'max']
set_temps = [0, 1.3, 4.4, 7.2, 10.0, 12.7, 15.5, 18.3, 21.1, 23.9, 26.7]

while True:
    hdr = ser.read(1)
    if hdr != b'\xc0':
        print(f'skip {hdr.hex()}')
        continue
    pktlen = ord(ser.read(1))
    if pktlen < 5:
        print(f'!len {pktlen.hex()}')
        continue
    addr = ord(ser.read(1))
    data = ser.read(pktlen - 4)
    check = ord(ser.read(1))
    f = ser.read(1)
    if f != b'\x01':
        print(f'!frame2 {f.hex()}')
        continue
    try:
        computed_check = crc_alg[addr].calc(data)
        if check != computed_check:
            print(f'!CRC got {check:#x}')
    except KeyError:
        print(f'!addr {addr}')
    print(f'{addr}:{data.hex()}')

    if addr == 1:
        mode, _, _, set_temp, coolant_temp, _, _, rpm = struct.unpack('>BBBBHBBH', data)
        print(f'mode {power_modes[mode]}  set {set_temps[set_temp]}°C  coolant {coolant_temp / 100}°C  compressor {rpm}rpm')
