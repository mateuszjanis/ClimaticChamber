#include "DataRecording.h"
#include <curl/curl.h>
#include <cstdio>

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

void sendToServer(const char* localpath, const char* remoteurl,
                 const char* user, const char* privkey){

    FILE* f = fopen(localpath, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    curl_off_t size = (curl_off_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    CURL* curl = curl_easy_init();
    CURLcode res = CURLE_OK;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, remoteurl); /* e.g. "sftp://host:22/remote/dir/file" */
        curl_easy_setopt(curl, CURLOPT_USERNAME, user);
        curl_easy_setopt(curl, CURLOPT_SSH_PRIVATE_KEYFILE, privkey);
        curl_easy_setopt(curl, CURLOPT_READDATA, f);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, size);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    fclose(f);
    return (res == CURLE_OK);

}