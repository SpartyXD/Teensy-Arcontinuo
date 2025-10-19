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
#define RX_PIN 16   // Pin donde llega el TX1 del Mega
#define TX_PIN 17   
#define FRAME_START 0xFF


//=====================================
//CONSTS
#define RX_COUNT 11
#define TX_COUNT 53


//=====================================
//LOCKS



//=====================================
//MACROS / PARAMETERS / AUX FUNCTIONS
#define rep(i, n) for(int i=0; i<n; i++)
#define MAX_ARDUINO_TIME 3294967295

unsigned long get_time(){
    return (millis()%MAX_ARDUINO_TIME);
}

#define DATA_CORE 1
#define MIDI_CORE 0


//=====================================


#endif