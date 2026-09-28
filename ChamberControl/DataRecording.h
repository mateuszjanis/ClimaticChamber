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
#include <sys/stat.h>

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- VARIABLES --------------------------------//
////////////////////////////////////////////////////////////////////////////////

const std::string data_file_path = "../Data/DataRecords.csv";
extern const char* sftp_file_path;
extern const char* user_psswd;

const unsigned int data_record_interval = 60; // seconds
const unsigned int cycles_to_send_image = 30; // half an hour
// extern unsigned int records_to_write;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////

void initializeFile();
bool saveLocally();
void sendImage(CURL *curl);

bool sendToServer(CURL *curl);

void runDataRecording(CURL *curl);
