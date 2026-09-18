#include "PeltierController.h"
#include "DataRecording.h"
// #include "ControlSensors.h"
#include <thread>
#include <curl/curl.h>

#define CHIP_PATH "/dev/gpiochip4"
#define CONSUMER "chamber_rpi5"

const char* sftp_file_path ="sftp://student.agh.edu.pl/home/imirgrp/matjanis/public_html/ChamberData.csv";
const char* user_psswd = "matjanis:Kezi!de5to";

int main() {

    double temp_sens = 2.0; // degrees celcius
    double toggle_treshold = 4; //degrees celcius
    double fan_treshold = 25; //degrees celcius
    
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

    std::cout << "Pins ready!\n";

    // set up SFTP server client

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, sftp_file_path );
        curl_easy_setopt(curl, CURLOPT_USERPWD, user_psswd);

        std::cout << "CURL ready!\n";
    }
    else {
        std::cout << "CURL wrong!\n";
    }

    PeltierController peltierController(request, 15, temp_sens, toggle_treshold, fan_treshold);

    std::thread sftpThread(runDataRecording, curl);

    while (true) {
        
        peltierController.runTemperatureControl();
        
    }

    return 0;
}
