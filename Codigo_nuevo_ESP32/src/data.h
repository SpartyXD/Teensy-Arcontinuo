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
uint8_t frame[RX_COUNT * (1 + TX_COUNT)];

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


void readData(){
if (DataSerial.available()) {
    uint8_t b = DataSerial.read();

    //Nuevo frame
    if (b == FRAME_START){
      int total_bytes = TX_COUNT * (1 + RX_COUNT);
      int bytes_read = DataSerial.readBytes(frame, total_bytes);

      if (bytes_read == total_bytes) {
        Serial.println("\nNuevo frame recibido:");

        int index = 0;
        for (int i = 0; i < TX_COUNT; i++) {
          uint8_t channel = frame[index++];
          Serial.print("TX");
          Serial.print(i);
          Serial.print(": ");

          for (int j = 0; j < RX_COUNT; j++) {
            uint8_t value = frame[index++];
            Serial.print(value);
            if (j < RX_COUNT - 1) Serial.print("\t");
          }
          Serial.println();
        }
      } 
      else{
        Serial.print("Hubo error leyendo frame (");
        Serial.print(bytes_read);
        Serial.println(" bytes)");
      }
    }
  }
}



#endif