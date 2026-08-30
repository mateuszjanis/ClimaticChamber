#include "PeltierController.h"

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

        setIdle(request);
        
        std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));

        request.set_values(COOL_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});

        std::cout << "------------------ Peltier set: COOLING -----------------\n";
}


void setHeating(::gpiod::line_request &request){
        
        setIdle(request);
        
        std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));
        
        request.set_values(HEAT_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});

        std::cout << "------------------ Peltier set: HEATING --------------------\n";
}

void setIdle(::gpiod::line_request &request){
        
        request.set_values(ALL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        
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