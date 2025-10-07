
#pragma once
#include <Arduino.h>

class Utils {
    public:
        void dispInt(const char *text, int value); 
        void dispFloat(const char *text, float value);
        void dispUnsignedLong(const char *text, unsigned long value);
        void disp(const char *text);
        void error(const char *text);
        int scale(float value, int low, int high);
        int quantize(float value, int low, int high);
        int qmap(float value, int low, int high);
        int pitchBendMap(float val, float range);
        int brightnessMap(float val, float range);
};