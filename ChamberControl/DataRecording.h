#pragma once
// #include "ControlSensors.h"
#include <string>
#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <cstdio>
#include <thread>
#include <chrono>
#include <ctime>

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- VARIABLES --------------------------------//
////////////////////////////////////////////////////////////////////////////////

const std::string data_file_path = "DataRecords/DataRecords.csv";
const unsigned int data_record_interval = 60; // seconds
extern unsigned int records_to_write;

extern double temp_mean;
extern double hum_mean;
extern float pelt_temp_in;
extern float pelt_temp_out;

const int sensors_update_interval = 10;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////


void initializeFile();
bool saveLocally();

bool sendToServer(CURL *curl);

void runDataRecording(CURL *curl);
