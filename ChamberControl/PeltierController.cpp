#include "PeltierController.h"

////////////////////////////////////////////////////////////////////////////////
//----------------------------------- PINS -----------------------------------//
////////////////////////////////////////////////////////////////////////////////

const int HEAT_PIN_1 = 17;
const int HEAT_PIN_2 = 23;
const int COOL_PIN_1 = 24;
const int COOL_PIN_2 = 27;

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- VARIABLES --------------------------------//
////////////////////////////////////////////////////////////////////////////////

const int TOGGLE_DELAY = 10; // seconds

////////////////////////////////////////////////////////////////////////////////
//---------------------------------- OBJECTS ---------------------------------//
////////////////////////////////////////////////////////////////////////////////

gpiod::line::offsets COOL_OFFSETS = {COOL_PIN_1, COOL_PIN_2};
gpiod::line::offsets HEAT_OFFSETS = {HEAT_PIN_1, HEAT_PIN_2};
gpiod::line::offsets ALL_OFFSETS = {COOL_PIN_1, COOL_PIN_2, HEAT_PIN_1, HEAT_PIN_2};

////////////////////////////////////////////////////////////////////////////////
//--------------------------------- FUNCTIONS --------------------------------//
////////////////////////////////////////////////////////////////////////////////


void setCooling(::gpiod::line_request &request){

        setIdle(request);
        
        std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));

        request.set_values(COOL_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});

        std::cout << "Peltier ustawiony w tryb chłodzenia.\n";
}


void setHeating(::gpiod::line_request &request){
        
        setIdle(request);
        
        std::this_thread::sleep_for(std::chrono::seconds(TOGGLE_DELAY));
        
        request.set_values(HEAT_OFFSETS,{gpiod::line::value::INACTIVE, gpiod::line::value::INACTIVE});

        std::cout << "Peltier ustawiony w tryb grzania.\n";
}

void setIdle(::gpiod::line_request &request){
        
        request.set_values(ALL_OFFSETS,{gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE, gpiod::line::value::ACTIVE});
        
        std::cout << "Peltier ustawiony w stan bezczynny.\n";
}
