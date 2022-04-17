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

// Input PWM -> target cooling level (OFF..MAX)
//
namespace Input
{
enum {
    OFF = 0,
    MIN = 1,
    MAX = 10,
};

// current / next state tracking
unsigned        current_target = OFF;
const unsigned  windows[MAX + 1][2] = {
    {0, 1},
    {2, 18},
    {15, 34},
    {31, 48},
    {45, 60},
    {58, 71},
    {69, 80},
    {79, 88},
    {86, 93},
    {92, 98},
    {97, 100}
};

volatile unsigned duty_cycle;
volatile unsigned samples;
volatile unsigned count;

// 1ms timer callback, samples input 500 time and updates
// duty_cycle accordingly.
//
void
sample(bool pin_state)
{

    if (pin_state) {
        count++;
    }
    if (samples++ >= 500) {
        duty_cycle = count / 5;
        count = 0;
        samples = 0;
    }
}

// map duty cycle to 0-10 target cooling level
unsigned
target()
{
    if ((current_target > OFF) &&                           // could we go lower?
            (duty_cycle < windows[current_target][0])) {        // should we go lower?
        current_target--;
    } else if ((current_target < MAX) &&                    // could we go higher?
               (duty_cycle > windows[current_target][1])) {   // should we go higher?
        current_target++;
    }

    return current_target;
}
}
