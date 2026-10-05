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
#include <vector>

class ServerHandler{

    CURL *curl;

    // std::string gas_data_payload;
    std::string climate_data_payload;
    std::vector<std::string> photo_payload;
    
    const std::string sftp_path;
    const std::string passwd_file_path;
    const std::string data_dir;
    const std::string climate_filename;
    // const std::string gas_filename;
    const std::string images_dir;

    std::string day_dir;

public:

    ServerHandler(const char* sftp_path, const std::string passwd_file_path, const std::string data_dir, const std::string climate_filename,
                    // const std::string gas_filename,
                    const std::string images_dir, std::string day_dir) : 
                        
                        climate_data_payload(""),
                        photo_payload({}),
                                            
                        sftp_path(sftp_path), 
                        passwd_file_path(passwd_file_path),
                        data_dir(data_dir), 
                        climate_filename(climate_filename),
                        // gas_filename(gas_filename),
                        images_dir(images_dir),
						day_dir(day_dir) {
	
							std::string sftp_passwd = parseENV();
		
			curl_global_init(CURL_GLOBAL_DEFAULT);
			curl = curl_easy_init();

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

    bool appendClimateData(std::string payload);
    // bool appendGasData();
    bool sendPhoto(std::string image_to_send);
    // bool sendState();
    void actualizeClimatePayload(std::string payload);
    // void actualizeGasPayload(std::string payload);
    void actualizeDayDir(std::string new_day_dir);
    void createMissingFiles();
    
    // create function to put payload in it and path to append to file ...

private:

    // bool createNewFile();
    // std::string findImageToSend();
    std::string parseENV();

};
