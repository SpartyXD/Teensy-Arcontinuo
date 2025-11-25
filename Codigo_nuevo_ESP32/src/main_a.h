#pragma once

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>
#include "common.h" // <--- IMPORTANTE

void core_A_main(void *params){

  Serial.println("Task de MIDI iniciada en Core 0.");
  SensorData receivedData;

  // Variables para la prueba de sonido (solo para demo)
  // Nota 60 = C3 (Do central)
  int note = 60; 
  int velocity = 100;
  int channel = 1;

  // Bucle infinito de la tarea
  for (;;) {
    
    // --- 1. MANTENER LA CONEXIÓN USB ---
    MidiUsb.read();

    // --- 2. PRUEBA DE SONIDO (DEMO) ---
    // Esto enviará una nota cada 1 segundo aprox
    
    // Enviar Nota ON (Apretar tecla)
    MidiUsb.sendNoteOn(note, velocity, channel);
    
    // Esperar 500ms (sin bloquear el procesador)
    vTaskDelay(pdMS_TO_TICKS(500)); 
    
    // Enviar Nota OFF (Soltar tecla) - ¡MUY IMPORTANTE PARA NO DEJAR LA NOTA PEGADA!
    MidiUsb.sendNoteOff(note, 0, channel);
    
    // Esperar otros 500ms de silencio
    vTaskDelay(pdMS_TO_TICKS(500)); 


    // --- EL RESTO DE TU CÓDIGO ---
    // (Aquí procesarías sensorDataQueue cuando implementes los sensores reales)
    /*
    if (xQueueReceive(sensorDataQueue, &receivedData, (TickType_t)0) == pdPASS) {
       // Procesar datos reales...
    }
    */
  }
}