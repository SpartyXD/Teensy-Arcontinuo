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
//MACROS / PARAMETERS / AUX FUNCTIONS
#define rep(i, n) for(int i=0; i<n; i++)
#define MAX_ARDUINO_TIME 3294967295

unsigned long get_time(){
    return (millis()%MAX_ARDUINO_TIME);
}


//=====================================


#endif