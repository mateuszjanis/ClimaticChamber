#pragma once
#include "ControlSensors.h"
#include <curl/curl.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <thread>
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#include <nlohmann/json.hpp>
#include <system_error>
#include <vector>
#include <filesystem>
// #include filesystem needed???

class DataHandler{

    const std::string data_dir;
    const std::string climate_filename;
    
    const std::string esp_path;
    const std::string gas_filename;
    const std::string images_dir;
    const std::string take_photo_script;

    std::string day_dir;

    const std::chrono::minutes climate_data_record_interval;
    const std::chrono::minutes gas_data_record_interval;
    const std::chrono::minutes image_record_interval;

    // server 

    std::string gas_data_payload;
    std::string climate_data_payload;
    std::vector<std::string> photo_payload; // should include day_dir/Images/photo_name
                                            // or use last_date in run()

    const std::string sftp_path;
    const std::string passwd_file_path;
    const std::string sftp_passwd;

public:

    DataHandler(nlohmann::json &config_json) : 
        data_dir(           config_json["paths"]["data_dir"].get<std::string>()),
        climate_filename(   config_json["paths"]["climate_filename"].get<std::string>()),
        esp_path(           config_json["paths"]["esp_path"].get<std::string>()),
        gas_filename(       config_json["paths"]["gas_filename"].get<std::string>()),
        images_dir(         config_json["paths"]["images_dir"].get<std::string>()),
        take_photo_script(  config_json["paths"]["take_photo_script"].get<std::string>()),

        day_dir("Data_" + getDateString()),

        climate_data_record_interval(   std::chrono::minutes(config_json["settings"]["climate_data_record_interval"])),
        gas_data_record_interval(       std::chrono::minutes(config_json["settings"]["gas_data_record_interval"])),
        image_record_interval(          std::chrono::minutes(config_json["settings"]["image_record_interval"])),

        gas_data_payload(""),
        climate_data_payload(""),
        photo_payload(std::vector<std::string>()),

        sftp_path(config_json["paths"]["sftp_path"].get<std::string>()),
        passwd_file_path(config_json["paths"]["passwd_file_path"].get<std::string>()),
        sftp_passwd(parseENV())
        {

            std::cout << "[DATA] DataHandler initialized\n";
        
    };

    void run();

private:

    // void appendClimateDataLocally(std::string& climate_payload);
    // void appendGasDataLocally(std::string& gas_payload);
    void appendDataLocally(std::string& payload_loc, char type);
    std::string getTimeString();
    std::string getDateString();
    std::string getClimatePayload();
    std::string getGasPayload();
    std::string getImageFileName();

    void actualizeDate(std::string new_date);
    void takePhoto(std::string image_file_path);
    void createNecessaryDirectoriesAndFiles();
    void deleteDayPhotos(std::string& last_date);

    // server functions
    std::string parseENV();

    void appendDataToServer(std::string& payload, char type);
    void sendPhoto(std::string& image_to_send);

    void createMissingFiles();
    bool fileExistsOnServer(CURL* curl);
};

    
