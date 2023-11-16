

// Check that the file has not been imported before
#ifndef WRITER_CLASS
#define WRITER_CLASS

#include "Writer.hpp"
#include "Time.cpp"
#include <String.h>
#include <SD.h>

#ifdef SD_DEBUG
#define SD_LOG_LN(msg) Serial.println(msg)
#else
#define SD_LOG_LN(msg)
#endif


const String COMA = String(',');

// Search for the number of lines in the csv file->
// SHOULD BE CALLED ONLY ONCE.
unsigned int GetNextLine() {
  File readInfile = SD.open(filename);
  unsigned int number_of_lines = 0;
  if (readInfile) {
    while (readInfile.available()) {
      // read an entire csv line (which end is a \n)
      if (readInfile.read() == '\n') {
        number_of_lines++;
      }
    }
  }
  readInfile.close();
  return number_of_lines;
}


void GetCurrentTime(Measure* measure) {
    // Not implemented yet.
    GetCurrentHour().toCharArray(measure->time, 9);
    GetCurrentDate().toCharArray(measure->date, 11);
}

//Class methods

void Writer::WriteInNewLine(Measure data){
    
    SD_LOG_LN("Writing data ...");
    this->file.println(String(data.id)+ COMA + data.date + COMA + data.time + COMA + String(data.mesure1) + COMA + String(data.mesure2) + COMA + String(data.mesure3) + COMA + String(data.mesure4));
    SD_LOG_LN("Done");

    SD_LOG_LN("Flushing ...");
    this->file.flush();
    SD_LOG_LN("Done");
}

void Writer::ConvertToWriteableMeasure(Measure* measure, MEASURE_T mesure1, MEASURE_T mesure2, MEASURE_T mesure3, MEASURE_T mesure4) {
    measure->mesure1 = mesure1;
    measure->mesure2 = mesure2;
    measure->mesure3 = mesure3;
    measure->mesure4 = mesure4;
}

bool Writer::Reconnect() {
    this->file.close();
    if (!SD.begin(this->CSPin)){
        SD_LOG_LN("Connection could not be established.");
        return false;
    }
    this->file = SD.open(filename, FILE_WRITE);
    return true;
}

void Writer::EstablishConnection(const int CSpin) {
    this->CSPin = CSpin;
    this->next_id = GetNextLine();
    this->file = SD.open(filename, FILE_WRITE);
}

void Writer::LogData(MEASURE_T mesure1, MEASURE_T mesure2, MEASURE_T mesure3, MEASURE_T mesure4) {

    // Create a new Measure
    Measure data;
    this->ConvertToWriteableMeasure(&data, mesure1, mesure2, mesure3, mesure4);
    GetCurrentTime(&data);
    data.id = this->next_id;
    bool is_connected = true;

    // Check if the connection is still established
    if (!SD.begin(this->CSPin) || !this->file){
        SD_LOG_LN("SD connection lost.");
        SD_LOG_LN("Trying to reconnect ...");
        
        // Try to reconnect
        is_connected = this->Reconnect();
        }

    if (!is_connected) {
        SD_LOG_LN("Connection could not be established.");
        return;
    }
    else {
    this->WriteInNewLine(data);
    this->next_id++;
    }
}

void Writer::Dispose() {
    this->file.close();
}

#endif