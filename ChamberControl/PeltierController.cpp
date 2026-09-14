#include "PeltierController.h"

const int HEAT_PIN_1 = 24;
const int HEAT_PIN_2 = 27;
const int COOL_PIN_1 = 23;
const int COOL_PIN_2 = 17;
const int FAN_PIN = 16;

gpiod::line::offsets COOL_OFFSETS = {COOL_PIN_1, COOL_PIN_2};
gpiod::line::offsets HEAT_OFFSETS = {HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets ALL_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets FAN_OFFSET = {FAN_PIN};
gpiod::line::offsets INIT_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2, FAN_PIN};

void PeltierController::setCooling(){
        
        ::gpiod::line::values heat_line_values = request.get_values(HEAT_OFFSETS);

        if (heat_line_values[0] == gpiod::line::value::INACTIVE && 
            heat_line_values[1] == gpiod::line::value::INACTIVE) {
        
                request.set_values(COOL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        }
        
        request.set_values(COOL_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});
        curr_mode = COOLING;

        std::cout << "------------------ Peltier set: COOLING -----------------\n";
}

void PeltierController::setHeating(){

        ::gpiod::line::values cool_line_values = request.get_values(COOL_OFFSETS);

        if (cool_line_values[0] == gpiod::line::value::INACTIVE && 
            cool_line_values[1] == gpiod::line::value::INACTIVE) {
        
                request.set_values(HEAT_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        }

        request.set_values(HEAT_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});
        curr_mode = HEATING;

        std::cout << "------------------ Peltier set: HEATING --------------------\n";
}

void PeltierController::setIdle(){
        
        request.set_values(ALL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        curr_mode = IDLE;
        
        std::cout << "------------------ Peltier set: IDLE --------------------\n";
}

void PeltierController::fanOn(){
        
        request.set_values(FAN_OFFSET,{gpiod::line::value::INACTIVE});
        
        std::cout << "---------------------- Fan set: ON ----------------------\n";
}

void PeltierController::fanOff(){
        
        request.set_values(FAN_OFFSET,{gpiod::line::value::ACTIVE});
        
        std::cout << "---------------------- Fan set: OFF ---------------------\n";
}

void PeltierController::setMode(enum mode mode){
        
        bool enable_temp_equalization = true;
        
        if(curr_mode == mode){
                std::cerr << "------------------ Peltier already in mode -----------------\n";
                return;
        }
        else{
                std::cout << "------------------- Peltier changing mode -----------------\n";
                
                switch(mode){
                        case IDLE:
                                setIdle();
                                enable_temp_equalization = false;
                                break;
                        case HEATING:
                                if (pelt_temp_in > pelt_temp_out) {
                                        setHeating();
                                        enable_temp_equalization = false;
                                }
                                break;
                        case COOLING:
                                if (pelt_temp_in < pelt_temp_out) {
                                        setCooling();
                                        enable_temp_equalization = false;
                                }       
                                break;
                        default:
                                std::cerr << "------------------ Invalid mode -----------------\n";
                                enable_temp_equalization = true;
                                break;
                }
                
                if (enable_temp_equalization){
                        
                        setIdle();
                        fanOn();

                        double pelt_temp_diff = abs(pelt_temp_in - pelt_temp_out);

                        while(pelt_temp_diff > temp_diff_toggle_threshold){
                                updateSensors();
                                printSensors();
                                pelt_temp_diff = abs(pelt_temp_in - pelt_temp_out);
                                std::this_thread::sleep_for(std::chrono::seconds(sensors_update_interval));
                        }

                        fanOff();
                        
                        switch(mode){
                        case IDLE:
                                setIdle();
                                break;
                        case HEATING:
                                setHeating();
                                break;
                        case COOLING:
                                setCooling();
                                break;
                        default:
                                std::cerr << "------------------ Invalid mode -----------------\n";
                                setIdle();
                                break;
                        }
                }
        }
}


void PeltierController::runTemperatureControl(){

        bool sensors_reading_error = updateSensors();
        printSensors();
        
        double pelt_temp_diff = abs(pelt_temp_in - pelt_temp_out);
        
        if(pelt_temp_diff > temp_diff_fan_threshold || pelt_temp_in >= 57 || pelt_temp_out >= 57) fanOn();
        else fanOff();

        if (!sensors_reading_error){
                switch (curr_mode) {
                        case IDLE: // idle
                                if (temp_mean < temp_min) {
                                        setMode(HEATING); // switch to heating
                                } else if (temp_mean > temp_max) {
                                        setMode(COOLING); // switch to cooling
                                } else {
                                        // setMode(IDLE); // switch to idle
                                        std::cout << "Staying in IDLE mode\n";
                                }
                                break;
                        case HEATING: // heating
                                if (temp_mean >= temp_heating_stop) {
                                        setMode(IDLE); // switch to idle
                                }else{
                                        std::cout << "Staying in HEATING mode\n";
                                }
                                break;
                        case COOLING: // cooling
                                if (temp_mean <= temp_cooling_stop) {
                                        setMode(IDLE); // switch to idle
                                }else{
                                        std::cout << "Staying in COOLING mode\n";
                                }
                                break;
                        default:
                                std::cerr << "------------------ Invalid mode -----------------\n";
                                setIdle();
                                break;
                }
        } else {
                std::cerr << "------------------ Error reading sensors -----------------\n";
                setMode(IDLE);
                fanOff();
        }

        std::this_thread::sleep_for(std::chrono::seconds(sensors_update_interval));

}

void PeltierController::calculateTemperatureControlParameters(){

        temp_min = goal_temperature - temp_sensitivity; // minimum temperature
        temp_max = goal_temperature + temp_sensitivity; // maximum temperature
        temp_heating_stop = goal_temperature - 0.25 * temp_sensitivity; // temperature at which heating stops
        temp_cooling_stop = goal_temperature + 0.25 * temp_sensitivity; // temperature at which cooling stops

        std::cout << "------------------ Temperature Control Parameters -----------------\n";
        std::cout << "Goal Temperature: " << goal_temperature << " °C" << std::endl;
        std::cout << "Temperature Sensitivity: " << temp_sensitivity << " °C" << std::endl;
        std::cout << "Temperature Min: " << temp_min << " °C" << std::endl;
        std::cout << "Temperature Max: " << temp_max << " °C" << std::endl;
        std::cout << "Temperature Heating Stop: " << temp_heating_stop << " °C" << std::endl;
        std::cout << "Temperature Cooling Stop: " << temp_cooling_stop << " °C" << std::endl;
        std::cout << "--------------------------------------------------------------------\n";
}

void PeltierController::setTemperatureGoal(double goal_temp) {
    
    goal_temperature = goal_temp;
    calculateTemperatureControlParameters();

}
