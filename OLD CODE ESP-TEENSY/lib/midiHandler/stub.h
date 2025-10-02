#pragma once
#include <Arduino.h>

class USBMIDIStub {
public:
    void sendNoteOn(byte note, byte velocity, byte channel) {}
    void sendNoteOff(byte note, byte velocity, byte channel) {}
    void sendAfterTouchPoly(byte note, byte pressure, byte channel) {}
    void sendControlChange(byte control, byte value, byte channel) {}
    void sendProgramChange(byte program, byte channel) {}
    void sendAfterTouch(byte pressure, byte channel) {}
    void sendPitchBend(int value, byte channel) {}
    void sendSysEx(unsigned int length, const byte* array, bool hasBeginEnd) {}
};

extern USBMIDIStub usbMIDI;
