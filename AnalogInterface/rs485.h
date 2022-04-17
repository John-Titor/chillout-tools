// Copyright (c) 2022 Michael Smith, All Rights Reserved
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  o Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  o Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//

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
