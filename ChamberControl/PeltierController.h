#pragma once
#include <gpiod.hpp>
#include <chrono>
#include <iostream>
#include "ControlSensors.h"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------- PINS -----------------------------------//
////////////////////////////////////////////////////////////////////////////////

const int HEAT_PIN_1 = 17;
const int HEAT_PIN_2 = 23;
const int COOL_PIN_1 = 24;
const int COOL_PIN_2 = 27;
const int FAN_PIN = 16;

gpiod::line::offsets COOL_OFFSETS = {COOL_PIN_1, COOL_PIN_2};
gpiod::line::offsets HEAT_OFFSETS = {HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets ALL_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets FAN_OFFSET = {FAN_PIN};
gpiod::line::offsets INIT_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2, FAN_PIN};

class PeltierController {

    ::gpiod::line_request &request;

    enum mode { 
        IDLE = 0,
        HEATING = 1,
        COOLING = -1
    };

    enum mode curr_mode;

    double goal_temperature; // degrees Celsius
    const double temp_sensitivity; // degrees Celsius
    const double temp_diff_toggle_threshold; // degrees Celsius
    const double temp_diff_fan_threshold; // degrees Celsius

    double temp_min; // minimum temperature
    double temp_max; // maximum temperature
    double temp_heating_stop; // temperature at which heating stops
    double temp_cooling_stop; // temperature at which cooling stops

public:

    PeltierController(::gpiod::line_request &request, double goal_temp) : request(request), 
        temp_sensitivity(2.0), temp_diff_toggle_threshold(4.0), temp_diff_fan_threshold(30.0) 
    {
        calculateTemperatureControlParameters();
        setTemperatureGoal(goal_temp);
        
        curr_mode = IDLE;
        setMode(IDLE);
        fanOff();
    }
    
    void runTemperatureControl();
    void setTemperatureGoal(double goal_temp);

private:

    void setCooling();
    void setHeating();
    void setIdle();
    void fanOn();
    void fanOff();
    void calculateTemperatureControlParameters();
    void setMode(enum mode mode);

};






