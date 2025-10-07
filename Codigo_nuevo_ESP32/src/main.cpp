/*
=================================================================
  Modulo que incluye el flujo de control principal del programa
=================================================================
*/

#include <Arduino.h>
<<<<<<< Updated upstream
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
=======
#include <EEPROM.h>
#include "Adafruit_TinyUSB.h"
#include "MIDI.h"
#include "utils.h"
#include "pitchHandler.h"
#include "gyro.h"

Gyro gyro;
Utils utils;
PitchHandler pitchHandler;

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// Arcontinuo configuration
int serialNumber = 0;
int gyroEnabled = 0;
int gyroCounter = 0;

void setup(){
  MIDI.begin();
  while (!TinyUSBDevice.mounted()) delay(1);
>>>>>>> Stashed changes
}


void loop(){
  if(readData(msg))
    Serial.println("Mensaje recibido: " + msg);
  vTaskDelay(100/portTICK_PERIOD_MS);
}
