#include "BlobSender.h"

BlobSender::BlobSender(HardwareSerial &serialPort, int baudRate) : serial(serialPort) {}

void BlobSender::begin() {
    serial.begin(300000);
    Serial.println("BlobSender initialized");
}

void BlobSender::generateBlobData(Blob blobs[], int numBlobs) {
    blobData[0] = SEP_VALUE; // Separator value
    
    // Configuration parameters
    blobData[1] = 10;  // thr
    blobData[2] = 100;  // percentageTop
    blobData[3] = 1;  // percentageBottom
    blobData[4] = 1;   // minArea
    blobData[5] = 1;   // minDist
    blobData[6] = 6;   // topLines
    blobData[7] = 6;   // bottomLines
    blobData[8] = 2;   // timeTracker
    blobData[9] = 4;   // distTracker
    blobData[10] = numBlobs;  // Guardar el número de blobs
    
    for (int i = 0; i < MAX_BLOBS; i++) {
        int index = DATA_HEAD + i * BLOB_SIZE;
        if (i < numBlobs) {
            blobData[index] = blobs[i].id;  // Blob ID
            blobData[index + 1] = blobs[i].x_sum ;  // X position
            blobData[index + 2] = (blobs[i].x_sum % blobs[i].count) * 256 / blobs[i].count; // X fractional
            blobData[index + 3] = blobs[i].y_sum ;  // Y position
            blobData[index + 4] = (blobs[i].y_sum % blobs[i].count) * 256 / blobs[i].count; // Y fractional
            blobData[index + 5] = blobs[i].pressure ;  // Avg Z
            blobData[index + 6] = (blobs[i].pressure % blobs[i].count) * 1024 / blobs[i].count; // Avg Z fractional
            blobData[index + 7] = blobs[i].pressure;  // Max Z
            blobData[index + 8] = blobs[i].count;  // Area
        } else {
            memset(&blobData[index], 0, BLOB_SIZE); // Llenar con ceros los blobs no usados
        }
    }
}

void BlobSender::sendBlobData(Blob blobs[], int numBlobs) {
    generateBlobData(blobs, numBlobs);

    // 🔥 Debug: Imprimir los datos de los primeros dos blobs en una sola línea
    /*
    Serial.print("DEBUG: ");
    for (int i = 0; i < 1 && i < numBlobs; i++) {  // Solo los primeros dos blobs
        int index = DATA_HEAD + i * BLOB_SIZE;
        Serial.print("[ID:"); Serial.print(blobData[index]); 
        Serial.print(" X:"); Serial.print(blobData[index + 1]);
        Serial.print("."); Serial.print(blobData[index + 2]);
        Serial.print(" Y:"); Serial.print(blobData[index + 3]);
        Serial.print("."); Serial.print(blobData[index + 4]);
        Serial.print(" Z:"); Serial.print(blobData[index + 5]);
        Serial.print("]  ");
    }
    Serial.println();
    */

    // Enviar los datos por UART
    serial.write(blobData, DATA_HEAD + BLOB_SIZE * MAX_BLOBS + 1); // Siempre enviar tamaño fijo
}
