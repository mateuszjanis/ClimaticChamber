#include "DataRecording.h"

// add inotify watch to detect when file was opened, written, created etc.

// unsigned int records_to_write = 0;

void initializeFiles(CURL *curl){ // create files locally and on the server if they don't exist
    
    if (access( data_file_path.c_str(), F_OK ) == -1){
     
        std::ofstream file(data_file_path);

        file << "DATE" << ";"
            << "TIME" << ";"
            << "temp_mean" << ";"
            << "hum_mean" << ";"
            << "pelt_temp_in" << ";"
            << "pelt_temp_out" << "\n";

    }

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    CURLcode res_exist = curl_easy_perform(curl);

    if (res_exist == CURLE_OK){

        std::string header = "time; hum; temp; temp_pelt_in; temp_pelt_out\n";
        FILE* mem_file = fmemopen((void*)header.c_str(), header.length(), "r");
        if (!mem_file) return;

        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_APPEND, 0L);
        curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)header.length());

        CURLcode res_written = curl_easy_perform(curl);
        if(res_written != CURLE_OK) {
            std::cerr << "Błąd tworzenia pliku: " << curl_easy_strerror(res_written) << std::endl;
        }

        fclose(mem_file);

    }
}

std::string getCSVPayload(){

    time_t now = std::time(nullptr);
    struct tm *time_struct = std::localtime(&now);

    // localtime_r(&now, &time_struct);

    char time_buffer[25];
    std::strftime(time_buffer, sizeof(time_buffer), "%Y.%m.%d %H:%M:%S", time_struct);
    
    char payload_buffer[128];

    std::snprintf(payload_buffer, sizeof(payload_buffer), "%s;%.2f;%.2f;%.2f;%.2f\n", 
                time_buffer, 
                temp_mean, 
                hum_mean, 
                pelt_temp_in, 
                pelt_temp_out);

    return std::string(payload_buffer);

}

std::string findImageToSend()
{

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

bool saveLocally(){
    
    std::string payload = getCSVPayload();

    FILE *file = std::fopen(data_file_path.c_str(), "a");

    if (file == nullptr) {
        return false;
    }

    std::fprintf(file, payload.c_str());

    std::fclose(file);
    return true;

}

void sendImage(CURL *curl){

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
            std::cout << "Zdjęcie zostało pomyślnie wysłane!" << '\n';
        }
    }
    
    fclose(file);

}

bool sendToServer(CURL *curl){

    if(curl){

    std::string payload = getCSVPayload();

    // fmemopen: Otwiera string w pamięci RAM jako wirtualny plik tylko do odczytu ("r").
    FILE* mem_file = fmemopen((void*)payload.c_str(), payload.length(), "r");
    if (!mem_file) return false;
    
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)payload.length());
    curl_easy_setopt(curl, CURLOPT_APPEND, 1L);

    CURLcode res = curl_easy_perform(curl);
    if(res != CURLE_OK) {
        std::cerr << "if(curl) correct. Błąd transferu: " << curl_easy_strerror(res) << std::endl;
        return false;
    }

    fclose(mem_file);

    std::cout << "Saved succesfully to server" << std::endl;

    return true;
    
    } 
    else 
    {
        return false;
    }

}

void runDataRecording(CURL *curl){

    initializeFiles(curl);

    unsigned int seconds_to_decrease = 0;
    unsigned int sending_cycles = 0;

    while(true){

    while (!saveLocally() && seconds_to_decrease < data_record_interval) {
        std::cout << "Failed to save locally. Trying again in 10 sec... ";
        std::cout << "Seconds to decrease " << seconds_to_decrease << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));  // if not succeed then try every 10 sec
        seconds_to_decrease += 10;
    }

    if (!sendToServer(curl)) {
        std::cout << "Failed to send server. Trying in next cycle..." << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(data_record_interval - seconds_to_decrease));

    seconds_to_decrease = 0;
    sending_cycles++;

    if (sending_cycles == cycles_to_send_image) sendImage(curl);

    }
}