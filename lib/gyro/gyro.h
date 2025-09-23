/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

 #pragma once
 #include <Arduino.h>
 #include <Wire.h>
 #include <MPU6050_tockn.h>
 
 #include "midiHandler.h"
 #include "dataHandler.h"
 #include "utils.h"
 
 class Settings;
 class Gyro : Utils {
  public:
    Settings* settings;
    MPU6050 mpu; // Gyroscope object
    float gyroData[10] = {0};
    float compassx[DataHandler::MAX_BLOBS] = {0};
    float compassy[DataHandler::MAX_BLOBS] = {0};

    static const byte FILTER_ORDER = 30;
    float filterX[FILTER_ORDER] = {0};
    float filterY[FILTER_ORDER] = {0};

    float avgGyroX = 0;
    float avgGyroY = 0;

    Gyro(); // Constructor

    void begin();
    void setSettings(Settings *);
    void readGyroData();   
    void sendGyroDataAsMIDI();
    void printGyroData();
    void printGyroDataGroup(int,int,int);
    int mapCirc(int);
    int compassMap(float,float);
    int whammyMap(float value);
    void filterGyroX();
    void filterGyroY();
};
