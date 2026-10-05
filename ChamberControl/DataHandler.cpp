#include "DataHandler.h"

void DataHandler::run(){

    while (getDateString().substr(0, 4) < "2026") {
        std::cout << "Waiting for time sync with NTP...\n";
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::string current_date = getDateString();

    createNecessaryDirectoriesAndFiles();

    // auto last_gas_send_time = std::chrono::steady_clock::now();
    auto last_climate_send_time = std::chrono::steady_clock::now();
    auto last_image_send_time = std::chrono::steady_clock::now();

    while (true) {

        std::string checked_date = getDateString();
        auto now = std::chrono::steady_clock::now(); // <- przekazywanie tej daty i z niej tworzenie payloadów

        // serverSFTP.sendState();
        
        if (checked_date != current_date) {
            actualizeDate(checked_date);
            current_date = checked_date;
        }

        // pozamykać poniższe funkcje w funckje typu actForClimateData() i actForGasData() i actForImageData()

        if (now - last_climate_send_time >= climate_data_record_interval) {
	
			std::cout << "[DATA] Climate data handle activated!\n";
			last_climate_send_time = std::chrono::steady_clock::now(); 
            appendClimateDataLocally();
            serverSFTP.appendClimateData(getClimatePayload());
            std::cout << "[DATA] Climate data handle completed!\n";
        }

        /*
        if (now - last_gas_send_time >= gas_data_record_interval) {
            appendGasDataLocally();
            serverSFTP.actualizeGasPayload(getGasPayload());
            serverSFTP.appendGasData();
            last_gas_send_time = std::chrono::steady_clock::now();
            std::cout << "Gas data handle activated!\n";
        }
        */
		
        if (now - last_image_send_time >= image_record_interval) {
            
            std::cout << "[DATA] Image handle activated!\n";

            last_image_send_time = std::chrono::steady_clock::now();		

            std::string image_filename = getImageFileName();
            
            std::string image_file_path = data_dir + "/" + day_dir + "/" + images_dir + "/" + image_filename;
            
            takePhoto(image_file_path);
            // serverSFTP.sendPhoto(image_filename);
            if (serverSFTP.sendPhoto(image_filename)) { std::cout << "[DATA] sent correctly\n"; }
            else { std::cout << "[DATA] sent incorrectly\n"; }
            
            std::cout << "[DATA] Image handle completed!\n"; 

        }
		
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void DataHandler::appendClimateDataLocally(){

    std::string climate_data_file_path = data_dir + "/" + day_dir + "/" + climate_filename;
    std::ofstream file(climate_data_file_path, std::ios::app);
    
    if (file.is_open()) {

        file << getClimatePayload();
        std::cout << "[DATA] Climate data appended\n";

    }
    else{

        std::cout << "[DATA] Climate data NOT appended\n";

    }

}

/*
void DataHandler::appendGasDataLocally(){

    std::string gas_data_file_path = data_dir + "/" + day_dir + "/" + gas_filename;
    std::ofstream file(gas_data_file_path, std::ios::app);
    
    if (file.is_open()) {
        file << getGasPayload();
        std::cout << "[DATA] Gas data appended\n";
    }
    else{
        std::cout << "[DATA] Gas data NOT appended\n";
    }

}
*/

std::string DataHandler::getDateString() {

    std::time_t now = std::time(nullptr);
    std::tm tm_struct;

    localtime_r(&now, &tm_struct); 
    
    char date_buffer[11]; // "YYYY-MM-DD\0"
    std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d", &tm_struct);
    
    return std::string(date_buffer);
}

std::string DataHandler::getTimeString(){

    std::time_t now = std::time(nullptr);
    std::tm tm_struct;

    localtime_r(&now, &tm_struct); 
    
    char time_buffer[25];
    std::strftime(time_buffer, sizeof(time_buffer), "%Y.%m.%d %H:%M:%S", &tm_struct);

    return std::string(time_buffer);
}

std::string DataHandler::getClimatePayload(){
    
    char payload_buffer[128];

    std::snprintf(payload_buffer, sizeof(payload_buffer), "%s;%.2f;%.2f;%.2f;%.2f\n", 
                getTimeString().c_str(), 
                hum_mean,
                temp_mean,
                pelt_temp_in, 
                pelt_temp_out);

    return std::string(payload_buffer);

}

std::string DataHandler::getImageFileName() {

    std::time_t now = std::time(nullptr);
    std::tm tm_struct;

    localtime_r(&now, &tm_struct); 
    
    char date_buffer[25]; // "YYYY-MM-DD_HH-MM-SS\0"
    std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d_%H-%M-%S", &tm_struct);
    
    return std::string("image_") + std::string(date_buffer) + std::string(".jpg");
}

/*
std::string DataHandler::getGasPayload(){
    
    char payload_buffer[128];

    std::snprintf(payload_buffer, sizeof(payload_buffer), "%s;%s\n", 
                getTimeString().c_str(),
                getGasData());

    return std::string(payload_buffer);

}

std::string DataHandler::getGasData(){

    //gas data reading from esp file

}
*/

void DataHandler::actualizeDate(std::string new_date){

    day_dir = "Data_" + new_date;
    serverSFTP.actualizeDayDir(day_dir);
    createNecessaryDirectoriesAndFiles();
    std::cout << "New date detected: " << new_date << "\n";

}

void DataHandler::takePhoto(std::string image_file_path){

    std::string command = "./" + take_photo_script + " '" + image_file_path + "'";
    int result = std::system(command.c_str());

}

void DataHandler::createNecessaryDirectoriesAndFiles() {		// <<------- ADD HERE CREATING FILE "HEADERS" ON SERVER!!!!!

    std::string day_dir_path = data_dir + "/" + day_dir;
    std::string images_dir_path = day_dir_path + "/" + images_dir;
    std::string climate_file_path = day_dir_path + "/" + climate_filename;

    // The same for gas file in the future

    // Create the day directory if it doesn't exist
    if (!std::filesystem::exists(day_dir_path)) {
        std::filesystem::create_directories(day_dir_path);
        std::cout << "[DATA] Created directory: " << day_dir_path << "\n";
    }

    // Create the images directory if it doesn't exist
    if (!std::filesystem::exists(images_dir_path)) {
        std::filesystem::create_directories(images_dir_path);
        std::cout << "[DATA] Created directory: " << images_dir_path << "\n";
    }

    // Create the data file if it doesn't exist
    if (!std::filesystem::exists(climate_file_path)) {
        std::ofstream climate_file(climate_file_path);
        climate_file << "Timestamp;Humidity;Temperature;Peltier_In;Peltier_Out\n";
        climate_file.close();
        std::cout << "[DATA] Created file: " << climate_file_path << "\n";
    }
    
    serverSFTP.createMissingFiles();
}
