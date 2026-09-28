#include "DataRecording.h"

// add inotify watch to detect when file was opened, written, created etc.

// unsigned int records_to_write = 0;

void initializeFiles(CURL *curl){ // create files locally and on the server if they dont exist
    
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

bool saveLocally(){

    time_t now = std::time(nullptr);
    struct tm *time_struct = std::localtime(&now);

    // Otwarcie pliku w trybie dopisywania (append)
    std::ofstream file(data_file_path, std::ios::app);

    // Sprawdzenie, czy plik został poprawnie otwarty
    if (!file.is_open()) {
        return false;
    }

    // Zapisanie danych oddzielonych przecinkami i znakiem nowej linii
    file << time_struct->tm_year + 1900 << "."
         << time_struct->tm_mon + 1 << "."
         << time_struct->tm_mday << " "
         << time_struct->tm_hour << ":"
         << time_struct->tm_min << ":"
         << time_struct->tm_sec << ";"
         << temp_mean << ";"
         << hum_mean << ";"
         << pelt_temp_in << ";"
         << pelt_temp_out << "\n";



    // Zwraca true, jeśli nie wystąpiły żadne błędy zapisu (np. brak miejsca na dysku)
    if(file.good()){

        std::cout << "Saved succesfully" << std::endl;
        return true;

    } else {

        return false;

    }

}

void sendImage(CURL *curl){

    std::string image_file_path = findImageToSend(); // funkcja znajdywania ostatniego zdjęcia

    FILE* file = fopen(image_dir_path + image_file_path, "rb");      // Otwieramy lokalny plik ze zdjęciem w trybie odczytu binarnego ("rb")
    
    if (!file) {                                                     // Sprawdzamy, czy wystąpił problem z otwarciem pliku (np. nie istnieje)
        std::cerr << "Błąd: nie można otworzyć pliku!" << '\n';      // Wypisujemy komunikat o błędzie na standardowe wyjście błędów
        return;                                                      // Przerywamy działanie programu i zwracamy kod błędu 1
    }                                                                // Zamykamy blok instrukcji warunkowej sprawdzającej plik

    struct stat file_info;                                           // Tworzymy strukturę, która przechowa szczegółowe dane o naszym pliku
    fstat(fileno(file), &file_info);                                 // Pobieramy informacje o pliku na podstawie jego deskryptora (w tym rozmiar)

    if (curl) {                                                      // Sprawdzamy, czy inicjalizacja sesji CURL zakończyła się sukcesem
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);                  // Informujemy bibliotekę CURL, że naszym celem jest wysłanie pliku (upload)
        curl_easy_setopt(curl, CURLOPT_READDATA, file);              // Wskazujemy wskaźnik na nasz otwarty plik, z którego CURL ma czytać dane
        
        // Zabezpieczenie rozmiaru pliku (wymagane przez libcurl dla płynnego uploadu)
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size); // Ustawiamy dokładny rozmiar pliku do wysłania
        // curl_easy_setopt(curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);           // Konfigurujemy typ autoryzacji SSH/SFTP na logowanie hasłem

        CURLcode res = curl_easy_perform(curl);                      // Uruchamiamy właściwy transfer pliku i zapisujemy kod wyniku do zmiennej "res"

        if (res != CURLE_OK) {                                       // Sprawdzamy, czy zwrócony kod wyniku oznacza jakikolwiek błąd transferu
            std::cerr << "Błąd przesyłania: " << curl_easy_strerror(res) << '\n'; // Wyświetlamy tekstowy opis błędu wygenerowany przez CURL
        } else {                                                     // W przeciwnym wypadku (jeśli kod wyniku to CURLE_OK, czyli sukces)
            std::cout << "Zdjęcie zostało pomyślnie wysłane!" << '\n'; // Wyświetlamy radosny komunikat informujący o udanym przesłaniu pliku
        }                                                            // Zamykamy blok instrukcji warunkowej obsługującej wynik
        
        curl_easy_cleanup(curl);                                     // Sprzątamy i zwalniamy pamięć/zasoby przypisane do naszej sesji CURL
    }                                                                // Zamykamy blok instrukcji warunkowej dla inicjalizacji uchwytu CURL

    fclose(file);                                                    // Zamykamy plik lokalny, żeby zwolnić zasoby systemowe Raspberry Pi
    curl_global_cleanup();
}

bool sendToServer(CURL *curl){

    if(curl){
    // do dopisania wczytywanie niezapisanych danych
    
    time_t now = std::time(nullptr);
    struct tm *time_struct = std::localtime(&now);

    std::string data_line = 
    std::to_string(time_struct->tm_year + 1900) + "." +
    std::to_string(time_struct->tm_mon + 1) + "." +
    std::to_string(time_struct->tm_mday) + " " +
    std::to_string(time_struct->tm_hour) + ":" +
    std::to_string(time_struct->tm_min) + ":" +
    std::to_string(time_struct->tm_sec) + ";" +
    std::to_string(hum_mean) + ";" + 
    std::to_string(temp_mean) + ";" + 
    std::to_string(pelt_temp_in) + ";" + 
    std::to_string(pelt_temp_out) +"\n";

    // fmemopen: Otwiera string w pamięci RAM jako wirtualny plik tylko do odczytu ("r").
    FILE* mem_file = fmemopen((void*)data_line.c_str(), data_line.length(), "r");
    if (!mem_file) return false;
    
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, mem_file);
    curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)data_line.length());
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
        // records_to_write++;
        std::cout << "Failed to send server. Trying in next cycle..." << std::endl;
    } else {
        // records_to_write = 0;
    }

    std::this_thread::sleep_for(std::chrono::seconds(data_record_interval - seconds_to_decrease));

    seconds_to_decrease = 0;
    sending_cycles++;

    if (sending_cycles == cycles_to_send_image) sendImage(curl);

    }
}