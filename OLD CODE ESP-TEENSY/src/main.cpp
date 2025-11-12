#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include "buttons.h"
#include "stub.h"
#include "midiHandler.h"
#include "utils.h"
#include "gyro.h"
#include "logo.h"
#include "dataHandler.h"
#include "settings.h"

// Cambiamos esta librería 
#include <MIDI.h>

#include <Adafruit_TinyUSB.h>
Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MIDI);


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
Gyro gyro;

// Crear instancia de MIDI UART usando Serial1
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI);

static void receiveControlChange(byte channel, byte control, byte value) {
    utils.disp("Received control change");
    midiHandler.receiveControlChange(channel, control, value, &settings);   
}

void setup() {
    Serial.begin(115200);
    Serial.println("inicializando ESP32");

    Serial1.begin(31250); // Puerto MIDI UART
    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI.setHandleControlChange(receiveControlChange);

    settings.writeInitValuesToEEPROM(1);
    settings.begin();

    buttons.begin();
    buttons.setSettings(&settings);
    buttons.setPitchHandler(&pitchHandler);

    dataHandler.setSettings(&settings);
    dataHandler.setPitchHandler(&pitchHandler);  

    gyro.setSettings(&settings);
    settings.setMidiHandler(&midiHandler);
    settings.setPitchHandler(&pitchHandler);   

    pitchHandler.begin();
    gyro.begin();

    delay(1000);
    settings.preset = 3;
    settings.setPreset(settings.preset);

    utils.disp("End setup");
}

void loop() {
    static unsigned long lastGyroUpdate = 0;
    const unsigned long gyroInterval = 1000 / 15; // ~66.67ms (15 Hz)

    if (millis() - lastGyroUpdate >= gyroInterval) {
        lastGyroUpdate = millis();
        gyro.readGyroData();
        // Enviar MIDI control change por UART
        MIDI.sendControlChange(102, settings.preset, 1);
    }

    buttons.check();
    dataHandler.checkSerialData();

    // Leer mensajes MIDI entrantes
    MIDI.read();
}
