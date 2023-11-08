// This file will contain all the code to log the data internally before they get transmitted.
// It uses an internal memory of the MKR board : 2MByte of persistent flash memory.


// Check that the file has not been imported before
#ifndef INTERNAL_LOG
#define INTERNAL_LOG

#include <SD.h>
#include <SPI.h>
#include <String.h>
#include "Measure.hpp"


// const char filename[] = "datalog.csv";
const char header[] = "Id,Date,Time,Capteur1,Capteur2,Capteur3,Capteur4";


bool AlreadyInitialised() {
    if (SD.exists(filename) == false) {
      return false;
    }
    else{
      File file =  SD.open(filename);
      int a = file.available();
      file.close();
      return (a > 0);
    }
}

//Generate a CSV file with a header IF necessary.
bool InitialiseLog(const int CSpin) {
    if (!SD.begin(CSpin)) {
      return false;
    }

    else if (!AlreadyInitialised()) {
        File file = SD.open(filename, FILE_WRITE);
        if (file) {
            file.println(header);
        }
        file.close();
    }

    return true;
}


#endif