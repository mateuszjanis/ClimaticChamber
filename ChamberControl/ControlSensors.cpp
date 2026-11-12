#include "ControlSensors.h"

double temp_up = -127.0;
double hum_up = -127.0;
double temp_down = -127.0;
double hum_down = -127.0;
double temp_mean = -127.0;
double hum_mean = -127.0;
double pelt_temp_in = -127.0;
double pelt_temp_out = -127.0;

double readSensor(const std::string& filepath) {

    std::ifstream file(filepath);
    if (!file.is_open()) {
        return -1.0;
    }
    double val;
    file >> val;
    return val / 1000.0;

}

void initialPeltierSensorsReading(){
    
    bool peltier_sensors_reading_error = true;

    double temp1;
    double temp2;

    while (peltier_sensors_reading_error) {
        
        temp1 = readSensor(pelt_temp_in_path);
        temp2 = readSensor(pelt_temp_out_path);

        std::cout << "---------------- Current Sensor Readings ----------------" << std::endl;
        std::cout << "PeltIn: " << temp1 << " °C ";
        std::cout << "PeltOut: " << temp2 << " °C " << std::endl;
        std::cout << "---------------------------------------------------------" << std::endl;
        std::cout << "Checking if peltier sensors read correctly: ";
                
        if (temp1 > 0 && temp2 > 0) {
            peltier_sensors_reading_error = false;
            std::cout << "YES\n";
        } 
        else {
            std::cout << "NO. Trying again\n";
        }

    }
    
    std::this_thread::sleep_for(std::chrono::seconds(sensors_update_interval));

    pelt_temp_in = temp1;
    pelt_temp_out = temp2;

}

void initialSensorsReading(){
    
    bool sensors_reading_error = true;
    
    double temp_up_temp;
    double hum_up_temp;
    double temp_down_temp;
    double hum_down_temp;
    
    while(sensors_reading_error){

        temp_up_temp = readSensor(temp_up_path);
        hum_up_temp = readSensor(hum_up_path);
        temp_down_temp = readSensor(temp_down_path);
        hum_down_temp = readSensor(hum_down_path);
        
        std::cout << "---------------- Current Sensor Readings ----------------" << std::endl;
        std::cout << "TempDown: " << temp_down_temp << " °C ";
        std::cout << "HumDown:  " << hum_down_temp << " % " << std::endl;
        std::cout << "TempUp:   " << temp_up_temp << " °C ";
        std::cout << "HumUp:    " << hum_up_temp << " % " << std::endl;
        std::cout << "---------------------------------------------------------" << std::endl;

        std::cout << "Checking if sensors read correctly: ";

        if(temp_up_temp > 10 && temp_up_temp < 30 && hum_up_temp > 0 && hum_up_temp < 100 && 
           temp_down_temp > 10 && temp_down_temp < 30 && hum_down_temp > 0 && hum_down_temp < 100) {
                sensors_reading_error = false;
                std::cout << "YES\n";
                break;
        }
        else {
            std::cout << "NO. Trying again\n";
            sensors_reading_error = true;
        }

        std::this_thread::sleep_for(std::chrono::seconds(sensors_update_interval));

    }

    temp_up = temp_up_temp;
    hum_up = hum_up_temp;
    temp_down = temp_down_temp;
    hum_down = hum_down_temp;
    temp_mean = (temp_up + temp_down) / 2.0;
    hum_mean = (hum_up + hum_down) / 2.0;

}

bool updateSensors() {

    bool sensors_reading_error = false;

    double temp_up_temp         = readSensor(temp_up_path);
    double hum_up_temp          = readSensor(hum_up_path);
    double temp_down_temp       = readSensor(temp_down_path);
    double hum_down_temp        = readSensor(hum_down_path);
    double pelt_temp_in_temp    = readSensor(pelt_temp_in_path);
    double pelt_temp_out_temp   = readSensor(pelt_temp_out_path);

    if (std::abs(temp_up_temp - temp_up) > acceptable_sens_diff || 
        std::abs(hum_up_temp - hum_up) > acceptable_sens_diff) 
    {
        sensors_reading_error =  true; // error
        std::cout << "Wrong DHT up reading\n";
        // std::cout << "temp_up_temp: " << temp_up_temp << " vs temp_up: " << temp_up << std::endl; // debug
        // std::cout << "hum_up_temp: " << hum_up_temp << " vs hum_up: " << hum_up << std::endl;
    } else {
        // successful reading
        temp_up = temp_up_temp;
        hum_up = hum_up_temp;
        std::cout << "Correct DHT up reading\n";
    }

    if (std::abs(temp_down_temp - temp_down) > acceptable_sens_diff || 
        std::abs(hum_down_temp - hum_down) > acceptable_sens_diff) 
    {    
        sensors_reading_error =  true; // error
        std::cout << "Wrong DHT down reading\n";
        // std::cout << "temp_down_temp: " << temp_down_temp << " vs temp_down: " << temp_down << std::endl; // debug
        // std::cout << "hum_down_temp: " << hum_down_temp << " vs hum_down: " << hum_down << std::endl;
    } else {
        // successful reading
        temp_down = temp_down_temp;
        hum_down = hum_down_temp;
        std::cout << "Correct DHT down reading\n";
    }

    if (!sensors_reading_error) {
        temp_mean = (temp_up + temp_down) / 2.0;
        hum_mean = (hum_up + hum_down) / 2.0;
    }

    if (std::abs(pelt_temp_in_temp - pelt_temp_in) > acceptable_sens_diff || 
        std::abs(pelt_temp_out_temp - pelt_temp_out) > acceptable_sens_diff ) 
    {

        std::cout << "Error reading Peltier sensors: " << pelt_temp_in_temp << ", " 
                  << pelt_temp_out_temp << std::endl;
        sensors_reading_error = true; // error
    
    } else {
    
        pelt_temp_in = pelt_temp_in_temp;
        pelt_temp_out = pelt_temp_out_temp;
        std::cout << "Peltier sensors read correctly: " << std::endl;
    
    }

    return sensors_reading_error; // Return the error status

}

void printSensors() {

    std::cout << "-------------------- Sensor Readings --------------------" << std::endl;
    std::cout << "TempDown: " << temp_down << " °C ";
    std::cout << "HumDown:  " << hum_down << " % " << std::endl;
    std::cout << "TempUp:   " << temp_up << " °C ";
    std::cout << "HumUp:    " << hum_up << " % " << std::endl;
    std::cout << "TempMean: " << temp_mean << " °C ";
    std::cout << "HumMean:  " << hum_mean << " % " << std::endl;
    std::cout << "PeltTempIn: " << pelt_temp_in << " °C ";
    std::cout << "PeltTempOut: " << pelt_temp_out << " °C " << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
}
