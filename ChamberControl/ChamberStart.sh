g++ -Wall main.cpp PeltierController.cpp ControlSensors.cpp DataHandler.cpp -o ChamberControl -lgpiodcxx -pthread -lcurl
./ChamberControl