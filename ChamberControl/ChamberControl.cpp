#include "PeltierController.h"
#include "DataHandler.h"
#include <thread>
#include <nlohmann/json.hpp>

#define CHIP_PATH "/dev/gpiochip4"
#define CONSUMER "chamber_rpi5"

void parseConfigurationFile(nlohmann::json &configuration_json);
// void createConfigurationESP();

int main() {
	
    nlohmann::json configuration_json;
    parseConfigurationFile(configuration_json);
    
    // set up GPIO chip and request lines - przenieść do konstruktora PeltierController

    auto chip = ::gpiod::chip(CHIP_PATH);
    auto request = chip.prepare_request()
        .set_consumer(CONSUMER)
        .add_line_settings(
            INIT_OFFSETS,
            ::gpiod::line_settings()
                .set_direction(::gpiod::line::direction::OUTPUT)
                .set_output_value(::gpiod::line::value::ACTIVE)
        ).do_request();

    std::cout << "Pins ready!\n";


    PeltierController peltierController(request, configuration_json);
    DataHandler dataHandler(configuration_json);

    std::thread sftpThread(&DataHandler::run, &dataHandler);

    while (true) {
        
        peltierController.runTemperatureControl();
        
    }

    return 0;
}

void parseConfigurationFile(nlohmann::json& configuration_json){
    
    std::fstream configuration_file;
    configuration_file.open(R"(../configuration.json)", std::ios::in);

    configuration_json = nlohmann::json::parse(configuration_file);

}

/*
void createConfigurationESP(){

}

*/
