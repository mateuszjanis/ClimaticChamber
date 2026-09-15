#include "DataRecording.h"

unsigned int records_to_write = 0;

void initializeFile(){
    
    std::ofstream file(data_file_path);

    // Zapisanie danych oddzielonych przecinkami i znakiem nowej linii
    file << "DATE" << ","
         << "TIME" << ","
         << "temp_mean" << ","
         << "hum_mean" << ","
         << "pelt_temp_in" << ","
         << "pelt_temp_out" << "\n";

}

bool saveLocally(){

    // Otwarcie pliku w trybie dopisywania (append)
    std::ofstream file(data_file_path, std::ios::app);

    // Sprawdzenie, czy plik został poprawnie otwarty
    if (!file.is_open()) {
        return false;
    }

    // Zapisanie danych oddzielonych przecinkami i znakiem nowej linii
    file << __DATE__ << ","
         << __TIME__ << ","
         << temp_mean << ","
         << hum_mean << ","
         << pelt_temp_in << ","
         << pelt_temp_out << "\n";

    // Zwraca true, jeśli nie wystąpiły żadne błędy zapisu (np. brak miejsca na dysku)
    return file.good();

}

bool initializeServerSSH(){

}

bool sendToServer(){
    
}

void runDataRecording(){

    unsigned int seconds_to_decrease = 0;

    while (!saveLocally() && seconds_to_decrease < sensors_update_interval) {
        std::cout << "Failed to save locally. Trying again...": << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(10));
        seconds_to_decrease += 10;
    }

    if (!sendToServer()) {
        records_to_write++;
        std::cout << "Failed to save on server. Trying in next cycle...": << std::endl;
    } else {
        records_to_write = 0;
    }

    std::this_thread::sleep_for(std::chrono::seconds(sensors_update_interval - seconds_to_decrease));

}