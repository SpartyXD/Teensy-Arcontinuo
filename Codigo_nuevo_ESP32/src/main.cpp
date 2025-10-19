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



//---------------------------------------------
//Function declarations


//---------------------------------------------

void setup(){
  initMIDI();
  Serial.begin(115200);
  initData(RX_PIN, TX_PIN, 115200);
}


void loop(){
  readData();
  vTaskDelay(100/portTICK_PERIOD_MS);
}
