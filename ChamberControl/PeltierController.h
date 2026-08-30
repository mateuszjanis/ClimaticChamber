#pragma once
#include <gpiod.hpp>
#include <chrono>
#include <thread>
#include <iostream>
#include <optional>

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

const int TOGGLE_DELAY = 10; // seconds

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


