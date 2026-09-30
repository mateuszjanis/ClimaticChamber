#pragma once
#include "ControlSensors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <ctime>
#include <sys/stat.h>

class ServerHandler{

    CURL *curl;

    // std::string gas_data_payload;
    std::string climate_data_payload;

public:

    ServerHandler(const char* sftp_path) : climate_data_payload(""), sftp_file_path(sftp_path) {

        const char* sftp_passwd = parseENV();

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init(); // może musi być stworzone w main

        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, sftp_file_path );
            curl_easy_setopt(curl, CURLOPT_USERPWD, sftp_passwd);

            std::cout << "CURL ready!\n";
        }
        else {
            if(result != CURLE_OK)
                printf("CURL error: %s\n", curl_easy_strerror(result));
        }
    }
    ~ServerHandler(){
        curl_easy_cleanup();
    }

    bool appendClimateData();
    // bool appendGasData();
    bool sendImage();
    // bool sendState();

private:

    // bool createNewFile();
    void actualizeClimatePayload(std::string payload);
    // void actualizeGasPayload(std::string payload);
    std::string findImageToSend();
    const char* parseENV();

}
