/*
=================================================================
  Modulo que incluye el flujo de control principal del programa
=================================================================
*/

#include <Arduino.h>
#include <midi.h>
#include <data.h>
#include <misc.h>

//---------------------------------------------
//GLOBALS
String msg;

//---------------------------------------------
//Function declarations


//---------------------------------------------

void setup(){
  initMIDI();
  Serial.begin(115200);
  initData(16, 17, 115200);
}


void loop(){
  if(readData(msg))
    Serial.println("Mensaje recibido: " + msg);
  vTaskDelay(100/portTICK_PERIOD_MS);
}
