#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "input.h"
#include "chillout.h"

TEST_CASE("Input") {
    // reset the input parser
    Input::current_target = Input::OFF;
    Input::samples = 0;
    Input::count = 0;
    Input::duty_cycle = 0;

    SUBCASE("sample increments state") {
        Input::sample(false);
        Input::sample(true);
        CHECK(Input::count == 1);
        CHECK(Input::samples == 2);
    }

    SUBCASE("zero duty cycle") {
        CHECK(Input::target() == Input::OFF);
        for (auto i = 0U; i < 1000; i++) {
            Input::sample(false);
        }
        CHECK(Input::duty_cycle == 0);
        CHECK(Input::target() == Input::OFF);
    }

    SUBCASE("100%% duty cycle") {
        CHECK(Input::target() == Input::OFF);
        for (auto i = 0U; i < 1000; i++) {
            Input::sample(true);
        }
        CHECK(Input::duty_cycle == 100);
        CHECK(Input::target() == Input::MIN);
        CHECK(Input::target() == (Input::MIN + 1));
        CHECK(Input::target() == (Input::MIN + 2));
        CHECK(Input::target() == (Input::MIN + 3));
        CHECK(Input::target() == (Input::MIN + 4));
        CHECK(Input::target() == (Input::MIN + 5));
        CHECK(Input::target() == (Input::MIN + 6));
        CHECK(Input::target() == (Input::MIN + 7));
        CHECK(Input::target() == (Input::MIN + 8));
        CHECK(Input::target() == (Input::MAX));
        CHECK(Input::target() == (Input::MAX));
    }

    SUBCASE("50%% duty cycle") {
        CHECK(Input::target() == Input::OFF);
        for (auto i = 0U; i < 1000; i += 2) {
            Input::sample(true);
            Input::sample(false);
        }
        CHECK(Input::duty_cycle == 50); 
        CHECK(Input::target() == Input::MIN);
        CHECK(Input::target() == (Input::MIN + 1));
        CHECK(Input::target() == (Input::MIN + 2));
        CHECK(Input::target() == (Input::MIN + 3));
        CHECK(Input::target() == (Input::MIN + 3));
    }
    SUBCASE("target downward") {
        for (auto i = 0U; i < 1000; i++) {
            Input::sample(true);
        }
        CHECK(Input::duty_cycle == 100);
        CHECK(Input::target() == Input::MIN);
        CHECK(Input::target() == (Input::MIN + 1));
        CHECK(Input::target() == (Input::MIN + 2));
        CHECK(Input::target() == (Input::MIN + 3));
        CHECK(Input::target() == (Input::MIN + 4));
        CHECK(Input::target() == (Input::MIN + 5));
        CHECK(Input::target() == (Input::MIN + 6));
        CHECK(Input::target() == (Input::MIN + 7));
        CHECK(Input::target() == (Input::MIN + 8));
        CHECK(Input::target() == Input::MAX);
        for (auto i = 0U; i < 1000; i++) {
            Input::sample(false);
        }
        CHECK(Input::duty_cycle == 0);
        CHECK(Input::target() == (Input::MIN + 8));
        CHECK(Input::target() == (Input::MIN + 7));
        CHECK(Input::target() == (Input::MIN + 6));
        CHECK(Input::target() == (Input::MIN + 5));
        CHECK(Input::target() == (Input::MIN + 4));
        CHECK(Input::target() == (Input::MIN + 3));
        CHECK(Input::target() == (Input::MIN + 2));
        CHECK(Input::target() == (Input::MIN + 1));
        CHECK(Input::target() == Input::MIN);
        CHECK(Input::target() == Input::OFF);
        CHECK(Input::target() == Input::OFF);
    }
}

TEST_CASE("Chillout") {
    Chillout::mode = 0;
    Chillout::parse_state = Chillout::WAIT_HEADER;

    SUBCASE("parse errors") {
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);

        CHECK(Chillout::recv(0xff) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);

        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0xff) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);

        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0x0e) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_ADDRESS);
        CHECK(Chillout::recv(0xff) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);

        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0x0e) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_ADDRESS);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_MODE);
        CHECK(Chillout::recv(0xff) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);

        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0x0e) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_ADDRESS);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_MODE);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_SETPOINT);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);

        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0x0e) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_ADDRESS);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_MODE);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_SETPOINT);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);

        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0x0e) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_ADDRESS);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_MODE);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_SETPOINT);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x01) == true);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);
    }

    SUBCASE("parse results") {
        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0x0e) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_ADDRESS);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_MODE);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_SETPOINT);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x01) == true);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);
        CHECK(Chillout::mode == 0);
        CHECK(Chillout::setting == 10);

        CHECK(Chillout::update_index(Input::OFF) == Chillout::SET_NONE);
        CHECK(Chillout::update_index(Input::MIN) == Chillout::SET_ON);

        CHECK(Chillout::recv(0xc0) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_LENGTH);
        CHECK(Chillout::recv(0x0e) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_ADDRESS);
        CHECK(Chillout::recv(0x01) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_MODE);
        CHECK(Chillout::recv(0x03) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::parse_state == Chillout::WAIT_SETPOINT);
        CHECK(Chillout::recv(0x0a) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x00) == false);
        CHECK(Chillout::recv(0x01) == true);
        CHECK(Chillout::parse_state == Chillout::WAIT_HEADER);
        CHECK(Chillout::mode == (Chillout::MODE_ON | Chillout::MODE_MAX));
        CHECK(Chillout::setting == 1);

        CHECK(Chillout::update_index(Input::OFF) == Chillout::SET_OFF);
        CHECK(Chillout::update_index(Input::MIN) == Chillout::SET_NONE);
        CHECK(Chillout::update_index(Input::MAX) == (Chillout::SET_ON + 10));
    }
}
