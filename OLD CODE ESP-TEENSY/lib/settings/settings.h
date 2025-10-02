/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <utils.h>

class MidiHandler;
class Logo;
class PitchHandler;

class Settings : Utils {
    
  public:

    static const byte BASE_EEPROM_CONFIG_VALUES = 10;
    static const byte MODE_SLIDE = 0;
    static const byte MODE_DISCRETE = 1;
    static const byte MODE_DISCRETE_SLIDE = 2;

    MidiHandler* midiHandler;
    Logo* logo;
    PitchHandler* pitchHandler;
    
    int thr = 20;
    int percentageTop = 90;
    int percentageBottom = 10;
    int minArea = 2;
    int minDist = 2;
    int topLines = 1;
    int bottomLines = 1;
    int timeTracker = 2;
    int distTracker = 4;
    byte enableGyro = 1;
    float areaThreshold = 0.1;
    int numBlobs = 0;

    
    byte serialNum = 0;
    byte oneFinger = 0;
    int configmode = 1; // 0 for selection, 1 for playing

    byte performanceMode = MODE_SLIDE;
    byte preset = 0;
  
    bool posToLight = true;

    unsigned long previousMillis=0;
    unsigned long timeInterval=0;


    void begin();
    void setMidiHandler(MidiHandler*);
    void setLogo(Logo*);
    void setPitchHandler(PitchHandler*);
    void setSerialNum(byte value);
    void setPreset(byte p);
    void startPreset();
    void stopPreset();
    void presetUp();
    void presetDown();
    void changePerformanceMode();
    void sendInitValuesToFPGA();
    void sendInitValuesToMidi();

    void readInitValuesFromEEPROM();
    void writeInitValuesToEEPROM(byte);
    
    void sendThresholdToFPGA(byte value);
    void sendPercentageTopToFPGA(byte value);
    void sendPercentageBottomToFPGA(byte value);
    void sendMinAreaToFPGA(byte value);
    void sendMinDistToFPGA(byte value);
    void sendTopLinesToFPGA(byte value);
    void sendBottomLinesToFPGA(byte value);
    void sendTrackerTimeToFPGA(byte value);
    void sendTrackerDistToFPGA(byte value);
    void rstMins(byte value);

};
