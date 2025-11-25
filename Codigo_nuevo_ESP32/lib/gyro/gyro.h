

#pragma once 

#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

#include "utils.h"
#include "stub.h"
#include "dataHandler.h"


class Settings; // Declaración anticipada
class Gyro : Utils {
    public:
        Settings* settings; 
        MPU6050 mpu;
        float gyroData[10] = {0};
        float compassx[DataHandler::MAX_BLOBS] = {0};
        float compassy[DataHandler::MAX_BLOBS] = {0};

        // Diego: filtro de media movil con 30 muestras
        static const byte FILTER_ORDER = 30;
        float filterX[FILTER_ORDER] = {0};
        float filterY[FILTER_ORDER] = {0};

        Gyro();

        void begin();
        void setSettings(Settings* s);
        void readGyroData();
        void sendGyroDataAsMIDI(); // Diego: nohace nada
        void filterGyroX();
        void filterGyroY();
        void printGyroData();

        // Diego: funciones parte del codigo anterior que no tienen
        // definicion
        // void printGyroDataGroup(int,int,int);
        // int mapCirc(int);
        // int compassMap(float,float);
        // int whammyMap(float value);  

};