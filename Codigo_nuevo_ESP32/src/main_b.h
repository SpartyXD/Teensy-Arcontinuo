#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
#include <blob_algo.h>
#include <misc.h>

//=================================
//Global structs

//---------------------------------------------
//Data handling

typedef uint8_t (*MatrixPtr_t)[TX_COUNT][RX_COUNT];
SemaphoreHandle_t data_lock; 

uint8_t matrix_a[TX_COUNT][RX_COUNT] = {0};
uint8_t matrix_b[TX_COUNT][RX_COUNT] = {0};

MatrixPtr_t matrices[2] = {
    &matrix_a, 
    &matrix_b
};

uint8_t sender_index = 0;
uint8_t receiver_index = 1;

//---------------------------------------------
//Serial 
HardwareSerial DataSerial(1); //UART 1
uint8_t frame[RX_COUNT * (1 + TX_COUNT)];


//=================================
//Funciones auxiliares

//---------------------------------------------
//Data Handling

void switch_ptrs(){
    if(xSemaphoreTake(data_lock, portMAX_DELAY) == pdTRUE){
        sender_index = !sender_index;
        receiver_index = !receiver_index;
        xSemaphoreGive(data_lock);
    }

}

//---------------------------------------------
//Serial 

void initData(int rxPin, int txPin, uint32_t baudrate=115200){
  DataSerial.begin(baudrate, SERIAL_8N1, rxPin, txPin);
  data_lock = xSemaphoreCreateMutex();
  delay(1000);
}


void readData(){
    if (DataSerial.available()) {
        uint8_t b = DataSerial.read();

        if (b != FRAME_START)
            return;

        //Nuevo frame
        int total_bytes = TX_COUNT * (1 + RX_COUNT);
        int bytes_read = DataSerial.readBytes(frame, total_bytes);

        if (bytes_read == total_bytes){
            Serial.println("\nNuevo frame recibido:");
            
            int idx = 0;
            rep(i, TX_COUNT){
                rep(j, RX_COUNT)
                    matrices[sender_index][i][j] = frame[idx++];
            }
        } 
        else{
            Serial.print("Hubo error leyendo frame (");
            Serial.print(bytes_read);
            Serial.println(" bytes)");
        }
    }
}


//TODO: Placeholder para procesar datos usando blob_algo.h
void process_data(){
    
}


//=================================

void core_B_main(void *params){
    while(true){
        readData();
        process_data();
        switch_ptrs();
        vTaskDelay(30);
    }

}