// src/common.h
#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <Adafruit_TinyUSB.h>
#include <MIDI.h>

// Estructura de recepción blobs (es una abstracción)
struct SensorData {
  float acelerometro_x;
  float giroscopo_z;
  int sensor_distancia;
  
  // --- Datos de la Malla Capacitiva (Blobs) ---
  // (Esto es un ejemplo, ajústalo a tus necesidades)
  int   blob_count;
  float blob1_x;
  float blob1_y;
  float blob1_pressure;
  float blob1_velocity_x;
  // ... (blob2, blob3, etc.)
};

// 2. Declara que estos objetos existen (EXTERN)
// Esto le dice al compilador: "No crees esto aquí, búscalo en main.cpp"
extern midi::MidiInterface<midi::SerialMIDI<Adafruit_USBD_MIDI>> MidiUsb;
extern QueueHandle_t sensorDataQueue;

#endif