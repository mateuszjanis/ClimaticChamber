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

    const std::string climate_data_file_path;
    // const std::string gas_data_file_path;
    const std::string image_dir_path;

    const std::chrono::minutes climate_data_record_interval; // może byc kilka
    // const std::chrono::minutes gas_data_record_interval;
    const std::chrono::minutes image_record_interval;

    ServerHandler serverSFTP;

public:

    DataHandler(nlohmann::json &config_json) : 
        climate_data_file_path(config_json["paths"]["climate_data_file_path"].get<std::string>().c_str()),
        image_dir_path(config_json["paths"]["image_dir_path"].get<std::string>()),
        climate_data_record_interval(std::chrono::minutes(config_json["settings"]["climate_data_record_interval"])),
        image_record_interval(std::chrono::minutes(config_json["settings"]["image_record_interval"])),
        serverSFTP(config_json["paths"]["sftp_path"].get<std::string>().c_str(), 
				   config_json["paths"]["sftp_file_path"].get<std::string>(), 
				   config_json["paths"]["sftp_image_path"].get<std::string>())
				   
        {std::cout << "[DATA] DataHandler initialized\n";} ; // gas_data_file_path; -- dopisać w jsonie

    void run();

private:

    void appendClimateDataLocally();
    // void appendGasDataLocally();
    std::string getTimeString();
    std::string getDateString();
    std::string getClimatePayload();
    // std::string getGasPayload();
    // std::string getGasData();

};

    
