#pragma once
#include "ControlSensors.h"
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

const std::string data_file_path = "../Data/DataRecords.csv";
extern const char* sftp_file_path;
extern const char* user_psswd;

const unsigned int data_record_interval = 60; // seconds
// extern unsigned int records_to_write;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////

void initializeFile();
bool saveLocally();

bool sendToServer(CURL *curl);

void runDataRecording(CURL *curl);
