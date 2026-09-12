#include "PeltierController.h"

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
        
        if(curr_mode == mode){
                std::cerr << "------------------ Peltier already in mode -----------------\n";
                return;
        }
        else{
                std::cout << "------------------- Peltier changing mode -----------------\n";
                
                setIdle();
                fanOn();

                double pelt_temp_diff = abs(pelt_temp_in - pelt_temp_out);

                while(pelt_temp_diff > temp_diff_toggle_threshold){
                        updateSensors();
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

void PeltierController::runTemperatureControl(){

        updateSensors();

        switch (curr_mode) {
                case IDLE: // idle
                        if (temp_mean < temp_min) {
                                setMode(HEATING); // switch to heating
                        } else if (temp_mean > temp_max) {
                                setMode(COOLING); // switch to cooling
                        } else {
                                setMode(IDLE); // switch to idle
                        }
                        break;
                case HEATING: // heating
                        if (temp_mean >= temp_heating_stop) {
                                setMode(IDLE); // switch to idle
                        }
                        break;
                case COOLING: // cooling
                        if (temp_mean <= temp_cooling_stop) {
                                setMode(IDLE); // switch to idle
                        }
                        break;
                default:
                        std::cerr << "------------------ Invalid mode -----------------\n";
                        setIdle();
                        break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(sensors_update_interval));

}

void PeltierController::calculateTemperatureControlParameters(){
        
        updateSensors();

        double temp_min = goal_temperature - temp_sensitivity; // minimum temperature
        double temp_max = goal_temperature + temp_sensitivity; // maximum temperature
        double temp_heating_stop = goal_temperature - 0.25 * temp_sensitivity; // temperature at which heating stops
        double temp_cooling_stop = goal_temperature + 0.25 * temp_sensitivity; // temperature at which cooling stops

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