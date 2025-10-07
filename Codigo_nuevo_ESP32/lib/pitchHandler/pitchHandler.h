

#pragma once 


#include <Arduino.h>


class PitchHandler {

    public: 
        static const byte MAX_COLS = 11;
        static const byte MAX_ROWS = 52;
        byte rows = 10;
        byte cols = 1;


        byte pitches[MAX_COLS][ MAX_ROWS] = {};
        byte transpositions[MAX_COLS] = {0};
        byte base_transposition = 0;



        void begin();
        byte getPitch(float x, float y);
        byte getPreviousPitch(float x, float y);
        byte getNextPitch(float x, float y);
        int getPositionInGrid(float value, int steps);
        float getLowerBound(float value, int steps);
        float getUpperBound(float value, int steps);
        void set3zonesMap();
        void setDefaultMap();
        void setMPEMap();
        void set1ColBluesMap();
        void set1ColBluesMapLong();
        void set2ColBluesMap();
        void set3ColBluesMap();
        void set4ColBluesMap();
        void setTibetanMap();
        void setArpMap();
        void setContinousMap();
};