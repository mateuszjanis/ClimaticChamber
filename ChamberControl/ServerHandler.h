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
    std::string image_dir_path;
    
    const std::string sftp_file_path;
    const std::string sftp_image_path;

public:

    ServerHandler(const char* sftp_path, std::string file_path, std::string image_path) : climate_data_payload(""), image_dir_path("../Images/"), 
				sftp_file_path(file_path), sftp_image_path(image_path) {

        std::string sftp_passwd = parseENV();

        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl = curl_easy_init(); // może musi być stworzone w main

        if(curl) {
            curl_easy_setopt(curl, CURLOPT_URL, sftp_path );
            curl_easy_setopt(curl, CURLOPT_USERPWD, sftp_passwd.c_str());
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            std::cout << "[SERVER] CURL ready!\n";
        }
        else {
            std::cout << "[SERVER] CURL error!\n";
        }

        std::cout << "[SERVER] ServerHandler initialized\n";
    }

    ~ServerHandler(){
        curl_easy_cleanup(curl);
    }

    bool appendClimateData();
    // bool appendGasData();
    bool sendImage();
    // bool sendState();
    void actualizeClimatePayload(std::string payload);
    // void actualizeGasPayload(std::string payload);

private:

    // bool createNewFile();
    std::string findImageToSend();
    std::string parseENV();

};
