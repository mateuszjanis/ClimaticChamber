#include "DataRecording.h"

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

void sendToServer(){
    
}