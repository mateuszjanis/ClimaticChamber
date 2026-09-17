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

const std::string data_file_path = "DataRecords/DataRecords.csv";
const std::string sftp_file_path ="sftp://student.agh.edu.pl/CHAMBER/Data/ChamberData.csv"; // /home/imirgrp/matjanis
const std::string user_psswd = "matjanis:Kezi!de5to";

const unsigned int data_record_interval = 60; // seconds
// extern unsigned int records_to_write;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////

void initializeFile();
bool saveLocally();

bool sendToServer(CURL *curl);

void runDataRecording(CURL *curl);
