#include "PeltierController.h"
#include "DataHandler.h"
#include <thread>
#include <nlohmann/json.hpp>

#define CHIP_PATH "/dev/gpiochip4"
#define CONSUMER "chamber_rpi5"

// const char* sftp_climate_path = "sftp://student.agh.edu.pl/home/imirgrp/matjanis/public_html/ChamberData.csv";
// const char* sftp_passwd = "matjanis:Kezi!de5to";

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

    /*
    // set up SFTP server client
    // 
    // curl_global_init(CURL_GLOBAL_DEFAULT);
    // CURL *curl = curl_easy_init();
    // 
    // if(curl) {
    //     curl_easy_setopt(curl, CURLOPT_URL, sftp_climate_path );
    //     curl_easy_setopt(curl, CURLOPT_USERPWD, sftp_passwd);
    //
    //     std::cout << "CURL ready!\n";
    // }
    // else {
    //     std::cout << "CURL wrong!\n";
    // }
    */

    PeltierController peltierController(request, configuration_json);
    DataHandler dataHandler(configuration_json);

    std::thread sftpThread(&DataHandler::run, &dataHandler);

    while (true) {
        
        peltierController.runTemperatureControl();
        
    }

    return 0;
}

void parseConfigurationFile(nlohmann::json& configuration_json){
    
//    std::cout << "Attepmting opening json\n";
    
    std::fstream configuration_file;
    configuration_file.open(R"(../configuration.json)", std::ios::in);

    configuration_json = nlohmann::json::parse(configuration_file);

//    std::cout << "Configuration files parsed!\n";
//    std::cout << "Trying reading json:\n";
//    std::cout << configuration_json["paths"]["climate_data_file_path"].get<std::string>() << std::endl;
//    std::cout << configuration_json["paths"]["image_dir_path"].get<std::string>() << std::endl;


}

/*
void createConfigurationESP(){

}

*/
