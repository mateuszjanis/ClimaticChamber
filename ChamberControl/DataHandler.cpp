#include "DataHandler.h"

void DataHandler::run(){

    std::string current_date = getDateString();

    createNecessaryDirectoriesAndFiles();

    // auto last_gas_send_time = std::chrono::steady_clock::now();
    auto last_gas_send_ftime = std::filesystem::last_write_time((esp_path + "/" + gas_filename).c_str());
    auto last_climate_send_time = std::chrono::steady_clock::now();
    auto last_image_send_time = std::chrono::steady_clock::now();

    std::string last_date = "";

    while (true) {

        std::string checked_date = getDateString();
        auto now = std::chrono::steady_clock::now(); // <- przekazywanie tej daty i z niej tworzenie payloadów

        // sendState();
        
        if (checked_date != current_date) {
            actualizeDate(checked_date);
            last_date = current_date;
            current_date = checked_date;
        }

        // pozamykać poniższe funkcje w funckje typu actForClimateData() i actForGasData() i actForImageData()

        if (now - last_climate_send_time >= climate_data_record_interval) {
	
			std::cout << "[DATA] Climate data handle activated!\n";
			last_climate_send_time = std::chrono::steady_clock::now();
            
            std::string climate_payload = getClimatePayload();
            appendDataLocally(climate_payload, 'c');
            appendDataToServer(climate_payload, 'c');

            std::cout << "[DATA] Climate data handle completed!\n";
        }

        // check if file has changed
        std::error_code ec;
        auto last_gas_modified_ftime = std::filesystem::last_write_time((esp_path + "/" + gas_filename).c_str(), ec);

        if (!ec && (last_gas_modified_ftime > last_gas_send_ftime)) {
	
			std::cout << "[DATA] Gas data handle activated!\n";
			last_gas_send_ftime = last_gas_modified_ftime;

            std::string gas_payload = getGasPayload();
            appendDataLocally(gas_payload, 'g');
            appendDataToServer(gas_payload, 'g');

            std::cout << "[DATA] Gas data handle completed!\n";
        }
		
        if (now - last_image_send_time >= image_record_interval) {
            
            std::cout << "[DATA] Image handle activated!\n";

            last_image_send_time = std::chrono::steady_clock::now();		

            std::string image_filename = getImageFileName();
            
            std::string image_file_path = day_dir + "/" + images_dir + "/" + image_filename;
            
            takePhoto( data_dir + "/" + image_file_path );
            sendPhoto(image_file_path);

            if (!last_date.empty() && photo_payload.empty()){

                deleteDayPhotos(last_date);
                last_date.erase(last_date.begin());

            }
            
            std::cout << "[DATA] Image handle completed!\n"; 

        }
		
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

// void DataHandler::appendClimateDataLocally(std::string& climate_payload){

//     std::string climate_data_file_path = data_dir + "/" + day_dir + "/" + climate_filename;
//     std::ofstream file(climate_data_file_path, std::ios::app);
    
//     if (file.is_open()) {

//         file << climate_payload;
//         std::cout << "[DATA] Climate data appended\n";

//     }
//     else{

//         std::cout << "[DATA] Climate data NOT appended\n";

//     }

// }

// void DataHandler::appendGasDataLocally(std::string& gas_payload){

//     std::string gas_data_file_path = data_dir + "/" + day_dir + "/" + gas_filename;
//     std::ofstream file(gas_data_file_path, std::ios::app);
    
//     if (file.is_open()) {
//         file << gas_payload;
//         std::cout << "[DATA] Gas data appended\n";
//     }
//     else{
//         std::cout << "[DATA] Gas data NOT appended\n";
//     }

// }

// both upper functions merged here
void DataHandler::appendDataLocally(std::string& payload_loc, char type){

    std::string data_file_path = data_dir + "/" + day_dir + "/" ;

    if (type == 'g') {
        data_file_path += gas_filename;
    } else if (type == 'c') {
        data_file_path += climate_filename;
    } else {
        std::cerr << "[DATA] Unknown type: " << type << std::endl;
        return;
    }

    std::ofstream file(data_file_path, std::ios::app);
    
    if (file.is_open()) {
        file << payload_loc;
        std::cout << "[DATA] Data " << type << " appended\n";
    }
    else{
        std::cout << "[DATA] Data " << type << " NOT appended\n";
    }

}

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
    std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &tm_struct);

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

std::string DataHandler::getGasPayload(){
    
    std::string gas_line;
    std::ifstream gas_file(esp_path + "/" + gas_filename);
    if (!gas_file.is_open()) return "";
    else getline(gas_file, gas_line);

    return getTimeString() + ";" + gas_line;

}

std::string DataHandler::getImageFileName() {

    std::time_t now = std::time(nullptr);
    std::tm tm_struct;

    localtime_r(&now, &tm_struct); 
    
    char date_buffer[25]; // "YYYY-MM-DD_HH-MM-SS\0"
    std::strftime(date_buffer, sizeof(date_buffer), "%Y-%m-%d_%H-%M-%S", &tm_struct);
    
    return std::string("image_") + std::string(date_buffer) + std::string(".jpg");
}

void DataHandler::actualizeDate(std::string new_date){

    day_dir = "Data_" + new_date;
    createNecessaryDirectoriesAndFiles();
    std::cout << "New date detected: " << new_date << "\n";

}

void DataHandler::takePhoto(std::string image_file_path){

    std::string command = "./" + take_photo_script + " '" + image_file_path + "'";
    std::system(command.c_str()); // add checking the result ???

}

void DataHandler::createNecessaryDirectoriesAndFiles() {		// <<------- ADD HERE CREATING FILE "HEADERS" ON SERVER!!!!!

    std::string day_dir_path =      data_dir + "/" + day_dir;
    std::string images_dir_path =   day_dir_path + "/" + images_dir;
    std::string climate_file_path = day_dir_path + "/" + climate_filename;
    std::string gas_file_path =     day_dir_path + "/" + gas_filename;

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
        climate_file << "Timestamp; Humidity; Temperature; Peltier_In; Peltier_Out\n";
        climate_file.close();
        std::cout << "[DATA] Created file: " << climate_file_path << "\n";
    }

    // Create the data file if it doesn't exist
    if (!std::filesystem::exists(gas_file_path)) {
        std::ofstream gas_file(gas_file_path);
        gas_file << "Timestamp; MG811_OKY_CO2_f; MQ3_EtOH_f; MQ4_CH4_f; SEN0565_CH4_f; SEN0568_H2S_f; SEN0569_EtOH_f; SEN0567_NH3_f; SEN0467_H2S_f; SEN0469_NH3_f; SCD41_CO2_f; SCD41_T_f; SCD41_H_f\n";
        gas_file.close();
        std::cout << "[DATA] Created file: " << gas_file_path << "\n";
    }
    
    createMissingFiles();
}

void DataHandler::deleteDayPhotos(std::string& last_date){

    std::string dir_to_delete = data_dir + "/" + "Data_" + last_date + "/" + images_dir;
    std::filesystem::remove_all(dir_to_delete);

}


// ---------- SERVER FUNCTIONS -------------------

void DataHandler::appendDataToServer(std::string& payload, char type) {

    std::string url = sftp_path + day_dir + "/";
    std::string* data_payload = nullptr;

    switch (type) {
        case 'g':
            gas_data_payload.append(payload);
            data_payload = &gas_data_payload;
            url += gas_filename;
            break;
        case 'c':
            climate_data_payload.append(payload);
            data_payload = &climate_data_payload;
            url += climate_filename;
            break;
        default:
            std::cerr << "[SERVER] Unknown type: " << type << std::endl;
            return;
    }

    FILE* mem_file = fmemopen(static_cast<void*>(data_payload->data()), data_payload->length(), "r");
    if (!mem_file) {
        std::cout << "[SERVER] Error opening memory file for payload\n";
        return;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = curl_easy_init();

	if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        std::cout << "[SERVER] url: " << url << std::endl;
        curl_easy_setopt(curl, CURLOPT_PASSWORD, sftp_passwd.c_str());
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_APPEND, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, static_cast<curl_off_t>(data_payload->length()));

        CURLcode res = curl_easy_perform(curl);
        fclose(mem_file);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
        std::cerr << "[SERVER] Błąd transferu pliku " << type << ": " << curl_easy_strerror(res) << std::endl;
        return;
    } 

    }
    else {
        std::cout << "[SERVER] CURL error!\n";
        return;
    }
    
    std::cout << "[SERVER] Saved data " << type << " successfully to " << url << std::endl;
    data_payload->clear();
    


}

void DataHandler::sendPhoto(std::string& image_file_path_to_send){

    photo_payload.push_back(image_file_path_to_send);

    curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_PASSWORD, sftp_passwd.c_str());
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    }
    else {
        std::cerr << "[SERVER] CURL error!\n";
        return;
    }

    while (!photo_payload.empty()) {

        std::string sftp_image_path = sftp_path + photo_payload.front();
		std::string image_file_path = data_dir  + "/" + photo_payload.front();
		
        std::cout << "[SERVER] Sending " << image_file_path << " to: "<< sftp_image_path << std::endl;

        FILE* file = fopen(image_file_path.c_str(), "rb");

        if (!file) {        
            std::cerr << "[SERVER] Błąd: nie można otworzyć pliku!" << '\n';
            return;
        }

        struct stat file_info;         
        fstat(fileno(file), &file_info);
        
        curl_easy_setopt(curl, CURLOPT_URL, sftp_image_path.c_str());
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
        curl_easy_setopt(curl, CURLOPT_READDATA, file);
        CURLcode res = curl_easy_perform(curl);
        fclose(file);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            std::cerr << "[SERVER] Błąd przesyłania: " << curl_easy_strerror(res) << '\n';
            return;
        }
        else {
            std::cout << "[SERVER] Zdjęcie zostało pomyślnie wysłane!" << '\n';
            photo_payload.erase(photo_payload.begin());
        }
    }
}

std::string DataHandler::parseENV(){

    std::ifstream file(passwd_file_path);
    
    if (!file.is_open()) {
        std::cerr << "[SERVER] Błąd: Nie można otworzyć pliku środowiskowego: " << passwd_file_path << std::endl;
        return "";
    }

    std::string prefix = "SFTP_PASSWORD=";
    std::string line;

    while (std::getline(file, line)) {

        size_t pos = line.find(prefix);
        if (pos == 0) { 
            std::string password = line.substr(prefix.length());

            if (!password.empty() && password.back() == '\r') {
                password.pop_back();
            }
            
            return password;
        }
    }

    return "";
}

void DataHandler::createMissingFiles(){
    
    bool climate_file_exists = false;
    bool gas_file_exists = false;

    CURL* curl = curl_easy_init();
    if (!curl) {
        return;
    }

    curl_easy_setopt(curl, CURLOPT_PASSWORD, sftp_passwd.c_str());
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);

    std::string path = sftp_path + day_dir + "/" + climate_filename;
    curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
    climate_file_exists = fileExistsOnServer(curl);

    // curl_easy_reset(); // may be needed
    
    path = sftp_path + day_dir + "/" + gas_filename;
    curl_easy_setopt(curl, CURLOPT_URL, path.c_str());
    gas_file_exists = fileExistsOnServer(curl);
    
    curl_easy_cleanup(curl);

    if (!climate_file_exists) {
        std::string climate_header = "Timestamp; Humidity; Temperature; Peltier_In; Peltier_Out\n";
        appendDataToServer(climate_header, 'c');
    }

    if (!gas_file_exists) {
        std::string gas_header =   "Timestamp; MG811_OKY_CO2_f; MQ3_EtOH_f; MQ4_CH4_f; SEN0565_CH4_f; SEN0568_H2S_f; SEN0569_EtOH_f; SEN0567_NH3_f; SEN0467_H2S_f; SEN0469_NH3_f; SCD41_CO2_f; SCD41_T_f; SCD41_H_f\n";
        appendDataToServer(gas_header, 'g');
    }
}

bool DataHandler::fileExistsOnServer(CURL* curl) {
    while (true) {
        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            return true;
        }

        if (res == CURLE_REMOTE_FILE_NOT_FOUND) {
            return false;
        }

        // Inny błąd (np. brak połączenia, odrzucone hasło lub nieznany klucz hosta)
        std::cerr << "[SERVER] Błąd libcurl: " << curl_easy_strerror(res) << '\n';
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
