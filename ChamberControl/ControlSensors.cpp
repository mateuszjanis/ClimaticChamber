#include "ControlSensors.h"

double temp_up = -127.0;
double hum_up = -127.0;
double temp_down = -127.0;
double hum_down = -127.0;
double temp_mean = -127.0;
double hum_mean = -127.0;
double pelt_temp_in = -127.0;
double pelt_temp_out = -127.0;

std::string readLastLine(const char* filepath) {

    std::string lastLine;
    
    // 1. Otwarcie pliku na końcu (ate) i w trybie binarnym
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);
    if (!file.is_open()) return "";

    // Pobranie rozmiaru pliku
    std::streamoff pos = file.tellg();
    if (pos == 0) return ""; // Zabezpieczenie przed pustym plikiem

    char ch = '\0';
    
    // Pominięcie ewentualnego pustego znaku nowej linii na samym końcu pliku
    file.seekg(-1, std::ios::end);
    file.get(ch);
    if (ch == '\n') pos--;

    // Szukanie początku ostatniej linii od końca
    while (pos > 0) {
        file.seekg(--pos);
        file.get(ch);
        if (ch == '\n') {
            break; 
        }
    }

    // Zapisanie wyniku do zmiennej
    std::getline(file, lastLine);
    file.close();

    return lastLine;
}

bool readPeltierSensors(){

    const char* filepath = "/home/esp/Data/PeltierTemperature_1.csv";
    if (access( filepath, F_OK ) == -1 ){
        return true; // file does not exist
    }

    std::string sensorsLine = readLastLine(filepath);
    char* endPtr;

    float temp1 = std::strtof(sensorsLine.c_str(), &endPtr);
    float temp2 = std::strtof(endPtr + 1, nullptr);

    if (temp1 < -100 || temp1 > 100 || temp2 < -100 || temp2 > 100) {
        std::cout << "Error reading Peltier sensors: " << temp1 << ", " << temp2 << std::endl;
        return true; // Return true to indicate an error
    } else {
        pelt_temp_in = temp1;
        pelt_temp_out = temp2;
        return false; // Return false to indicate successful reading
    }
    
    return true; // Return true to indicate an error if the line is empty or malformed

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

bool updateSensors() {

    bool sensors_reading_error = false;

    double temp_up_temp = readSensor(temp_up_path);
    double hum_up_temp = readSensor(hum_up_path);
    double temp_down_temp = readSensor(temp_down_path);
    double hum_down_temp = readSensor(hum_down_path);

    if (temp_up_temp < 0 || hum_up_temp < 0) {
        sensors_reading_error =  true; // error
    } else {
        // successful reading
        temp_up = temp_up_temp;
        hum_up = hum_up_temp;
    }

    if (temp_down_temp < 0 || hum_down_temp < 0) {
        sensors_reading_error =  true; // error
    } else {
        // successful reading
        temp_down = temp_down_temp;
        hum_down = hum_down_temp;
    }

    if (!sensors_reading_error) {
        temp_mean = (temp_up + temp_down) / 2.0;
        hum_mean = (hum_up + hum_down) / 2.0;
    }

    if (readPeltierSensors()) {
        sensors_reading_error =   true; // error reading Peltier sensors
    }

    // pelt_temp_in = readSensor(pelt_temp_in_path);
    // pelt_temp_out = readSensor(pelt_temp_out_path);

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
