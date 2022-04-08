
#pragma once

#include <uart.h>
#include "defs.h"

namespace RS485
{
void
init()
{
	// UART setup
	UART0_RXD.claim_pin(RXD);
	UART0_TXD.claim_pin(TXD);
	UART0.configure(115200);        // XXX autobaud? other versions use 9600bps

	// Turn on the RS-485 receiver
	RXTX.configure(Pin::Output, Pin::PushPull).set(0);
}

bool
recv(unsigned &c)
{
    return UART0.recv(c);
}

void
send(const Chillout::Command *cmd)
{
    if (cmd) {
        // switch to claim the line
        RXTX.set(1);
        Timer1.delay(USEC(200));

        // send the command
        for (const auto &c : cmd->bytes) {
            UART0.send(c);
        }

        // wait for the command to finish sending
        while (!UART0.txidle());

        // and release the line
        Timer1.delay(USEC(200));
        RXTX.set(0);
    }
}
}
