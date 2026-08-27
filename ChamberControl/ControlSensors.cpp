#include "ControlSensors.h"

double temp_up;
double hum_up;
double temp_down;
double hum_down;
double pelt_temp_down;
double pelt_temp_up;

double readSensor(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return -1.0;
    }
    double val;
    file >> val;
    return val / 1000.0;
}

void updateSensors() {
    temp_up = readSensor(temp_up_path);
    hum_up = readSensor(hum_up_path);
    temp_down = readSensor(temp_down_path);
    hum_down = readSensor(hum_down_path);
    pelt_temp_down = readSensor(pelt_temp_down_path);
    pelt_temp_up = readSensor(pelt_temp_up_path);
}

void printSensors() {
    std::cout << "-------------------- Sensor Readings --------------------" << std::endl;
    std::cout << "Temperature Up: " << temp_up << " °C " << std::endl;
    std::cout << "Humidity Up: " << hum_up << " % " << std::endl;
    std::cout << "Temperature Down: " << temp_down << " °C " << std::endl;
    std::cout << "Humidity Down: " << hum_down << " % " << std::endl;
    std::cout << "Peltier Temperature Down: " << pelt_temp_down << " °C " << std::endl;
    std::cout << "Peltier Temperature Up: " << pelt_temp_up << " °C " << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
}

