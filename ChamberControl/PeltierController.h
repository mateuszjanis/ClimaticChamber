#pragma once
#include <gpiod.hpp>
#include <chrono>
#include <iostream>

#define CHIP_PATH "/dev/gpiochip4"
#define CONSUMER "chamber_rpi5"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------- PINS -----------------------------------//
////////////////////////////////////////////////////////////////////////////////

const int HEAT_PIN_1 = 17;
const int HEAT_PIN_2 = 23;
const int COOL_PIN_1 = 24;
const int COOL_PIN_2 = 27;
const int FAN_PIN = 16;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- VARIABLES --------------------------------//
////////////////////////////////////////////////////////////////////////////////

const int sensors_update_delay = 2; // seconds

extern int curr_mode; // 0 = idle, 1 = heating, -1 = cooling
extern double goal_temperature; // degrees Celsius
const double temp_sensitivity = 2.0; // degrees Celsius
const double temp_diff_toggle_threshold = 4.0; // degrees Celsius
const double temp_diff_fan_threshold = 30.0; // degrees Celsius

double temp_min = 23; // minimum temperature
double temp_max = 27; // maximum temperature
double temp_heating_stop = 24.5; // temperature at which heating stops
double temp_cooling_stop = 25.5; // temperature at which cooling stops

////////////////////////////////////////////////////////////////////////////////
//---------------------------------- OBJECTS ---------------------------------//
////////////////////////////////////////////////////////////////////////////////

extern gpiod::line::offsets COOL_OFFSETS;
extern gpiod::line::offsets HEAT_OFFSETS;
extern gpiod::line::offsets ALL_OFFSETS;
extern gpiod::line::offsets FAN_OFFSET;
extern gpiod::line::offsets INIT_OFFSETS;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////

void setCooling(::gpiod::line_request &request);
void setHeating(::gpiod::line_request &request);
void setIdle(::gpiod::line_request &request);
void fanOn(::gpiod::line_request &request);
void fanOff(::gpiod::line_request &request);
void setMode(::gpiod::line_request &request, unsigned int mode);
void runTemperatureControl(::gpiod::line_request &request);
void calculateTemperatureControlParameters();


