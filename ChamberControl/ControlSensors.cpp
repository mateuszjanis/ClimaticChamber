#include "ControlSensors.h"

const std::string temp_up_path = "/sys/bus/iio/devices/iio:device0/in_temp_input";
const std::string hum_up_path  = "/sys/bus/iio/devices/iio:device0/in_humidityrelative_input";
const std::string temp_down_path = "/sys/bus/iio/devices/iio:device1/in_temp_input";
const std::string hum_down_path  = "/sys/bus/iio/devices/iio:device1/in_humidityrelative_input";

double temp_up;
double hum_up;
double temp_down;
double hum_down;

double read_sensor(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return -1.0;
    }
    double val;
    file >> val;
    return val / 1000.0;
}

void update_sensors() {
    temp_up = read_sensor(temp_up_path);
    hum_up = read_sensor(hum_up_path);
    temp_down = read_sensor(temp_down_path);
    hum_down = read_sensor(hum_down_path);
}

void print_sensors() {
    std::cout << "Temperature Up: " << temp_up << " °C " << std::endl;
    std::cout << "Humidity Up: " << hum_up << " % " << std::endl;
    std::cout << "Temperature Down: " << temp_down << " °C " << std::endl;
    std::cout << "Humidity Down: " << hum_down << " % " << std::endl;
}

