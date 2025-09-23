/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#include "midiHandler.h"
#include "settings.h"


/* Settings ROLI Lightpad
 * Note ON-OFF: C2 with velocity proportional to Z, if Channel Pressure > 8
 * X: PitchWheel from -1902 to 1907 (Glide)
 * Y: Brightness from 63 to 127 upwards, from 63 to 0 downwards (Slide)
 * Z: Channel Pressure, from 0 to 127 (Press)
 * 
 * Settings ROLI Seaboard
 * In key, X: PitchWheel from 0 to -127 to 127, in band from 0 to 8192 or -8192 (Glide)
 * In key, Y:  Brightness from 63 to 127 upwards, from 63 to 0 downwards (Slide)
 * In key, Z: * Z: Channel Pressure, from 0 to 127 (Press)
 * 
 * Others: program on all channels, controllers 107, 109, 111, 2D (113 and 114) Data Entry (coarse) 6 and 7 (octaves)
  usbMIDI.sendNoteOn(note, velocity, channel);
  usbMIDI.sendNoteOff(note, velocity, channel);
  usbMIDI.sendAfterTouchPoly(note, pressure, channel);
  usbMIDI.sendControlChange(control, value, channel);
  usbMIDI.sendProgramChange(program, channel);
  usbMIDI.sendAfterTouch(pressure, channel);
  usbMIDI.sendPitchBend(value, channel);
  usbMIDI.sendSysEx(length, array, hasBeginEnd);
 * 
 * 
 */

MidiHandler::MidiHandler() {
  
}

void MidiHandler::changeProgram() {

  usbMIDI.sendProgramChange(programNumber++,MAIN_MIDI_CHANNEL);
  if (programNumber > 10) programNumber = 0;
    
}

void MidiHandler::setProgram(int i) {

  usbMIDI.sendProgramChange(i,MAIN_MIDI_CHANNEL);
  
}

void MidiHandler::receiveControlChange(byte channel, byte control, byte value, Settings * s) {
    
  Serial.print(value);
  switch(control) {

    case RX_THRESHOLD:
      s->sendThresholdToFPGA(value);
      break;

    case RX_PERCENTAGE_TOP:
      s->sendPercentageTopToFPGA(value);
      break;

    case RX_PERCENTAGE_BOTTOM:
      s->sendPercentageBottomToFPGA(value);
      break;

    case RX_MIN_AREA:
      s->sendMinAreaToFPGA(value);
      break;

    case RX_MIN_DIST:
      s->sendMinDistToFPGA(value);
      break;

    case RX_TOP_LINES:
      s->sendTopLinesToFPGA(value);
      break;

    case RX_BOTTOM_LINES:
      s->sendBottomLinesToFPGA(value);
      break;  

    case RX_TRACKER_TIME:
      s->sendTrackerTimeToFPGA(value);
      break;

    case RX_TRACKER_DIST:
      s->sendTrackerDistToFPGA(value);
      break;

    case RX_PRESET:
      s->setPreset(value);
      break;  

    case RX_SERIAL_NUM:
      s->setSerialNum(value);
      break;   

    case RX_RESET:
      s->rstMins(value);
      break; 

    case RX_WRITE_CONFIG_EEPROM:
      s->writeInitValuesToEEPROM(value);
      break;  
   
  }
    
    
}
