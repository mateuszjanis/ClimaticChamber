#include "PeltierController.h"
#include "ControlSensors.h"

int main() {

    // 1. Otwarcie układu GPIO
    auto chip = ::gpiod::chip(CHIP_PATH);

    // 2. Konfiguracja stan bezczynny
    auto request = chip.prepare_request()
        .set_consumer(CONSUMER)
        .add_line_settings(
            ALL_OFFSETS,
            ::gpiod::line_settings()
                .set_direction(::gpiod::line::direction::OUTPUT)
                .set_output_value(::gpiod::line::value::ACTIVE)
        ).do_request();

    update_sensors();
    print_sensors();

    setCooling(request);
    update_sensors();
    print_sensors();
    std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));
    
    setHeating(request);
    update_sensors();
    print_sensors();
    std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));

    setIdle(request);
    update_sensors();
    print_sensors();

    return 0;
}