g++ -Wall ChamberControl.cpp PeltierController.cpp ControlSensors.cpp DataHandler.cpp ServerHandler.cpp -o ChamberControl -lgpiodcxx -pthread -lcurl
./ChamberControl