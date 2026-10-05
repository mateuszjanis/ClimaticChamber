#include "ServerHandler.h"

/*
bool ServerHandler::sendState(){
    // być może metoda POST do strony internetowej
}

bool ServerHandler::createNewFile(){

}
*/

void ServerHandler::actualizeClimatePayload(std::string payload){

    climate_data_payload.append(payload);
    std::cout << "[SERVER] Climate payload actualized: " << climate_data_payload << std::endl;

}
/*
void ServerHandler::actualizeGasPayload(std::string payload){

    gas_data_payload.append(payload);

}
*/
bool ServerHandler::appendClimateData(std::string payload){

    actualizeClimatePayload(payload);

    // fmemopen: Otwiera string w pamięci RAM jako wirtualny plik tylko do odczytu ("r").
    FILE* mem_file = fmemopen((void*)climate_data_payload.c_str(), climate_data_payload.length(), "r");
    if (!mem_file) return false;

    std::string sftp_climate_path = sftp_path + day_dir + "/" + climate_filename;

    if(curl){

    curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
    curl_easy_setopt(curl, CURLOPT_URL, sftp_climate_path.c_str() );
    std::cout << "[SERVER] sftp_climate_path: " << sftp_climate_path << std::endl;
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_APPEND, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)climate_data_payload.length());

    CURLcode res = curl_easy_perform(curl);
    fclose(mem_file);
        
    if(res != CURLE_OK) {
        std::cerr << "[SERVER]  if(curl) correct. Błąd transferu: " << curl_easy_strerror(res) << std::endl;
        return false;
    }
    else {
		std::cout << "[SERVER]  Saved climate data succesfully to server" << std::endl;
		climate_data_payload.clear();
		return true;
	}

    } 
    else 
    {
        return false;
    }

}
/*
bool ServerHandler::appendGasData(){

    // fmemopen: Otwiera string w pamięci RAM jako wirtualny plik tylko do odczytu ("r").
    FILE* mem_file = fmemopen((void*)gas_data_payload.c_str(), gas_data_payload.length(), "r");
    if (!mem_file) return false;

    if(curl){
       
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_APPEND, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)gas_data_payload.length());


    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "if(curl) correct. Błąd transferu: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    fclose(mem_file);

    std::cout << "Saved succesfully to server" << std::endl;
    gas_data_payload.clear();

    return true;
    
    } 
    else 
    {
        return false;
    }

}
*/

bool ServerHandler::sendPhoto(std::string image_to_send){

    photo_payload.push_back(image_to_send);

    while (!photo_payload.empty()) {

        std::string sftp_image_path = sftp_path + day_dir + "/" + images_dir + "/" + photo_payload.front();
		std::string image_file_path = data_dir + "/" + day_dir + "/" + images_dir + "/" + photo_payload.front();
		
        std::cout << "[SERVER] Sending " << image_file_path << " to: "<< sftp_image_path << std::endl;

        FILE* file = fopen(image_file_path.c_str(), "rb");

        if (!file) {        
            std::cerr << "[SERVER] Błąd: nie można otworzyć pliku!" << '\n';
            return false;
        }

        struct stat file_info;         
        fstat(fileno(file), &file_info);
        
        if (curl) {
            
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
            curl_easy_setopt(curl, CURLOPT_FTP_CREATE_MISSING_DIRS, 1L);
            curl_easy_setopt(curl, CURLOPT_URL, sftp_image_path.c_str());
            // std::cout << "[SERVER] sftp_image_path: " << sftp_image_path << std::endl;
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
            curl_easy_setopt(curl, CURLOPT_APPEND, 0L);
            curl_easy_setopt(curl, CURLOPT_READDATA, file);
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

            CURLcode res = curl_easy_perform(curl);
            fclose(file);

            if (res != CURLE_OK) {
                std::cerr << "[SERVER] Błąd przesyłania: " << curl_easy_strerror(res) << '\n';
                return false;
            }
            else {
                std::cout << "[SERVER] Zdjęcie zostało pomyślnie wysłane!" << '\n';
                photo_payload.erase(photo_payload.begin());
            }
        }
    }

    return true;    

}

    


// std::string ServerHandler::findImageToSend(){
    
//     namespace fs = std::filesystem;

//     if (!fs::exists(image_dir_path) || !fs::is_directory(image_dir_path)) {
//         return "";
//     }

//     std::string latestImagePath = "";
//     // Ustawiamy najmniejszy możliwy czas jako punkt startowy
//     auto latestTime = fs::file_time_type::min(); 

//     for (const auto& entry : fs::directory_iterator(image_dir_path)) {
        
//         auto fileTime = fs::last_write_time(entry);
        
//         if (fileTime > latestTime) {
//             latestTime = fileTime;
//             latestImagePath = entry.path().string();
//         }

//     }
    
//     latestImagePath.erase(0, image_dir_path.length() - 1);
    
//     std::cout << "[SERVER] Found last image path: " << latestImagePath << std::endl;

//     return latestImagePath;
// }

std::string ServerHandler::parseENV(){

    std::ifstream file(passwd_file_path); // może być zgrzyt - wtedy wskaźnik na passwd_file_path: &passwd_file_path
    
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

void ServerHandler::actualizeDayDir(std::string new_day_dir) {
    day_dir = new_day_dir;
}

void ServerHandler::createMissingFiles(){
     
    std::string climate_header = "Timestamp;Humidity;Temperature;Peltier_In;Peltier_Out\n";
	appendClimateData(climate_header);
	
	// the same for gas data

}
