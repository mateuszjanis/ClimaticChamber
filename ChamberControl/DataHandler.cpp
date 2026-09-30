#include "DataHandler.h"

void DataHandler::run(){

    while (getDateString().substr(0, 4) < "2026") {
        std::cout << "Czekam na synchronizacje czasu NTP...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::string current_date = getDateString();
    std::chrono::minutes last_gas_send_time = std::chrono::steady_clock::now();
    std::chrono::minutes last_climate_send_time = std::chrono::steady_clock::now();
    std::chrono::minutes last_image_send_time = std::chrono::steady_clock::now();

    while (true) {

        std::string checked_date = getDateString();
        std::chrono::minutes now = steady_clock::now();

        ServerHandler.sendState();
        
        if (checked_date != current_date) {
            // actForNewDate();
        }

        if (now - last_climate_send_time >= climate_data_record_interval) {

            appendClimateDataLocally();
            ServerHandler.actualizeClimatePayload(getClimatePayload());
            ServerHandler.appendClimateData();
            last_climate_send_time = std::chrono::steady_clock::now(); 
        }

        if (now - last_gas_send_time >= gas_data_record_interval) {
            appendGasDataLocally();
            ServerHandler.actualizeGasPayload(getGasPayload());
            ServerHandler.appendGasData();
            last_gas_send_time = std::chrono::steady_clock::now();
        }

        if (now - last_image_send_time >= image_data_record_interval) {
            ServerHandler.sendImage();
            last_image_send_time = std::chrono::steady_clock::now(); 
        }

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void DataHandler::appendClimateDataLocally(){

    std::ofstream file(climate_data_file_path, std::ios::app);
    
    if (file.is_open()) {
        file << getClimatePayload();
        std::cout << "Climate data appended\n";
    }
    else{
        std::cout << "Climate data NOT appended\n";
    }

}

void DataHandler::appendGasDataLocally(){

    std::ofstream file(gas_data_file_path, std::ios::app);
    
    if (file.is_open()) {
        file << getGasPayload();
        std::cout << "Gas data appended\n";
    }
    else{
        std::cout << "Gas data NOT appended\n";
    }

}

std::string DataHandler::getDateString() {

    std::time_t now = std::time(nullptr);
    std::tm tm_struct;

    localtime_r(&now, &tm_struct); 
    
    char date_buffer[11]; // "YYYY-MM-DD\0" to dokładnie 11 znaków
    std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", &tm_struct);
    
    return std::string(date_buffer);
}

std::string DataHandler::getTimeString(){

    std::time_t now = std::time(nullptr);
    std::tm tm_struct;

    localtime_r(&now, &tm_struct); 
    
    char time_buffer[25];
    std::strftime(time_buffer, sizeof(time_buffer), "%Y.%m.%d %H:%M:%S", time_struct);

    return std::string(time_buffer);
}

std::string DataHandler::getClimatePayload(){
    
    char payload_buffer[128];

    std::snprintf(payload_buffer, sizeof(payload_buffer), "%s;%.2f;%.2f;%.2f;%.2f\n", 
                getTimeString(), 
                temp_mean, 
                hum_mean, 
                pelt_temp_in, 
                pelt_temp_out);

    return std::string(payload_buffer);

}

std::string DataHandler::getGasPayload(){
    
    char payload_buffer[128];

    std::snprintf(payload_buffer, sizeof(payload_buffer), "%s;%s\n", 
                getTimeString(),
                getGasData());

    return std::string(payload_buffer);

}

std::string DataHandler::getGasData(){

    //gas data reading from esp file

}

