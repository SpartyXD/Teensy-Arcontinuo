/*
=================================================================
  Modulo que incluye el flujo de control principal del programa
=================================================================
*/

#include <Arduino.h>
#include <EEPROM.h>
#include "Adafruit_TinyUSB.h"
#include "MIDI.h"
#include "utils.h"

Utils utils;

Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);

// Arcontinuo configuration
int serialNumber = 0;
int gyroEnabled = 0;
int gyroCounter = 0;

void setup(){
  usb_midi.begin();
  while (!TinyUSBDevice.mounted()) delay(1);
}


void loop(){



}
