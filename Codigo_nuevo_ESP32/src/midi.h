/*
=================================================================
  Modulo que maneja la comunicacion MIDI
=================================================================
*/

#ifndef MIDI_H
#define MIDI_H

#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
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

// =================================================================
// 6. TAREA DEL CORE 0: LÓGICA MIDI Y PC

// void midiTask(void *parameter) {
//   Serial.println("Task de MIDI iniciada en Core 0.");
//   SensorData receivedData;

//   // Bucle infinito de la tarea
//   for (;;) {
    
//     // --- 1. REVISAR SI LLEGÓ MIDI DESDE EL PC ---
//     // ¡OBLIGATORIO! Esto revisa el USB y dispara los
//     // callbacks (receiveControlChange, receiveSysEx)
//     MidiUsb.read();

//     // --- 2. REVISAR SI LLEGARON DATOS DE SENSORES ---
//     // Revisa la cola. Si hay datos nuevos del Core 1, procésalos.
//     // Espera 0 ticks (no bloquea), solo revisa si hay algo.
//     if (xQueueReceive(sensorDataQueue, &receivedData, (TickType_t)0) == pdPASS) {
      
//       // ¡Llegaron datos! Pásalos a los handlers
//       // (Esta es la lógica de tu antigua Teensy)
      
//       // El dataHandler podría tomar los blobs y pasarlos al pitchHandler
//       //dataHandler.processSensorData(receivedData); 
      
//       // El midiHandler toma los datos y los convierte/envía por USB
//       //midiHandler.sendMidiFromSensorData(receivedData);

//       // (Las funciones de arriba son ejemplos, usa las tuyas)
//     }

//     // --- 3. REVISAR OTROS EVENTOS (BOTONES, ETC) ---
//     // (Esta es la lógica que estaba en el loop() de tu Teensy)
//     //buttons.update(); // Revisa si se presionó un botón
//     //pitchHandler.update(); // Actualiza el estado del pitch
    
//     // Da un respiro de 1ms al scheduler para no saturar el Core 0
//     vTaskDelay(pdMS_TO_TICKS(1)); 
//   }
// }