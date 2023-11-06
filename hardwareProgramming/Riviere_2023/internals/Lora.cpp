// This file will contain all functions relative to LoRa communication

// Check that the file has not been imported before
#ifndef MY_LORA
#define MY_LORA


#include "Lora.hpp"


// Initialise the lora module for the first time. Call before any other LoRa function
void InitialiseLora(float frequency) {
  LoRa.begin(frequency);

  LoRa.onReceive(OnLoraReceivePacket);
  LoRa.receive();
}

void OnLoraReceivePacket(int packetSize) {
  // If the header is incomplete, ignore the packet
  if (packetSize < 13) {
    ClearBytes(packetSize);
    return;
  }

  
  unsigned int senderId = ReadFromLoRa<unsigned int>();
  unsigned int destinationId = ReadFromLoRa<unsigned int>();

  if (destinationId != networkId) {
    ClearBytes(packetSize - 8);
    return;
  }

  unsigned int thisPacketNumber = ReadFromLoRa<unsigned int>();

  // If the packet has already been received, ignore the packet
  if (thisPacketNumber < receivedPacketNumber) {
    ClearBytes(packetSize - 12);
    return;
  }
  receivedPacketNumber = thisPacketNumber;

  char requestId = LoRa.read();

  if (requestId == DT_REQ) {
    HandleDataRequest(senderId);
    return;
  }
}


void HandleDataRequest(unsigned int senderId) {
  unsigned int requestedSampleId = ReadFromLoRa<unsigned int>();
  unsigned int lastSampleId = GetLastMeasurementId();

  for (unsigned int id = requestedSampleId; id <= lastSampleId; id++)
  {
    Measure measure = GetMeasurementById(id);
    SendMeasurement(measure, senderId);
  }
}


void ClearBytes(unsigned int length) {
  for (size_t i = 0; i < length; i++)
  {
    LoRa.read();
  }
}


template<typename T>
T ReadFromLoRa() {
  char bytes[sizeof(T)];

  for (size_t i = 0; i < sizeof(T); i++)
  {
    bytes[i] = static_cast<char>(LoRa.read());
  }
  
  return *reinterpret_cast<T*>(&bytes);
}


// Temporarily disable the LoRa module to save battery. It can be waken up with WakeUpLora
void SleepLora() {
  LoRa.sleep();
}


// Re-enable the LoRa module if it was asleep. I.E. Exit low-power mode for the LoRa module
void WakeUpLora() {
  // TODO : test that this works
  LoRa.receive();
}


bool SendPacket(const void* payload, unsigned int payloadSize, unsigned int destinationId, RequestType requestType) {
  bool success = (bool)LoRa.beginPacket();
  if (!success) {
    return false;
  }

  LoRa.write(reinterpret_cast<uint8_t*>(&networkId), sizeof(networkId));
  LoRa.write(reinterpret_cast<uint8_t*>(&destinationId), sizeof(destinationId));
  LoRa.write(reinterpret_cast<uint8_t*>(&sentPacketNumber), sizeof(sentPacketNumber));
  LoRa.write(reinterpret_cast<uint8_t*>(&requestType), sizeof(requestType));
  LoRa.write(reinterpret_cast<uint8_t*>(&payload), payloadSize);

  success = (bool)LoRa.endPacket();

  sentPacketNumber++;

  return success;
}


// The exact signature of this function has to be determined
bool SendMeasurement(Measure measure, unsigned int destinationId) {
  return SendPacket(&measure, sizeof(measure), destinationId, DT_RPL);
}


#endif