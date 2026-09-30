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

    ServerHandler ServerHandler;

public:

    DataHandler(nlohmann::json &config_json) : ServerHandler(config_json["path"]["sftp_file_path"]){
        
        climate_data_file_path = config_json["path"]["climate_data_file_path"];
        // gas_data_file_path; -- dopisać w jsonie
        image_dir_path = config_json["path"]["image_dir_path"];//      być może zbędne - za każdego dnia inny folder

        climate_data_record_interval = std::chrono::minutes(config_json["settings"]["climate_data_record_interval"]);
        // gas_data_record_interval = std::chrono::minutes();
        image_record_interval = std::chrono::minutes(config_json["settings"]["image_record_interval"]);

    } 
    
    void run();

private:

    void appendClimateDataLocally();
    // void appendGasDataLocally();
    std::string getTimeString();
    std::string getDateString();
    std::string getClimatePayload();
    // std::string getGasPayload();
    // std::string getGasData();

}

    