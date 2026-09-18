g++ -Wall ChamberControl.cpp PeltierController.cpp ControlSensors.cpp DataRecording.cpp -o ChamberControl -lgpiodcxx -pthread -lcurl
./ChamberControl