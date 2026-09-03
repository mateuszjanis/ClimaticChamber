#include "PeltierController.h"
#include "ControlSensors.h"
#include <thread>

void handleInput(::gpiod::line_request &request) {
    
    bool is_temp_ok = false;
    
    while(!is_temp_ok) {

        double received_goal_temp = 0.0;
        std::cout << "Wpisz docelowa temperature:" << std::endl;
        std::cin >> received_goal_temp;

        if (received_goal_temp < 0.0 || received_goal_temp > 100.0) {
            std::cout << "Nieprawidlowa temperatura. Wpisz wartosc z zakresu 0-100." << std::endl;
        }
        else {
            std::cout << "Otrzymana temperatura: " << received_goal_temp << std::endl;
            goal_temperature = received_goal_temp;
            is_temp_ok = true;
        }
    }
}

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

    handleInput(request);
    calculateTemperatureControlParameters();

    while (true) {

        runTemperatureControl(request);
        printSensors();
        
    }

    return 0;
}