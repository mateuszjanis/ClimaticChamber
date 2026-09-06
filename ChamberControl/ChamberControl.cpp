#include "PeltierController.h"
#include "ControlSensors.h"
#include <thread>

#define CHIP_PATH "/dev/gpiochip4"
#define CONSUMER "chamber_rpi5"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------- PINS -----------------------------------//
////////////////////////////////////////////////////////////////////////////////

const int HEAT_PIN_1 = 17;
const int HEAT_PIN_2 = 23;
const int COOL_PIN_1 = 24;
const int COOL_PIN_2 = 27;
const int FAN_PIN = 16;

gpiod::line::offsets COOL_OFFSETS = {COOL_PIN_1, COOL_PIN_2};
gpiod::line::offsets HEAT_OFFSETS = {HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets ALL_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets FAN_OFFSET = {FAN_PIN};
gpiod::line::offsets INIT_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2, FAN_PIN};


int main() {

    // set up GPIO chip and request lines

    auto chip = ::gpiod::chip(CHIP_PATH);
    auto request = chip.prepare_request()
        .set_consumer(CONSUMER)
        .add_line_settings(
            INIT_OFFSETS,
            ::gpiod::line_settings()
                .set_direction(::gpiod::line::direction::OUTPUT)
                .set_output_value(::gpiod::line::value::ACTIVE)
        ).do_request();

    // std::thread sensorsThread(ReciveSensorsData);
    // std::thread sftpThread(); only sending or also receiving? if only sending, then no need for a thread, just call the function in the main loop

    PeltierController peltierController(request, 22.0);

    while (true) {

        peltierController.runTemperatureControl();
        printSensors();
        
    }

    return 0;
}