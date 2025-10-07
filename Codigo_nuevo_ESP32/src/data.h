/*
=================================================================
  Modulo que maneja la recepcion de datos desde la ESP32
=================================================================
*/

#ifndef DATA_H
#define DATA_H

#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial DataSerial(1); //UART 1

//---------------------------------------------
//Function declarations
void initData(int rxPin, int txPin, uint32_t baudrate);
bool readData(String &msg);

//---------------------------------------------
//Implementations

void initData(int rxPin, int txPin, uint32_t baudrate=115200){
  DataSerial.begin(baudrate, SERIAL_8N1, rxPin, txPin);   
  delay(1000);
}


bool readData(String &msg){
  if (DataSerial.available()) {
    msg = DataSerial.readStringUntil('\n');
    msg.trim();
    return true;
  }
  return false;
}



#endif