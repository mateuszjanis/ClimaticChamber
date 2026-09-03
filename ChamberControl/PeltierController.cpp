#include "PeltierController.h"


int curr_mode = 0; // 0 = idle, 1 = heating, -1 = cooling
double goal_temperature = 25.0; // default goal temperature


////////////////////////////////////////////////////////////////////////////////
//---------------------------------- OBJECTS ---------------------------------//
////////////////////////////////////////////////////////////////////////////////

gpiod::line::offsets COOL_OFFSETS = {COOL_PIN_1, COOL_PIN_2};
gpiod::line::offsets HEAT_OFFSETS = {HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets ALL_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets FAN_OFFSET = {FAN_PIN};
gpiod::line::offsets INIT_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2, FAN_PIN};

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////


void setCooling(::gpiod::line_request &request){
        
        ::gpiod::line::values heat_line_values = request.get_values(HEAT_OFFSETS);

        if (heat_line_values[0] == gpiod::line::value::INACTIVE && 
            heat_line_values[1] == gpiod::line::value::INACTIVE) {
        
                request.set_values(COOL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        }
        
        request.set_values(COOL_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});
        curr_mode = -1;

        std::cout << "------------------ Peltier set: COOLING -----------------\n";
}

void setHeating(::gpiod::line_request &request){

        ::gpiod::line::values cool_line_values = request.get_values(COOL_OFFSETS);

        if (cool_line_values[0] == gpiod::line::value::INACTIVE && 
            cool_line_values[1] == gpiod::line::value::INACTIVE) {
        
                request.set_values(HEAT_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        }

        request.set_values(HEAT_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});
        curr_mode = 1;

        std::cout << "------------------ Peltier set: HEATING --------------------\n";
}

void setIdle(::gpiod::line_request &request){
        
        request.set_values(ALL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        curr_mode = 0;
        
        std::cout << "------------------ Peltier set: IDLE --------------------\n";
}

void fanOn(::gpiod::line_request &request){
        
        request.set_values(FAN_OFFSET,{gpiod::line::value::INACTIVE});
        
        std::cout << "---------------------- Fan set: ON ----------------------\n";
}

void fanOff(::gpiod::line_request &request){
        
        request.set_values(FAN_OFFSET,{gpiod::line::value::ACTIVE});
        
        std::cout << "---------------------- Fan set: OFF ---------------------\n";
}

void setMode(::gpiod::line_request &request, int mode){
        
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
                case 0:
                        setIdle(request);
                        break;
                case 1:
                        setHeating(request);
                        break;
                case -1:
                        setCooling(request);
                        break;
                default:
                        std::cerr << "------------------ Invalid mode -----------------\n";
                        setIdle(request);
                        break;
                }

        }
}

void runTemperatureControl(::gpiod::line_request &request){

        updateSensors();

        switch (curr_mode) {
                case 0: // idle
                        if (temp_mean < temp_min) {
                                setMode(request, 1); // switch to heating
                        } else if (temp_mean > temp_max) {
                                setMode(request, -1); // switch to cooling
                        } else {
                                setMode(request, 0); // switch to idle
                        }
                        break;
                case 1: // heating
                        if (temp_mean >= temp_heating_stop) {
                                setMode(request, 0); // switch to idle
                        }
                        break;
                case -1: // cooling
                        if (temp_mean <= temp_cooling_stop) {
                                setMode(request, 0); // switch to idle
                        }
                        break;
                default:
                        std::cerr << "------------------ Invalid mode -----------------\n";
                        setIdle(request);
                        break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(sensors_update_delay));

}

void calculateTemperatureControlParameters(){
        
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