#pragma once
#include "ControlSensors.h"
#include "ServerHandler.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <thread>
#include <chrono>
#include <ctime>
#include <sys/stat.h>
#include <nlohmann/json.hpp>

class DataHandler{

    const std::string data_dir;
    const std::string climate_filename;
    // const std::string gas_filename;
    const std::string images_dir;
    const std::string take_photo_script;

    std::string day_dir;

    const std::chrono::minutes climate_data_record_interval; // może byc kilka
    // const std::chrono::minutes gas_data_record_interval;
    const std::chrono::minutes image_record_interval;

    ServerHandler serverSFTP;

public:

    DataHandler(nlohmann::json &config_json) : 
        data_dir(config_json["paths"]["data_dir"].get<std::string>()),
        climate_filename(config_json["paths"]["climate_filename"].get<std::string>()),
        // gas_filename(config_json["paths"]["gas_filename"].get<std::string>()),
        images_dir(config_json["paths"]["images_dir"].get<std::string>()),
        take_photo_script(config_json["paths"]["take_photo_script"].get<std::string>()),

        day_dir("Data_" + getDateString()),

        climate_data_record_interval(std::chrono::minutes(config_json["settings"]["climate_data_record_interval"])),
        // gas_data_record_interval(std::chrono::minutes(config_json["settings"]["gas_data_record_interval"])),
        image_record_interval(std::chrono::minutes(config_json["settings"]["image_record_interval"])),
        
        serverSFTP(config_json["paths"]["sftp_path"].get<std::string>().c_str(), 
				   config_json["paths"]["passwd_file_path"].get<std::string>(), 
				   data_dir, climate_filename,
                   // gas_filename,
                   images_dir, day_dir) 
        {

            std::cout << "[DATA] DataHandler initialized\n";
        
        };

    void run();

private:

    void appendClimateDataLocally();
    // void appendGasDataLocally();
    std::string getTimeString();
    std::string getDateString();
    std::string getClimatePayload();
    std::string getImageFileName();
    void actualizeDate(std::string new_date);
    // std::string getGasPayload();
    // std::string getGasData();
    void actualizeDate(std::string new_date);
    void takePhoto(std::string image_file_path);

};

    
