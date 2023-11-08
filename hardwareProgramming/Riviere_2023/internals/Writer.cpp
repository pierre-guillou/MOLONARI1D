#include <SD.h>
#include <SPI.h>
#include <String.h>
#include "Measure.h"
#include "Writer.h"

#ifndef WRITER_CLASS
#define WRITER_CLASS


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

// TODO : Get the current time from the RTC. (not necessarly here)
void GetCurrentTime(Measure* measure) {
    // Not implemented yet.
    strncpy(measure->time, "12:00:00", 9);
    strncpy(measure->date, "01/01/2020", 11);
}

//Class methods

void Writer::WriteInNewLine(Measure data){
    this->file.print(data.id);
    this->file.print(",");
    this->file.print(data.date);
    this->file.print(",");
    this->file.print(data.time);
    this->file.print(",");
    this->file.print(data.mesure1);
    this->file.print(",");
    this->file.print(data.mesure2);
    this->file.print(",");
    this->file.print(data.mesure3);
    this->file.print(",");
    this->file.println(data.mesure4);

    this->file.flush();
}

void Writer::ConvertToWriteableMeasure(Measure* measure, MEASURE_T mesure1, MEASURE_T mesure2, MEASURE_T mesure3, MEASURE_T mesure4) {
    measure->mesure1 = mesure1;
    measure->mesure2 = mesure2;
    measure->mesure3 = mesure3;
    measure->mesure4 = mesure4;
}

void Writer::EstablishConnection() {
    this->next_id = GetNextLine();
    this->file = SD.open(filename, FILE_WRITE);
}

void Writer::LogData(MEASURE_T mesure1, MEASURE_T mesure2, MEASURE_T mesure3, MEASURE_T mesure4) {

    // Create a new Measure
    Measure data;
    this->ConvertToWriteableMeasure(&data, mesure1, mesure2, mesure3, mesure4);
    GetCurrentTime(&data);
    data.id = this->next_id;

    if (this->file){
        WriteInNewLine(data);
    }

    this->next_id++;
}

#endif