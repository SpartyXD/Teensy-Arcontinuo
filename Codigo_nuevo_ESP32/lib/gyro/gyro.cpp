#include "gyro.h"
#include "settings.h"
#include <Wire.h>
#include <MPU6050_tockn.h>

// Diego: Esto se puso a llorar por la contruccion del Gyro
Gyro::Gyro() : mpu(Wire) {}

void Gyro::begin() {
    Wire.begin();
    Serial.println("Inicializing MPU6050 (GY-521)...");
    mpu.begin();
    mpu.calcGyroOffsets(true);
    Serial.println("MPU6050 initialized and calibrated.");

}

void Gyro::setSettings(Settings* s) {
    settings = s;
}

void Gyro::readGyroData(){
    mpu.update();

    gyroData[3] = mpu.getGyroX();
    gyroData[4] = mpu.getGyroY();
    gyroData[5] = mpu.getGyroZ();

    // codigo de prueba
    // Serial.print(gyroData[3]);
    // Serial.print(",");
    // Serial.print(gyroData[4]);
    // Serial.print(",");
    // Serial.print(gyroData[5]);
    // Serial.println(" ");

    filterGyroX();
    filterGyroY();
    sendGyroDataAsMIDI();
}

void Gyro::sendGyroDataAsMIDI(){
    
    float midiX = map(gyroData[4], -250, 250, 0, 127);
    float midiY = map(gyroData[3], -250, 250, 0, 127);
    float midiZ = map(gyroData[5], -250, 250, 0, 127);

    usbMIDI.sendControlChange(117, (int)midiX, 1);
    usbMIDI.sendControlChange(118, (int)midiY, 1);
    usbMIDI.sendControlChange(119, (int)midiZ, 1);
}

// Diego: Filtros de media movil


// Para X
void Gyro::filterGyroX() {
    float value = 0;
    for (int i = 0; i < FILTER_ORDER - 1; i++) {
        filterX[i] = filterX[i + 1];
        value += filterX[i];
    }
    filterX[FILTER_ORDER - 1] = gyroData[4];
    value += filterX[FILTER_ORDER - 1];
    gyroData[4] = value / FILTER_ORDER;
}

// Para Y
void Gyro::filterGyroY() {
    float value = 0;
    for (int i = 0; i < FILTER_ORDER - 1; i++) {
        filterY[i] = filterY[i + 1];
        value += filterY[i];
    }
    filterY[FILTER_ORDER - 1] = gyroData[3];
    value += filterY[FILTER_ORDER - 1];
    gyroData[3] = value / FILTER_ORDER;
}

// Diego: Prints

void Gyro::printGyroData() {
    for (int i = 0; i < 10; i++) {
        Serial.print(gyroData[i]);
        Serial.print("\t");
    }
    Serial.println(".");
}
