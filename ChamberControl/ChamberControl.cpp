#include "PeltierController.h"
#include "DataRecording.h"
// #include "ControlSensors.h"
#include <thread>
#include <curl/curl.h>

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

    // set up SFTP server client

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, sftp_file_path );
        curl_easy_setopt(curl, CURLOPT_USERPWD, user_psswd);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    }

    std::thread sftpThread(runDataRecording, curl);

    PeltierController peltierController(request, 28);

    while (true) {
        
        peltierController.runTemperatureControl();
        
    }

    return 0;
}
