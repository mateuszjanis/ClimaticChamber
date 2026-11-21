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

    std::string gas_data_payload;
    std::string climate_data_payload;
    std::vector<std::string> photo_payload;
    
    const std::string sftp_path;
    const std::string sftp_username;
    const std::string passwd_file_path;
    const std::string data_dir;
    const std::string images_dir;

    std::string day_dir;

public:

    ServerHandler(const std::string sftp_path, /*const std::string sftp_username,*/ const std::string passwd_file_path, const std::string data_dir, 
                    const std::string images_dir, std::string day_dir) : 
                        
                        climate_data_payload(""),
                        photo_payload({}),
                                            
                        sftp_path(sftp_path),
//                        sftp_username(sftp_username),
                        passwd_file_path(passwd_file_path),
                        data_dir(data_dir), 
                        images_dir(images_dir),
						day_dir(day_dir) {
			
			std::string sftp_passwd = parseENV();
	
			std::cout << "[SERVER]" << std::endl;
			std::cout << "Path: " << sftp_path << std::endl;
//			std::cout << "Username:  " << sftp_username << std::endl;
			std::cout << "Password: " << sftp_passwd << std::endl;			
		
			curl_global_init(CURL_GLOBAL_DEFAULT);
			curl = curl_easy_init();

			if(curl) {
				curl_easy_setopt(curl, CURLOPT_URL, sftp_path.c_str() );
//				curl_easy_setopt(curl, CURLOPT_USERNAME, sftp_username.c_str());
				curl_easy_setopt(curl, CURLOPT_PASSWORD, sftp_passwd.c_str());
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
				curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);

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

    // bool appendClimateData(std::string& payload);
    // bool appendGasData(std::string& payload);

    bool appendData(std::string& payload, const std::string& filename, char type);

    bool sendPhoto(std::string& image_to_send);
    // bool sendState();
    void actualizeDayDir(std::string& new_day_dir);
    void createMissingFiles();
    bool isPhotoPayloadEmpty() { return photo_payload.empty(); };
    
    // create function to put payload in it and path to append to file ...

private:

    // bool createNewFile();
    // std::string findImageToSend();
    std::string parseENV();

};
