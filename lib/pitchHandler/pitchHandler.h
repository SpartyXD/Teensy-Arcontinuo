/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#pragma once
#include <Arduino.h>
// Surface


class PitchHandler {
    
  public:


    static const byte MAX_ROWS = 52;
    static const byte MAX_COLS = 11;
    byte id = 0;
    byte rows = 10;
    byte cols = 1;
   
    byte pitches[MAX_COLS][MAX_ROWS] = {};
    byte transpositions[MAX_COLS] = {0};
    byte base_transposition = 0;

    void begin();
    byte getPitch(float x, float y);
    byte getPreviousPitch(float x, float y);
    byte getNextPitch(float x, float y);    
    int getPositionInGrid(float value, int steps);
    float getLowerBound(float,int);
    float getUpperBound(float,int);
    void setDefaultMap();
    void setMPEMap();
    void set3zonesMap();
    void set1ColBluesMap();
    void set1ColBluesMapLong();
    void set2ColBluesMap();
    void set3ColBluesMap();
    void set4ColBluesMap();
    void setTibetanMap();
    void setArpMap();
    void setContinousMap();

};
