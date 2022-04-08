#pragma once

namespace StatusLED
{
// LED bit patterns; LSB to MSB, one bit per tick.
enum {
    ERROR = 0x55,
    OFF = 0x01,
    ON = 0xff,
};

unsigned mode = ERROR;
unsigned phase = 0;

void
tick(Pin led)
{
    led.set(!(mode & (1U << phase++)));

    if (phase >= 8) {
        phase = 0;
    }
}
}
