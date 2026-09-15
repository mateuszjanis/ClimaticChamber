#include "DataRecording.h"
#include <thread>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int main(){

    srand( (unsigned)time(NULL) );

    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL *curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "sftp://student.agh.edu.pl/home/imirgrp/matjanis/CHAMBER/Data/ChamberData.csv");
        curl_easy_setopt(curl, CURLOPT_USERPWD, "matjanis:Kezi!de5to");
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    }


    while(true){
        
        temp_mean = rand()%100;
        hum_mean = rand()%100;
        pelt_temp_in = rand()%100;
        pelt_temp_out = rand()%100;

        runDataRecording(curl);
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}