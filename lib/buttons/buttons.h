/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#pragma once
#include <Arduino.h>
#include "utils.h"

class Settings;
class PitchHandler;
class MidiHandler;

class Buttons : Utils {
    
  public:

    Settings* settings;
    PitchHandler* pitchHandler;
    
    static const byte LEFT_LEFT = 1;
    static const byte MIDDLE_LEFT = 0;
    static const byte MIDDLE_RIGHT = 3;
    static const byte RIGHT_RIGHT = 2;
    
    static const byte NUM_BUTTONS = 4; // Number of buttons
    const int btnPins[NUM_BUTTONS] = {7,8,9,10}; // Button pins, re-order
    unsigned long time_pressed[NUM_BUTTONS] = {0}; // Buttons states
 
    byte state[NUM_BUTTONS] = {HIGH}; // Buttons states

    int configmode = 1;


    Buttons();
    void setSettings(Settings*);
    void setPitchHandler(PitchHandler *);
    void begin();
    void check(); 
    void buttonPressed(int i);
    void buttonReleased(int i);
    void buttonLongPressed(int i); 

};
