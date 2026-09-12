#include "PeltierController.h"
// #include "ControlSensors.h"
#include <thread>

#define CHIP_PATH "/dev/gpiochip4"
#define CONSUMER "chamber_rpi5"

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

    // initial sensors readings
    updateSensors();

    std::thread sensorsThread(ReciveSensorsData);
    // std::thread sftpThread(); only sending or also receiving? if only sending, then no need for a thread, just call the function in the main loop

    PeltierController peltierController(request, 22.0);

    while (true) {

        peltierController.runTemperatureControl();
        printSensors();
        
    }

    return 0;
}