#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- VARIABLES --------------------------------//
////////////////////////////////////////////////////////////////////////////////

extern double temp_up;
extern double hum_up;
extern double temp_down;
extern double hum_down;
extern double temp_mean;
extern double hum_mean;
extern double pelt_temp_in;
extern double pelt_temp_out;

const int sensors_update_interval = 10; // seconds
const double acceptable_sens_diff = 10.0;

const std::string temp_down_path = "/sys/bus/iio/devices/iio:device0/in_temp_input";
const std::string hum_down_path  = "/sys/bus/iio/devices/iio:device0/in_humidityrelative_input";
const std::string temp_up_path = "/sys/bus/iio/devices/iio:device1/in_temp_input";
const std::string hum_up_path  = "/sys/bus/iio/devices/iio:device1/in_humidityrelative_input";
// const std::string pelt_temp_in_path = "/sys/bus/w1/devices/w1_bus_master1/28-000898431e08/temperature";
// const std::string pelt_temp_out_path = "/sys/bus/w1/devices/w1_bus_master1/28-0516a4a33eff/temperature";

const std::string compile_filepath = "ChamberControl";

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////

double readSensor(const std::string& filepath);
std::string readLastLine(const char* filepath);

void initialSensorsReading();
void initialPeltierSensorsReading();

bool readPeltierSensors();
bool updateSensors();
void printSensors();

