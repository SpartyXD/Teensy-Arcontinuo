/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */
 
#pragma once
#include <Arduino.h>

class Settings;

class MidiHandler {
    
  public:
      
    // Control numbers
    static const byte MAIN_MIDI_CHANNEL = 1;

    // Global controls in main midi channel
    static const byte TX_THRESHOLD = 48;
    static const byte TX_PERCENTAGE_TOP = 49;
    static const byte TX_PERCENTAGE_BOTTOM = 50;
    static const byte TX_MIN_AREA = 51;
    static const byte TX_MIN_DIST = 52;    
    static const byte TX_TOP_LINES = 53;
    static const byte TX_BOTTOM_LINES = 54;
    static const byte TX_TRACKER_TIME = 55;
    static const byte TX_TRACKER_DIST = 56;
    static const byte TX_NUM_BLOBS = 57;
    static const byte TX_ON_OFF = 58;
  
    static const byte TX_PRESET = 60;
    static const byte TX_SERIAL_NUM = 61;  
  
    static const byte TX_GYRO_X = 113;
    static const byte TX_GYRO_Y = 114;
    static const byte TX_GYRO_Z = 115;
    static const byte TX_BUTTON_BASE = 104;

    // Blob messages per channel
    static const byte TX_BLOB_X = 50;
    static const byte TX_BLOB_Y = 51;
    static const byte TX_BLOB_Z = 52;
    static const byte TX_BLOB_Z_MAX = 53;
    static const byte TX_BLOB_A = 54;
    static const byte TX_BLOB_DIST_X = 55;
    static const byte TX_BLOB_DIST_Y = 56; 
    static const byte TX_BLOB_ACTIVE = 57; 
    static const byte TX_BLOB_BANG = 58;

    // Blob messages per main channel
    static const byte TX_BLOBS_BANG = 84;
  
    static const byte TX_BRIGHTNESS = 74;

    // Data received by teensy
    static const byte RX_THRESHOLD = 48;
    static const byte RX_PERCENTAGE_TOP = 49;
    static const byte RX_PERCENTAGE_BOTTOM = 50;
    static const byte RX_MIN_AREA = 51;
    static const byte RX_MIN_DIST = 52;
    static const byte RX_TOP_LINES = 53;
    static const byte RX_BOTTOM_LINES = 54;
    static const byte RX_TRACKER_TIME = 55;
    static const byte RX_TRACKER_DIST = 56;
    static const byte RX_RESET = 57;
    static const byte RX_WRITE_CONFIG_EEPROM = 58;
          
    static const byte RX_PRESET = 60;
    static const byte RX_SERIAL_NUM = 61;  
           
    byte programNumber = 1;
    byte enablePitchBend = 1;
    float pitchBendFactor = 1.0;
    byte enableVibrato = 0;
    int vibratoController = 107;
    byte enableFilter = 0;
    int filterController = 24;
    byte enableBrightness = 1;
  
    byte sendBlobData = 1;
  
    MidiHandler();
    void changeProgram();
    void setProgram(int i);
    void receiveControlChange(byte channel, byte control, byte value, Settings *);

};
