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

}
/*
void ServerHandler::actualizeGasPayload(std::string payload){

    gas_data_payload.append(payload);

}
*/
bool ServerHandler::appendClimateData(){

    if(curl){

    // fmemopen: Otwiera string w pamięci RAM jako wirtualny plik tylko do odczytu ("r").
    FILE* mem_file = fmemopen((void*)climate_data_payload.c_str(), climate_data_payload.length(), "r");
    if (!mem_file) return false;
    
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)climate_data_payload.length());
    curl_easy_setopt(curl, CURLOPT_APPEND, 1L);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "if(curl) correct. Błąd transferu: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    fclose(mem_file);

    std::cout << "Saved succesfully to server" << std::endl;
    climate_data_payload.clear();

    return true;
    
    } 
    else 
    {
        return false;
    }

}
/*
bool ServerHandler::appendGasData(){

    if(curl){

    // fmemopen: Otwiera string w pamięci RAM jako wirtualny plik tylko do odczytu ("r").
    FILE* mem_file = fmemopen((void*)gas_data_payload.c_str(), gas_data_payload.length(), "r");
    if (!mem_file) return false;
    
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)gas_data_payload.length());
    curl_easy_setopt(curl, CURLOPT_APPEND, 1L);

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

bool ServerHandler::sendImage(){

    std::string image_file_path = findImageToSend();

    FILE* file = fopen(image_dir_path + image_file_path, "rb");

    if (!file) {        
        std::cerr << "Błąd: nie można otworzyć pliku!" << '\n';
        return;
    }

    struct stat file_info;         
    fstat(fileno(file), &file_info);
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READDATA, file);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "Błąd przesyłania: " << curl_easy_strerror(res) << '\n';
        }
        else {
             std::cout << "Zdjęcie zostało pomyślnie wysłane!" << '\n';
        }
    }
    
    fclose(file);

}

std::string ServerHandler::findImageToSend(){
    
    namespace fs = std::filesystem;

    if (!fs::exists(image_dir_path) || !fs::is_directory(image_dir_path)) {
        return "";
    }

    std::string latestImagePath = "";
    // Ustawiamy najmniejszy możliwy czas jako punkt startowy
    auto latestTime = fs::file_time_type::min(); 

    for (const auto& entry : fs::directory_iterator(image_dir_path)) {
        
        auto fileTime = fs::last_write_time(entry);
        
        if (fileTime > latestTime) {
            latestTime = fileTime;
            latestImagePath = entry.path().string();
        }

    }

    return latestImagePath;
}

const char* ServerHandler::parseENV(){

    const std::string& filePath = "../.env";
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Błąd: Nie można otworzyć pliku środowiskowego: " << filePath << std::endl;
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