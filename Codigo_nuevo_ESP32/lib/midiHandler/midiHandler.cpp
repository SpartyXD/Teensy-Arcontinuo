/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#include "midiHandler.h"
#include "settings.h"

MidiHandler::MidiHandler() {
    // No inicialización especial, Teensy hace usbMIDI por defecto
}

void MidiHandler::changeProgram() {
    usbMIDI.sendProgramChange(programNumber++, MAIN_MIDI_CHANNEL);
    if (programNumber > 10) programNumber = 0;
}

void MidiHandler::setProgram(int i) {
    usbMIDI.sendProgramChange(i, MAIN_MIDI_CHANNEL);
}

void MidiHandler::receiveControlChange(byte channel, byte control, byte value, Settings * s) {
    
    Serial.print(value);
    switch(control) {

        case RX_THRESHOLD: s->sendThresholdToFPGA(value); break;
        case RX_PERCENTAGE_TOP: s->sendPercentageTopToFPGA(value); break;
        case RX_PERCENTAGE_BOTTOM: s->sendPercentageBottomToFPGA(value); break;
        case RX_MIN_AREA: s->sendMinAreaToFPGA(value); break;
        case RX_MIN_DIST: s->sendMinDistToFPGA(value); break;
        case RX_TOP_LINES: s->sendTopLinesToFPGA(value); break;
        case RX_BOTTOM_LINES: s->sendBottomLinesToFPGA(value); break;  
        case RX_TRACKER_TIME: s->sendTrackerTimeToFPGA(value); break;
        case RX_TRACKER_DIST: s->sendTrackerDistToFPGA(value); break;
        case RX_PRESET: s->setPreset(value); break;  
        case RX_SERIAL_NUM: s->setSerialNum(value); break;   
        case RX_RESET: s->rstMins(value); break; 
        case RX_WRITE_CONFIG_EEPROM: s->writeInitValuesToEEPROM(value); break;  
    }
}
