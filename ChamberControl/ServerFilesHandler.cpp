#include "ServerFilesHandler.h"

void runDataRecording(){

    saveDataLocally();
    

}

bool sendState();
bool sendImage();
bool createNewFile();
void actualizePayload();

void appendData();

bool appendClimateData();
bool appendGasData();

std::string findImageToSend();