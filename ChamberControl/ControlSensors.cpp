#include "ControlSensors.h"

double temp_up;
double hum_up;
double temp_down;
double hum_down;
double pelt_temp_in;
double pelt_temp_out;

std::string readLastLine(const char* filepath) {

    std::ifstream file(filepath);
    while (!file.is_open()) {
        std::cout << "Error opening file: " << filepath << ". Trying again..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    file.seekg(0, std::ios::end);
    std::streamoff pos = file.tellg();

    if (pos <= 0)
        return "";

    // Szukamy początku ostatniej linii
    while (pos > 0)
    {
        --pos;
        file.seekg(pos);

        char c;
        file.get(c);

        if (c == '\n')
        {
            ++pos;
            break;
        }
    }

    // Odczytujemy ostatnią linię
    file.clear();
    file.seekg(pos);

    std::string line;
    std::getline(file, line);

    file.close();

    return line;
}

void readPeltierSensors(){

    const char* filepath = "/home/esp/Data/PeltierTemperature_1.csv";
    std::string sensorsLine = readLastLine(filepath);
    char* endPtr;

    float temp1 = std::strtof(sensorsLine.c_str(), &endPtr);
    float temp2 = std::strtof(endPtr + 1, nullptr);

    if (temp1 < -100 || temp1 > 100 || temp2 < -100 || temp2 > 100) {
        std::cout << "Error reading Peltier sensors: " << temp1 << ", " << temp2 << std::endl;
        return;
    }
    
    pelt_temp_in = temp1;
    pelt_temp_out = temp2;

}

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
    temp_mean = (temp_up + temp_down) / 2.0;
    hum_mean = (hum_up + hum_down) / 2.0;
    // pelt_temp_in = readSensor(pelt_temp_in_path);
    // pelt_temp_out = readSensor(pelt_temp_out_path);
    readPeltierSensors();
    
}

void printSensors() {

    updateSensors();

    std::cout << "-------------------- Sensor Readings --------------------" << std::endl;
    std::cout << "TempDown: " << temp_down << " °C ";
    std::cout << "HumDown:  " << hum_down << " % " << std::endl;
    std::cout << "TempUp:   " << temp_up << " °C ";
    std::cout << "HumUp:    " << hum_up << " % " << std::endl;
    std::cout << "PeltTempDown: " << pelt_temp_in << " °C ";
    std::cout << "PeltTempUp: " << pelt_temp_out << " °C " << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;
}

void ReciveSensorsData() {
    while (true) {
        updateSensors();
        printSensors();
        std::this_thread::sleep_for(std::chrono::seconds(sensors_update_interval));
    }
}