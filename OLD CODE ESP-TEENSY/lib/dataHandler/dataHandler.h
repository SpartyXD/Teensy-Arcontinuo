/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */
#pragma once
#include <Arduino.h>
#include "utils.h"

class Settings;
class MidiHandler;
class PitchHandler;



class DataHandler : Utils {
    
  public:

    Settings *settings;
    PitchHandler *pitchHandler;

    static const byte DATA_HEAD = 9; // Extra data: Threshold, Number of detected blobs
    static const byte BLOB_SIZE = 9;  // Allocated data for each blob in the serial buffer
    static const byte DATA_SIZE = BLOB_SIZE*10 + DATA_HEAD + 1; // Serial data buffer length 9*10 + DATA_HEAD + 1 (255)
    static const byte SEP_VALUE = 255; // Separation value for delimiting blob data
  
    static const byte POS_ID_1 = DATA_HEAD + 1; // Position in buffer of BlobID 1
    static const byte POS_ID_2 = POS_ID_1 + BLOB_SIZE;  // Position in buffer of BlobID 1
    static const byte POS_ID_3 = POS_ID_2 + BLOB_SIZE;  // Position in buffer of BlobID 2
    static const byte POS_ID_4 = POS_ID_3 + BLOB_SIZE;  // Position in buffer of BlobID 3
    static const byte POS_ID_5 = POS_ID_4 + BLOB_SIZE;  // Position in buffer of BlobID 4
    static const byte POS_ID_6 = POS_ID_5 + BLOB_SIZE;  // Position in buffer of BlobID 5
    static const byte POS_ID_7 = POS_ID_6 + BLOB_SIZE;  // Position in buffer of BlobID 6
    static const byte POS_ID_8 = POS_ID_7 + BLOB_SIZE;  // Position in buffer of BlobID 7
    static const byte POS_ID_9 = POS_ID_8 + BLOB_SIZE;  // Position in buffer of BlobID 8
    static const byte POS_ID_10 = POS_ID_9 + BLOB_SIZE; // Position in buffer of BlobID 9

    // Blob data
    static const byte MAX_BLOBS = 10; 
    static const byte MAX_X = 18;
    static const byte MAX_Y = 84;
    static const byte MAX_AVG_Z = 40;
    static const byte MAX_Z = 80;
    static const byte MAX_A = 110;

    static const byte BLOB_LIFE_THRESHOLD = 200;
    float velThreshold = 0.005;

    int lastNumBlobs = 0;    
    int ind=0;

    float lastX[10] = {0,0,0,0,0,0,0,0,0,0};
    float lastY[10] = {0,0,0,0,0,0,0,0,0,0};
    float lastZ[10] = {0,0,0,0,0,0,0,0,0,0};
    float lastA[10] = {0,0,0,0,0,0,0,0,0,0};

    float distancex[MAX_BLOBS] = {0};
    float distancey[MAX_BLOBS] = {0};  
    float lastdistancex[MAX_BLOBS] = {0};
    float lastdistancey[MAX_BLOBS] = {0};  
    float velocityx[MAX_BLOBS] = {0};
    float velocityy[MAX_BLOBS] = {0};  
    float initx[MAX_BLOBS] = {0};
    float inity[MAX_BLOBS] = {0};


    float PITCH_BEND_SEMITONE = 170.6666;


    byte blobStatus[MAX_BLOBS] = {0};
    //byte changed[MAX_BLOBS] = {0};
    byte pitches[MAX_BLOBS] = {0};
    byte slidePitches[MAX_BLOBS] = {0};
    bool slideCross = false;
    int centerPitchBend[MAX_BLOBS] = {0};

    unsigned long life[MAX_BLOBS] = {0};
    unsigned long inittime[MAX_BLOBS] = {0};
    
  
    byte inputData[2*(DATA_SIZE-1)+1] = {0};
  
    float blobData[60] = {0};
    float lastBlobData[60] = {0};
  
    static const byte INACTIVE = 0;
    static const byte ACTIVE = 0;

    void setSettings(Settings *);
    void setPitchHandler(PitchHandler *);
    void processData();
    void checkSerialData();
    void readBlobData();
    void checkBlobStatus(bool);
    void fixFirstBlob();
    void copyLastBlobData();
    bool checkSerialBuffer(int index);
    void printBlobData();
    void printInputData();
    void updateSettingsData();
    void outputMidiData();
    void sendMPEData(int id, float x, float y, float z);

    //void sendMidiOn(int,float,float,float);
    //void sendMidiOff(int);
    //void updateMidiOnOff(int,float,float,float,float,float,float,float,float);

    void sendMidiOn(int,float,float,float,float,float,float);
    void sendMidiOff(int);
    void updateMidiOnOff(int,float,float,float,float,float,float);

};
