/*
=================================================================
  Modulo que maneja la comunicacion MIDI
=================================================================
*/

#ifndef MIDI_H
#define MIDI_H

#include <Adafruit_TinyUSB.h>
#include <misc.h>

//---------------------------------------------
// Interfaces USB (CDC + MIDI)
Adafruit_USBD_CDC SerialUSB;
Adafruit_USBD_MIDI usb_midi;

//---------------------------------------------
//Function declarations
void initMIDI();
void handleCommands(void *pvParameters);

//MIDI_commands
void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel);
void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel);
void midiControlChange(uint8_t control, uint8_t value, uint8_t channel);
void midiPitchBend(int value, uint8_t channel);
void midiAfterTouch(uint8_t pressure, uint8_t channel);

//---------------------------------------------

void initMIDI(){
    TinyUSBDevice.setManufacturerDescriptor("Acme Industries");
    TinyUSBDevice.setProductDescriptor("ESP32 S3-MIDI");
    
    TinyUSBDevice.begin();
    SerialUSB.begin(115200);
    usb_midi.begin();

    //Waitear hasta que este listo
    while (!TinyUSBDevice.mounted()) {
      delay(500);
    }

    xTaskCreatePinnedToCore(
    handleCommands,
    "MIDI Command Handler",
    4096,       // stack sz
    NULL,       // parameters
    1,          // priority
    NULL,       // handler
    MIDI_CORE           // core
    );
    
    delay(1000);
}


void handleCommands(void *pvParameters){
  while(true){
    sendNoteOn(60, 10, 3);
    Serial.println("Enviando datos random midi...");
    vTaskDelay(500/portTICK_PERIOD_MS);
    sendNoteOff(60, 10, 3);
  }
}


//-----------------------------------------------

void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel) {
  uint8_t msg[3] = { uint8_t(0x90 | ((channel - 1) & 0x0F)), note, velocity };
  usb_midi.write(msg, 3);
}

void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel) {
  uint8_t msg[3] = { uint8_t(0x80 | ((channel - 1) & 0x0F)), note, velocity };
  usb_midi.write(msg, 3);
}


void midiControlChange(uint8_t control, uint8_t value, uint8_t channel) {
  uint8_t msg[3] = { uint8_t(0xB0 | ((channel - 1) & 0x0F)), control, value };
  usb_midi.write(msg, 3);
}

//Pitch Bend (valor de 14 bits)
void midiPitchBend(int value, uint8_t channel) {
  value = constrain(value, -8192, 8191);
  uint16_t bend = value + 8192;
  uint8_t msg[3] = {
    uint8_t(0xE0 | ((channel - 1) & 0x0F)),
    uint8_t(bend & 0x7F),
    uint8_t((bend >> 7) & 0x7F)
  };
  usb_midi.write(msg, 3);
}

//Channel Pressure (aftertouch)
void midiAfterTouch(uint8_t pressure, uint8_t channel) {
  uint8_t msg[2] = { uint8_t(0xD0 | ((channel - 1) & 0x0F)), pressure };
  usb_midi.write(msg, 2);
}


#endif