#pragma once
#include <Arduino.h>


struct Blob {
    int id = 0;
    int x_sum = 0;
    int y_sum = 0;
    int count = 0;
    int pressure = 0;
};

class BlobSender {
public:
    BlobSender(HardwareSerial &serialPort, int baudRate = 115200);
    void begin();
    void sendBlobData(Blob blobs[], int numBlobs);

private:
    HardwareSerial &serial;
    static const byte DATA_HEAD = 10;
    static const byte BLOB_SIZE = 9;
    static const byte MAX_BLOBS = 10;
    static const byte SEP_VALUE = 255;
    byte blobData[DATA_HEAD + BLOB_SIZE * MAX_BLOBS + 1] = {0};
    void generateBlobData(Blob blobs[], int numBlobs);
};
