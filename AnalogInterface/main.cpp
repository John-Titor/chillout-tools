// Overview
// ========
//
// Sample the ~40Hz PWM signal generated by the Cool Shirt pump controller
// and command a Chillout Quantum V3 compressor accordingly. Other compressor
// models are likely similar, but this is the one we need to talk to.
//
// The controller generates a very low duty cycle (~1%) at minimum setting,
// so it is possible to distinguish between "off" and "minimum".
//
// System configuration:
// =====================
//
// Core/AHB clock: 24MHz
// UART: RS-485
// Timer0: input poll @ 1kHz
// Timer1: inline delays
// Timer2: LED flash
// Timer3: receive timeout
//

#define RXD     P0_0    // RS-485 receive
#define TXD     P0_4    // RS-485 send
#define RXTX    P0_1    // RS-485 transciever control: low = receive, high = send
#define IN      P0_2    // Input current sensor: low = no current, high = current
#define LED     P0_3    // LED: low = on, high = off

#include <sysctl.h>
#include <pin.h>
#include <timer.h>
#include <sct.h>
#include <interrupt.h>

#include "input.h"
#include "chillout.h"
#include "rs485.h"
#include "statusled.h"

extern "C" int main();

void
sample_tick()
{
   Input::sample(IN);
}

void
led_tick()
{
    StatusLED::tick(LED);
}

int
main()
{
    Sysctl::init_24MHz();

    // Make reset pin work for easier in-system flashing.
    RST.enable();

    // Setup input pin
    IN.configure(Pin::Input, Pin::PullDown);

    // Serial interface up.
    RS485::init();

    // 1ms timer callback to sample input.
    Timer0.configure(sample_tick, MSEC(1), Timer::periodic);

    // Turn on the LED
    LED.configure(Pin::Output, Pin::PushPull).set(0);

    // 250ms LED timer tick
    Timer2.configure(led_tick, MSEC(125), Timer::periodic);

    // main loop
    for (;;) {
        unsigned c;

        // Poll for status packet bytes; once a packet has been completely received
        // use the idle window to possibly send an update to adjust the compressor
        // settings.
        //
        if (RS485::recv(c)) {
            if (Chillout::recv(c)) {

                // compressor needs update to match input?
                RS485::send(Chillout::update_command(Input::target()));

                // update LED
                if (Chillout::mode & Chillout::MODE_ON) {
                    StatusLED::mode = StatusLED::ON;
                } else {
                    StatusLED::mode = StatusLED::OFF;
                }

                // reset comms timeout timer
                Timer3.configure(MSEC(2000));
            }
        }

        // Check for comms timeout
        if (Timer3.expired()) {
            StatusLED::mode = StatusLED::ERROR;
        }
    }
}
