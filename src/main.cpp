#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "buttons.h"
#include "midiHandler.h"
#include "utils.h"
#include "gyro.h"
#include "logo.h"
#include "pitchHandler.h"
#include "dataHandler.h"
#include "settings.h"
 
// Arcontinuo configuration
 
int serialNumber = 0;
int gyroEnabled = 0;
int gyroCounter = 0;

 
Settings settings;
Logo logo;
Buttons buttons;
MidiHandler midiHandler;
PitchHandler pitchHandler;
DataHandler dataHandler;
Utils utils;
Gyro gyro;  // Correctly instantiated


 
static void receiveControlChange(byte channel, byte control, byte value) {
  utils.disp("Received control change");
  midiHandler.receiveControlChange(channel,control,value,&settings);   
}
 
void setup() {
  

  usbMIDI.begin();  // Inicializa el puerto MIDI USB
  //logo.begin();
  //logo.setLight(255,0,0);
  Serial.begin(115200);
  //while(!Serial) {};
  Serial.println("inicializando Teensy");
  Serial1.begin(300000);
  
  while(!Serial1) {};
  settings.writeInitValuesToEEPROM(1);
  settings.begin();    
  //logo.blinkUp(255,0,0,0,0,255,60,125);
  usbMIDI.setHandleControlChange(receiveControlChange);
  delay(10);
  //settings.sendInitValuesToFPGA();
  delay(100);
  //settings.sendInitValuesToMidi();
  buttons.begin();
  buttons.setSettings(&settings);
  buttons.setPitchHandler(&pitchHandler);
  dataHandler.setSettings(&settings);
  dataHandler.setPitchHandler(&pitchHandler);  
  gyro.setSettings(&settings);
  //settings.setLogo(&logo);
  settings.setMidiHandler(&midiHandler);
  settings.setPitchHandler(&pitchHandler);   
  pitchHandler.begin();

  gyro.begin();

  
  delay(1000);
  //logo.blinkDown(255,255,0,0,0,0,3,100);
  settings.preset = 3;

  settings.setPreset(settings.preset);
  utils.disp("End setup");
}
 
void loop() {
  static unsigned long lastGyroUpdate = 0;
  const unsigned long gyroInterval = 1000 / 15; // ~66.67ms (15 Hz)

  // Ejecuta gyro.readGyroData() cada 66.67ms
  if (millis() - lastGyroUpdate >= gyroInterval) {
      lastGyroUpdate = millis();
      gyro.readGyroData();
      usbMIDI.sendControlChange(102,settings.preset, 1);

  }

  buttons.check();
  dataHandler.checkSerialData();
  usbMIDI.read();
}
