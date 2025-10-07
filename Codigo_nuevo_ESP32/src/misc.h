#ifndef MISC_H
#define MISC_H

/*
=================================================================
  Modulo que incluye macros y constantes utiles en el programa
=================================================================
*/

#include <Arduino.h>

//=====================================
//PINS



//=====================================
//LOCKS



//=====================================
//MACROS / PARAMETERS / AUX FUNCTIONS
#define rep(i, n) for(int i=0; i<n; i++)
#define MAX_ARDUINO_TIME 3294967295

unsigned long get_time(){
    return (millis()%MAX_ARDUINO_TIME);
}

#define DATA_CORE 0
#define MIDI_CORE 1


//=====================================


#endif