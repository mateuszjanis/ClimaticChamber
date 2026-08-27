#include "PeltierController.h"
#include "ControlSensors.h"

int main() {

    auto chip = ::gpiod::chip(CHIP_PATH);
    auto request = chip.prepare_request()
        .set_consumer(CONSUMER)
        .add_line_settings(
            ALL_OFFSETS,
            ::gpiod::line_settings()
                .set_direction(::gpiod::line::direction::OUTPUT)
                .set_output_value(::gpiod::line::value::ACTIVE)
        ).do_request();

    updateSensors();
    printSensors();

    setCooling(request);
    updateSensors();
    printSensors();
    std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));
    
    setHeating(request);
    updateSensors();
    printSensors();
    std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));

    setIdle(request);
    updateSensors();
    printSensors();

    return 0;
}