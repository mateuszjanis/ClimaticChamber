#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- VARIABLES --------------------------------//
////////////////////////////////////////////////////////////////////////////////

extern double temp_up;
extern double hum_up;
extern double temp_down;
extern double hum_down;
extern double temp_mean;
extern double hum_mean;
extern double pelt_temp_down;
extern double pelt_temp_up;

const std::string temp_up_path = "/sys/bus/iio/devices/iio:device0/in_temp_input";
const std::string hum_up_path  = "/sys/bus/iio/devices/iio:device0/in_humidityrelative_input";
const std::string temp_down_path = "/sys/bus/iio/devices/iio:device1/in_temp_input";
const std::string hum_down_path  = "/sys/bus/iio/devices/iio:device1/in_humidityrelative_input";
const std::string pelt_temp_down_path = "/sys/bus/w1/devices/w1_bus_master1/28-000898431e08/temperature";
const std::string pelt_temp_up_path = "/sys/bus/w1/devices/w1_bus_master1/28-0516a4a33eff/temperature";

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////

double readSensor(const std::string& filepath);
void updateSensors();
void printSensors();