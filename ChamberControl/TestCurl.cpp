#include <iostream>
#include <curl/curl.h>
#include <string>

const std::string sftp_file_path ="sftp://student.agh.edu.pl/CHAMBER/Data/ChamberData.csv"; // /home/imirgrp/matjanis
const std::string user_psswd = "matjanis:Kezi!de5to";

int main(){

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, sftp_file_path );
        curl_easy_setopt(curl, CURLOPT_USERPWD, user_psswd);

        std::cout << "CURL ready!\n";
    }
    else {
        std::cout << "CURL wrong!\n";
    }

    curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    CURLcode res_exist = curl_easy_perform(curl);

    if (res_exist == CURLE_OK){

        std::string header = "time, hum, temp, temp_pelt_in, temp_pelt_out\n";
        FILE* mem_file = fmemopen((void*)header.c_str(), header.length(), "r");
        if (!mem_file) std::cout << "Not a mem file";

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
    else {
        std::cerr << "Błąd połączenia: " << curl_easy_strerror(res_written) << std::endl;
    }

    return 0;
}