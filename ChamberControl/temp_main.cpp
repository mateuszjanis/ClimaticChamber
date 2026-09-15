#inclde "DataRecording.h"
#include <thread>

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

double temp_mean;
double hum_mean;
float pelt_temp_in;
float pelt_temp_out;

int main(){

    srand( (unsigned)time(NULL) );

    while(true){
        
        temp_mean = rand();
        hum_mean = rand();
        pelt_temp_in = rand();
        pelt_temp_out = rand();

        runDataRecording();
    }


    return 0;
}