#pragma once
#include "ControlSensors.h"
#include <string>
#include <iostream>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- VARIABLES --------------------------------//
////////////////////////////////////////////////////////////////////////////////

const std::string data_file_path = "../../DataRecords/DataRecords.csv";
const unsigned int data_record_interval = 60; // seconds
unsigned int recoeds_to_write;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////


void initializeFile();
bool saveLocally();

bool initializeServerSSH();
bool sendToServer();

void runDataRecording();
