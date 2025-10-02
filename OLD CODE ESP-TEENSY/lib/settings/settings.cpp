  /** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#include <settings.h>
#include <logo.h>
#include <midiHandler.h>
#include "pitchHandler.h"

/*
      setDefaultMap();
      setTibetanMap();
      set1ColBluesMap();
      set2ColBluesMap();
      set3ColBluesMap();
      set4ColBluesMap();
      setMPEDemo();
 */

void Settings::begin() {

  readInitValuesFromEEPROM();
  //Serial1.write("ATHDMIF");
  //rstMins(1);

}

void Settings::setMidiHandler(MidiHandler* m) {
  midiHandler = m;
}

void Settings::setLogo(Logo* l) {
  logo = l;
}

void Settings::setPitchHandler(PitchHandler* p) {
  pitchHandler = p;
}

void Settings::setSerialNum(byte value) {

  serialNum = value;
  usbMIDI.sendControlChange(MidiHandler::TX_SERIAL_NUM, serialNum, MidiHandler::MAIN_MIDI_CHANNEL);  
  
}
 
 
void Settings::setPreset(byte p) {


  preset = p;
  
  switch(p) {

    case 0:
      enableGyro = 1;
      oneFinger = 1;
      midiHandler->enablePitchBend = 1;
      midiHandler->enableBrightness = 1;
      logo->setLight(0,0,255);
      pitchHandler->setContinousMap();
      break;
 
    case 1:
      //setProgram(1);
      posToLight = true;
      enableGyro = 1;
      oneFinger = 1;
      midiHandler->enablePitchBend = 1;
      midiHandler->enableBrightness = 1;
      pitchHandler->setMPEMap();
      break;

    case 2:
      posToLight = true;
      enableGyro = 0;
      midiHandler->enablePitchBend = 1;
      midiHandler->enableVibrato = 1;
      midiHandler->enableBrightness = 1;
      pitchHandler->setArpMap();
      break;


   case 3:
      posToLight = true;
      enableGyro = 0;
      midiHandler->pitchBendFactor = 1;
      midiHandler->enablePitchBend = 1;
      midiHandler->enableVibrato = 1;
      midiHandler->enableFilter = 1;
      pitchHandler->set3zonesMap();
      break;

   case 4:
      posToLight = true;
      enableGyro = 0;
      midiHandler->pitchBendFactor = 0;
      midiHandler->enablePitchBend = 1;
      midiHandler->enableVibrato = 1;
      midiHandler->enableFilter = 1;
      pitchHandler->set3ColBluesMap();
      break;

    
   case 5:
      posToLight = true;
      enableGyro = 1;
      midiHandler->pitchBendFactor = 1;
      midiHandler->enablePitchBend = 1;
      midiHandler->enableVibrato = 1;
      midiHandler->enableFilter = 1;
      break;
  }

  usbMIDI.sendControlChange(MidiHandler::TX_PRESET,preset,MidiHandler::MAIN_MIDI_CHANNEL);  
  
}


void Settings::startPreset() {

  disp("Beggining setup");
  configmode = 0;
  logo->blinkUp(255,0,0,0,0,0,3,500);
  logo->setLight(255,0,0);

}

void Settings::stopPreset() {

  disp("Ending setup");
  logo->blinkDown(255,0,0,0,0,0,3,500);
  configmode = 1;
  logo->setLight(255,255,0);

}

void Settings::presetUp() {
  preset++;
  if (preset > 127)
    preset = 127;
  logo->blinkDown(255,0,0,0,0,0,preset,250);
  setPreset(preset);
  logo->setLight(255,0,0);
}


void Settings::presetDown() {
  preset--;
  if (preset < 0)
    preset = 0;
   logo->blinkUp(255,0,0,0,0,0,preset,250); 
  setPreset(preset);
  logo->setLight(255,0,0);
}

void Settings::changePerformanceMode() {

    if (performanceMode == MODE_SLIDE) 
      performanceMode = MODE_DISCRETE;
    else if (performanceMode == MODE_DISCRETE) 
      performanceMode = MODE_DISCRETE_SLIDE;
    else if (performanceMode == MODE_DISCRETE_SLIDE) 
      performanceMode = MODE_SLIDE;

    dispInt("performanceMode ", performanceMode);
}

void Settings::sendThresholdToFPGA(byte value) {

  Serial1.write("ATU");
  Serial1.write(value);
  dispInt("Send Threshold ", value);
  //Serial1.flush(); delay(100);
}
  
void Settings::sendPercentageTopToFPGA(byte value) {

  Serial1.write("ATPERT");
  Serial1.write(value);
  dispInt("Send % top ", value);
  //Serial1.flush(); delay(100);
}

void Settings::sendPercentageBottomToFPGA(byte value) {

  Serial1.write("ATPERB");
  Serial1.write(value);
  dispInt("Send % bottom ", value);
  //Serial1.flush(); delay(100);

}

void Settings::sendMinAreaToFPGA(byte value) {

  Serial1.write("ATAREA");
  Serial1.write(value);
  dispInt("Send min area ", value);
  //Serial1.flush(); delay(100);
    
}

void Settings::sendMinDistToFPGA(byte value) {

  Serial1.write("ATDIST");
  Serial1.write(value);
  dispInt("Send min dist ", value);
  //Serial1.flush(); delay(100);
  
}

void Settings::sendTopLinesToFPGA(byte value) {

  Serial1.write("ATCU");
  Serial1.write(value);
  dispInt("Send top lines ", value);
  //Serial1.flush(); delay(100);
  
}

void Settings::sendBottomLinesToFPGA(byte value) {

  Serial1.write("ATCD");
  Serial1.write(value);
  dispInt("Send bottom lines ", value);
  //Serial1.flush(); delay(100);
  
}

void Settings::sendTrackerTimeToFPGA(byte value) {

  Serial1.write("ATTIME");
  Serial1.write(value);
  dispInt("Send tracker time ", value);
  //Serial1.flush(); delay(100);
  
}

void Settings::sendTrackerDistToFPGA(byte value) {

  Serial1.write("ATTDIST");
  Serial1.write(value);
  dispInt("Send tracker dist ", value);
  //Serial1.flush(); delay(10);
  
}

void Settings::rstMins(byte value) {

  Serial1.write("ATRST");
  dispInt("Send reset ", value);
  //Serial1.flush(); delay(10);
  
}

void Settings::sendInitValuesToFPGA () {
    
  sendThresholdToFPGA((byte) thr);
  sendPercentageTopToFPGA((byte) percentageTop);
  sendPercentageBottomToFPGA((byte) percentageBottom);
  sendMinAreaToFPGA((byte) minArea);
  sendMinDistToFPGA((byte) minDist);
  sendTopLinesToFPGA((byte) topLines);
  sendBottomLinesToFPGA((byte) bottomLines);
  sendTrackerTimeToFPGA((byte) timeTracker);
  sendTrackerDistToFPGA((byte) distTracker);

}

void Settings::readInitValuesFromEEPROM() {

  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES) != 255) thr = EEPROM.read(BASE_EEPROM_CONFIG_VALUES);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+1) != 255) percentageTop = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+1);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+2) != 255) percentageBottom = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+2);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+3) != 255) minArea = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+3);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+4) != 255) minDist = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+4);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+5) != 255) topLines = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+5);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+6) != 255) bottomLines = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+6);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+7) != 255) timeTracker = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+7);
  if (EEPROM.read(BASE_EEPROM_CONFIG_VALUES+8) != 255) distTracker = EEPROM.read(BASE_EEPROM_CONFIG_VALUES+8);

}

void Settings::writeInitValuesToEEPROM(byte value) {

  if (value == 1) {

    if (thr != EEPROM.read(BASE_EEPROM_CONFIG_VALUES)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES,thr);
    if (percentageTop != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+1)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+1,percentageTop);
    if (percentageBottom != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+2)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+2,percentageBottom);
    if (minArea != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+3)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+3,minArea);
    if (minDist != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+4)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+4,minDist);
    if (topLines != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+5)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+5,topLines);
    if (bottomLines != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+6)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+6,bottomLines);
    if (timeTracker != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+7)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+7,timeTracker);
    if (distTracker  != EEPROM.read(BASE_EEPROM_CONFIG_VALUES+8)) EEPROM.write(BASE_EEPROM_CONFIG_VALUES+8,distTracker );
    
  }
}



void Settings::sendInitValuesToMidi() {

  disp("Updating settings data to midi");

  usbMIDI.sendControlChange(MidiHandler::TX_THRESHOLD,thr,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_PERCENTAGE_TOP,percentageTop,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_PERCENTAGE_BOTTOM,percentageBottom,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_MIN_AREA,minArea,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_MIN_DIST,minDist,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_TOP_LINES,topLines,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_BOTTOM_LINES,bottomLines,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_TRACKER_TIME,timeTracker,MidiHandler::MAIN_MIDI_CHANNEL);
  usbMIDI.sendControlChange(MidiHandler::TX_TRACKER_DIST,distTracker,MidiHandler::MAIN_MIDI_CHANNEL);
}
