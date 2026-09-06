#include "PeltierController.h"

void PeltierController::setCooling(){
        
        ::gpiod::line::values heat_line_values = request.get_values(HEAT_OFFSETS);

        if (heat_line_values[0] == gpiod::line::value::INACTIVE && 
            heat_line_values[1] == gpiod::line::value::INACTIVE) {
        
                request.set_values(COOL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        }
        
        request.set_values(COOL_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});
        curr_mode = -1;

        std::cout << "------------------ Peltier set: COOLING -----------------\n";
}

void PeltierController::setHeating(){

        ::gpiod::line::values cool_line_values = request.get_values(COOL_OFFSETS);

        if (cool_line_values[0] == gpiod::line::value::INACTIVE && 
            cool_line_values[1] == gpiod::line::value::INACTIVE) {
        
                request.set_values(HEAT_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        }

        request.set_values(HEAT_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});
        curr_mode = 1;

        std::cout << "------------------ Peltier set: HEATING --------------------\n";
}

void PeltierController::setIdle(){
        
        request.set_values(ALL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        curr_mode = 0;
        
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

void PeltierController::setMode(enum curr_mode mode){
        
        if(curr_mode == mode){
                std::cerr << "------------------ Peltier already in mode -----------------\n";
                return;
        }
        else{
                std::cout << "------------------- Peltier changing mode -----------------\n";
                
                setIdle(request);
                fanOn(request);

                double pelt_temp_diff = abs(pelt_temp_down - pelt_temp_up);

                while(pelt_temp_diff > temp_diff_toggle_threshold){
                        updateSensors();
                        pelt_temp_diff = abs(pelt_temp_down - pelt_temp_up);
                        std::this_thread::sleep_for(std::chrono::seconds(sensors_update_delay));
                }

                fanOff(request);
                
                switch(mode){
                case IDLE:
                        setIdle(request);
                        break;
                case HEATING:
                        setHeating(request);
                        break;
                case COOLING:
                        setCooling(request);
                        break;
                default:
                        std::cerr << "------------------ Invalid mode -----------------\n";
                        setIdle(request);
                        break;
                }

        }
}

void PeltierController::runTemperatureControl(){

        updateSensors();

        switch (curr_mode) {
                case IDLE: // idle
                        if (temp_mean < temp_min) {
                                setMode(request, HEATING); // switch to heating
                        } else if (temp_mean > temp_max) {
                                setMode(request, COOLING); // switch to cooling
                        } else {
                                setMode(request, IDLE); // switch to idle
                        }
                        break;
                case HEATING: // heating
                        if (temp_mean >= temp_heating_stop) {
                                setMode(request, IDLE); // switch to idle
                        }
                        break;
                case COOLING: // cooling
                        if (temp_mean <= temp_cooling_stop) {
                                setMode(request, IDLE); // switch to idle
                        }
                        break;
                default:
                        std::cerr << "------------------ Invalid mode -----------------\n";
                        setIdle(request);
                        break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(sensors_update_delay));

}

void PeltierController::calculateTemperatureControlParameters(){
        
        updateSensors();

        double temp_min = goal_temperature - temp_sensitivity; // minimum temperature
        double temp_max = goal_temperature + temp_sensitivity; // maximum temperature
        double temp_heating_stop = goal_temperature - 0.25 * temp_sensitivity; // temperature at which heating stops
        double temp_cooling_stop = goal_temperature + 0.25 * temp_sensitivity; // temperature at which cooling stops

        cout << "------------------ Temperature Control Parameters -----------------\n";
        cout << "Goal Temperature: " << goal_temperature << " °C" << endl;
        cout << "Temperature Sensitivity: " << temp_sensitivity << " °C" << endl;
        cout << "Temperature Min: " << temp_min << " °C" << endl;
        cout << "Temperature Max: " << temp_max << " °C" << endl;
        cout << "Temperature Heating Stop: " << temp_heating_stop << " °C" << endl;
        cout << "Temperature Cooling Stop: " << temp_cooling_stop << " °C" << endl;
        cout << "--------------------------------------------------------------------\n";
}

void PeltierController::setTemperatureGoal(double goal_temp) {
    
    goal_temperature = goal_temp;
    calculateTemperatureControlParameters();

}