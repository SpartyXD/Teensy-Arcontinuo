// Standard Libs
/*
 * =================================================================
 * ARQUITECTURA RTOS PARA CONTROLADOR MIDI EN ESP32-S3
 * =================================================================
 * CORE 1 (Sensor Core):
 * - Tarea: sensorTask()
 * - Propósito: Leer malla capacitiva, giroscopio, etc.
 * - Filtrar datos y empaquetarlos en la struct 'SensorData'.
 * - Enviar 'SensorData' a la cola (Queue).
 *
 * CORE 0 (MIDI Core):
 * - Tarea: midiTask()
 * - Propósito: Reemplaza a la Teensy.
 * - Recibir 'SensorData' de la cola.
 * - Leer botones.
 * - Procesar lógica de Pitch, Settings, etc.
 * - Enviar y Recibir MIDI USB al PC.
 * - Manejar presets (recepción de JSON por SysEx).
 * =================================================================
 */

// --- LIBRERÍAS ESTÁNDAR Y USB ---
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <MIDI.h>
#include <Wire.h>
#include <EEPROM.h>


// --- LIBRERÍAS PERSONALIZADAS ---
#include "utils.h"
#include "gyro.h"
#include "stub.h"
#include "midiHandler.h"
#include "logo.h"
#include "dataHandler.h"
#include "settings.h"
#include "buttons.h"
#include "pitchHandler.h"

// =================================================================
// 1. DEFINICIÓN DE TAREAS Y COLA (RTOS)

// Cores
#define DATA_CORE 1 // Núcleo para sensores (Core 1)
#define MIDI_CORE 0 // Núcleo para USB y MIDI (Core 0)

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

// Handle de la cola (Queue) de RTOS
QueueHandle_t sensorDataQueue;


// Prototipos de las tareas
void sensorTask(void *parameter);
void midiTask(void *parameter);

// =================================================================
// 2. OBJETOS GLOBALES E INSTANCIAS MIDI

// Objetos principales
Settings settings;
Logo logo;
Buttons buttons;
MidiHandler midiHandler;
PitchHandler pitchHandler;
DataHandler dataHandler;
Utils utils;
Gyro gyro;

// Configuración MIDI USB
Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, MidiUsb);

// Tamaño EEPROM
#define EEPROM_SIZE 1024

// =================================================================
// 3. HANDLERS (CALLBACKS) DE MIDI

// Esta función se llamará automáticamente desde midiTask() cuando
// llegue un mensaje MIDI CC desde el PC.
static void receiveControlChange(byte channel, byte control, byte value) {
    utils.disp("Received control change");
    midiHandler.receiveControlChange(channel, control, value, &settings); 
}

// ¡AQUÍ MANEJAS EL JSON DE PRESETS!
// Esta función se llamará desde midiTask() cuando llegue un SysEx.
static void receiveSysEx(byte* data, unsigned int length) {
    utils.disp("SysEx recibido! (Procesando JSON de Presets...)");
    // 'data' es un puntero al array de bytes (ej: F0 ... datos ... F7)
    // 'length' es el número de bytes
    
    // Aquí iría tu lógica para:
    // 1. Validar que es un SysEx para ti.
    // 2. Parsear los bytes 'data' (que contienen el JSON)
    // 3. Actualizar el objeto 'settings'.
    // 4. Guardar en EEPROM.
}

// =================================================================
// 4. SETUP (Se ejecuta una sola vez en el Core 1)

int serialNumber = 0;
int gyroEnabled = 0;
int gyroCounter = 0;


void setup() {
  // Serial init
  Serial.begin(115200);
  Serial.println("Iniciando ESP...");

  // USB MIDI init
  TinyUSBDevice.setManufacturerDescriptor("PIO");
  TinyUSBDevice.setProductDescriptor("EspUSB");
  MidiUsb.begin(MIDI_CHANNEL_OMNI);
  MidiUsb.setHandleControlChange(receiveControlChange);
  Serial.println("Midi USB setup done");

  // 1. Inicializar componentes básicos
  // Inicializar EEPROM (¡OBLIGATORIO EN ESP32!)
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("¡Error al inicializar EEPROM!");
  }

  // 2. Cargar configuración desde EEPROM
  settings.writeInitValuesToEEPROM(1);
  settings.begin();
  Serial.println("Settings y EEPROM listos");
  
  buttons.begin();
  pitchHandler.begin();
  gyro.begin();

// 3. Conectar dependencias entre componentes
  buttons.setSettings(&settings);
  buttons.setPitchHandler(&pitchHandler);
  dataHandler.setSettings(&settings);
  dataHandler.setPitchHandler(&pitchHandler);
  gyro.setSettings(&settings);
  settings.setMidiHandler(&midiHandler);
  settings.setPitchHandler(&pitchHandler);
  Serial.println("Dependencias inyectadas");

  delay(1000);
  
  // 4. Iniciar preset inicial
  settings.preset = 3;
  settings.setPreset(settings.preset);
  utils.disp("End setup");

  // 5. Queu
  sensorDataQueue = xQueueCreate(10, sizeof(SensorData));
    if (sensorDataQueue == NULL) {
      Serial.println("¡Error al crear la cola!");
    }

  // FreeRTOS Tasks
  Serial.println("Lanzando tareas RTOS...");

// Tarea de Sensores: Core 1 (DATA_CORE), Prioridad 2 (más alta)
    xTaskCreatePinnedToCore(
        sensorTask,     // Función de la tarea
        "SensorTask",   // Nombre (para debug)
        1024 * 4,       // Tamaño del stack (4KB)
        NULL,           // Parámetros de la tarea
        2,              // Prioridad
        NULL,           // Handle de la tarea
        DATA_CORE);     // Núcleo donde se ejecuta

    // Tarea de MIDI: Core 0 (MIDI_CORE), Prioridad 1 (más baja)
    xTaskCreatePinnedToCore(
        midiTask,       // Función de la tarea
        "MidiTask",     // Nombre (para debug)
        1024 * 4,       // Tamaño del stack (4KB)
        NULL,           // Parámetros de la tarea
        1,              // Prioridad
        NULL,           // Handle de la tarea
        MIDI_CORE);     // Núcleo donde se ejecuta

  utils.disp("Fin del setup. RTOS tomando el control.");
}

// =================================================================
// 5. TAREA DEL CORE 1: LECTURA DE SENSORES

void sensorTask(void *parameter) {
  Serial.println("Task de Sensores iniciada en Core 1.");
  SensorData dataToSend;

  // Bucle infinito de la tarea
  for (;;) {
    // --- 1. LEER Y FILTRAR DATOS ---
    // (Aquí va todo el código de tu antigua ESP32)
    
    // Leer Giroscopio (usando el objeto gyro)
    //gyro.update(); // Actualiza los valores internos del giroscopio
    //dataToSend.acelerometro_x = gyro.getAcelX(); // (asumiendo estas funciones)
    //dataToSend.giroscopo_z = gyro.getGyroZ();
    
    // Leer Malla Capacitiva (tus "blobs")
    // (Aquí iría tu lógica de filtrado de la malla)
    dataToSend.blob_count = 1;
    dataToSend.blob1_x = 0.5; // (valor de ejemplo)
    dataToSend.blob1_pressure = 0.8; // (valor de ejemplo)
    // ... etc ...

    // --- 2. ENVIAR DATOS A LA COLA ---
    // Envía la estructura completa al Core 0.
    // No espera si la cola está llena (timeout 0)
    xQueueSend(sensorDataQueue, &dataToSend, (TickType_t)0);
    
    // Espera 10ms (100Hz) antes de la próxima lectura.
    // Ajusta esto a tu necesidad de "sample rate".
    vTaskDelay(pdMS_TO_TICKS(10)); 
  }
}

// =================================================================
// 6. TAREA DEL CORE 0: LÓGICA MIDI Y PC

void midiTask(void *parameter) {
  Serial.println("Task de MIDI iniciada en Core 0.");
  SensorData receivedData;

  // Bucle infinito de la tarea
  for (;;) {
    
    // --- 1. REVISAR SI LLEGÓ MIDI DESDE EL PC ---
    // ¡OBLIGATORIO! Esto revisa el USB y dispara los
    // callbacks (receiveControlChange, receiveSysEx)
    MidiUsb.read();

    // --- 2. REVISAR SI LLEGARON DATOS DE SENSORES ---
    // Revisa la cola. Si hay datos nuevos del Core 1, procésalos.
    // Espera 0 ticks (no bloquea), solo revisa si hay algo.
    if (xQueueReceive(sensorDataQueue, &receivedData, (TickType_t)0) == pdPASS) {
      
      // ¡Llegaron datos! Pásalos a los handlers
      // (Esta es la lógica de tu antigua Teensy)
      
      // El dataHandler podría tomar los blobs y pasarlos al pitchHandler
      //dataHandler.processSensorData(receivedData); 
      
      // El midiHandler toma los datos y los convierte/envía por USB
      //midiHandler.sendMidiFromSensorData(receivedData);

      // (Las funciones de arriba son ejemplos, usa las tuyas)
    }

    // --- 3. REVISAR OTROS EVENTOS (BOTONES, ETC) ---
    // (Esta es la lógica que estaba en el loop() de tu Teensy)
    //buttons.update(); // Revisa si se presionó un botón
    //pitchHandler.update(); // Actualiza el estado del pitch
    
    // Da un respiro de 1ms al scheduler para no saturar el Core 0
    vTaskDelay(pdMS_TO_TICKS(1)); 
  }
}

// =================================================================
// 7. LOOP DE ARDUINO (Vacío)

void loop() {
  // Vacío. Todo el trabajo se hace en las Tareas RTOS.
  vTaskDelay(pdMS_TO_TICKS(1000));
}



/* void loop() {
  for (int i = 0; i < 128; i++) {
    MidiUsb.sendControlChange(0, i, 1);
    delay(100);
  }
}
 */