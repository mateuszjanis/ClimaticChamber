#pragma once
#include <gpiod.hpp>
#include <chrono>
#include <iostream>
#include "ControlSensors.h"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------- PINS -----------------------------------//
////////////////////////////////////////////////////////////////////////////////

extern const int HEAT_PIN_1;
extern const int HEAT_PIN_2;
extern const int COOL_PIN_1;
extern const int COOL_PIN_2;
extern const int FAN_PIN;

extern gpiod::line::offsets COOL_OFFSETS;
extern gpiod::line::offsets HEAT_OFFSETS;
extern gpiod::line::offsets ALL_OFFSETS;
extern gpiod::line::offsets FAN_OFFSET;
extern gpiod::line::offsets INIT_OFFSETS;

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

    PeltierController(::gpiod::line_request &request, nlohmann::json &config_json) : request(request)
    {

        goal_temperature = config_json["control"]["goal_temperature"];
        temp_sensitivity = config_json["settings"]["temp_sensitivity"];
        temp_diff_toggle_threshold = config_json["settings"]["temp_diff_toggle_threshold"];
        temp_diff_fan_threshold = config_json["settings"]["temp_diff_fan_threshold"];

        initialSensorsReading();
        initialPeltierSensorsReading();
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






