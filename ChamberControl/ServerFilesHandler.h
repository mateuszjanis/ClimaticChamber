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

class ServerFilesHandler{

    CURL *curl;

    const std::string climate_data_file_path;
    const std::string gas_data_file_path;
    const std::string image_dir_path;

    const unsigned int climate_data_record_interval; // może byc kilka
    const unsigned int gas_data_record_interval;
    const unsigned int image_record_interval;

    std::string data_payload;

public:

    ServerFilesHandler(){
        
        data_file_path          // 
        image_dir_path          // get from json file
        data_record_interval    //

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

    ~ServerFilesHandler(){
        curl_easy_cleanup();
    }

    void appendData();
    bool sendImage();

private:

    bool sendState();
    bool createNewFile();
    void actualizePayload();
    bool appendClimateData();
    bool appendGasData();
    std::string findImageToSend();

}

void runDataRecording();  