# 📚 Resumen de Librerías - Proyecto Arcontinuo

**Autor:** Rodrigo F. Cadiz  
**Fecha de última actualización:** Mar-05-2018  
**Análisis realizado:** Septiembre 2025

---

## 🎯 **Visión General del Proyecto**

El **Arcontinuo** es un controlador MIDI gestual que combina:
- **Hardware:** Teensy 3.1 + MPU6050 + 4 botones + 2 LEDs RGB + FPGA externa
- **Software:** Arquitectura modular con 8 librerías personalizadas
- **Funcionalidad:** Control musical mediante movimientos, tacto y botones

---

## 🏗️ **Arquitectura del Sistema**

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Hardware      │────│   Librerías     │────│   Salida MIDI   │
│                 │    │   Personalizadas│    │                 │
│ • Gyro MPU6050  │    │ • buttons       │    │ • USB MIDI      │
│ • 4 Botones     │────│ • gyro          │────│ • Control Change│
│ • 2 LEDs RGB    │    │ • logo          │    │ • Note On/Off   │
│ • FPGA          │    │ • midiHandler   │    │ • Pitch Bend    │
│                 │    │ • settings      │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

---

## 📦 **Librerías Personalizadas**

### 🔘 **1. buttons** - Control de Interfaz Física
**Archivo:** `lib/buttons/`

#### **Funcionalidad:**
- ✅ Manejo de 4 botones físicos (pines 7, 8, 9, 10)
- ✅ Detección de presión corta y larga
- ✅ Anti-rebote (debounce) de 100ms
- ✅ Envío automático de mensajes MIDI
- ✅ Configuración mediante punteros a Settings y PitchHandler

#### **Características Técnicas:**
```cpp
const int btnPins[4] = {7,8,9,10};  // Pines físicos
unsigned long time_pressed[4];      // Control de timing
byte state[4] = {HIGH};            // Estados actuales
```

#### **Mejoras Propuestas:**
- 🔧 **Configurabilidad:** Hacer los pines configurables desde Settings
- 🔧 **Múltiples acciones:** Implementar doble-click y combinaciones
- 🔧 **Feedback táctil:** Añadir soporte para botones con LED integrado
- 🔧 **Calibración automática:** Auto-detección de tiempo de presión largo

---

### 🌐 **2. gyro** - Sensor de Movimiento
**Archivo:** `lib/gyro/`

#### **Funcionalidad:**
- ✅ Lectura de giroscopio MPU6050 a 15Hz
- ✅ Filtrado de señal con promedio móvil (30 muestras)
- ✅ Mapeo a valores MIDI (0-127)
- ✅ Conversión a coordenadas polares para control gestual
- ✅ Función "whammy" para efectos de guitarra

#### **Características Técnicas:**
```cpp
MPU6050 mpu;                       // Objeto del sensor
float gyroData[10];               // Buffer de datos
float filterX[30], filterY[30];   // Filtros de promedio móvil
```

#### **Mejoras Propuestas:**
- 🚀 **Filtros avanzados:** Implementar Kalman Filter para mejor precisión
- 🚀 **Calibración dinámica:** Auto-calibración al inicio y durante uso
- 🚀 **Detección de gestos:** Reconocimiento de patrones de movimiento
- 🚀 **Configuración de sensibilidad:** Ajustes por usuario
- 🚀 **Múltiples ejes:** Utilizar también el acelerómetro

---

### 🎨 **3. logo** - Sistema de Iluminación
**Archivo:** `lib/logo/`

#### **Funcionalidad:**
- ✅ Control de 2 LEDs RGB NeoPixel (pin 5)
- ✅ Efectos de parpadeo programables
- ✅ Control individual de LED superior/inferior
- ✅ Sincronización con estados del sistema

#### **Características Técnicas:**
```cpp
Adafruit_NeoPixel pixels(2, 5, NEO_GRB + NEO_KHZ800);
static const byte NUM_PIXELS = 2;
```

#### **Mejoras Propuestas:**
- 🌈 **Efectos avanzados:** Gradientes, respiración, arcoíris
- 🌈 **Respuesta musical:** LEDs que reaccionen a la música
- 🌈 **Configuración de brillo:** Control automático según ambiente
- 🌈 **Patrones personalizados:** Efectos definibles por el usuario
- 🌈 **Ahorro de energía:** Modo de bajo consumo

---

### 🎵 **4. midiHandler** - Comunicación Musical
**Archivo:** `lib/midiHandler/`

#### **Funcionalidad:**
- ✅ Gestión completa de protocolo MIDI USB
- ✅ Mapeo de controles específicos del Arcontinuo
- ✅ Manejo de múltiples canales MIDI
- ✅ Sistema de "blob" para control multi-touch
- ✅ Integración con gyro y botones

#### **Características Técnicas:**
```cpp
// Controles principales
static const byte TX_GYRO_X = 113;
static const byte TX_GYRO_Y = 114;
static const byte TX_BUTTON_BASE = 104;
static const byte MAIN_MIDI_CHANNEL = 1;
```

#### **Mejoras Propuestas:**
- 🎶 **MIDI 2.0:** Soporte para el nuevo estándar MIDI 2.0
- 🎶 **MPE extendido:** Mayor integración con MIDI Polyphonic Expression
- 🎶 **Latencia reducida:** Optimización para tiempo real
- 🎶 **Configuración dinámica:** Mapeo de controles modificable
- 🎶 **Sistema de templates:** Plantillas para diferentes DAWs

---

### ⚙️ **5. settings** - Configuración del Sistema
**Archivo:** `lib/settings/`

#### **Funcionalidad:**
- ✅ Almacenamiento persistente en EEPROM
- ✅ Gestión de presets (configuraciones guardadas)
- ✅ Parámetros de calibración de sensores
- ✅ Modos de rendimiento (slide, discrete, discrete-slide)
- ✅ Configuración de umbrales y rangos

#### **Características Técnicas:**
```cpp
// Parámetros principales
int thr = 20;                    // Umbral de detección
int percentageTop = 90;          // Zona superior
int percentageBottom = 10;       // Zona inferior
byte performanceMode = MODE_SLIDE;
```

#### **Mejoras Propuestas:**
- 💾 **Backup en la nube:** Sincronización de configuraciones
- 💾 **Interfaz gráfica:** Editor visual de parámetros
- 💾 **Validación de datos:** Verificación de integridad EEPROM
- 💾 **Configuración por contexto:** Diferentes settings por aplicación
- 💾 **Exportar/Importar:** Compartir configuraciones entre dispositivos

---

### 🎼 **6. pitchHandler** - Mapeo Musical
**Archivo:** `lib/pitchHandler/`

#### **Funcionalidad:**
- ✅ Grilla musical configurable (52x11)
- ✅ Múltiples escalas musicales (blues, tibetana, etc.)
- ✅ Sistema de transposición por columnas
- ✅ Mapeo continuo y discreto
- ✅ Compatibilidad con MPE (MIDI Polyphonic Expression)

#### **Características Técnicas:**
```cpp
static const byte MAX_ROWS = 52;
static const byte MAX_COLS = 11;
byte pitches[MAX_COLS][MAX_ROWS];
byte transpositions[MAX_COLS];
```

#### **Mejoras Propuestas:**
- 🎵 **Escalas personalizadas:** Editor de escalas musicales
- 🎵 **Microtonalidad:** Soporte para temperamentos alternativos
- 🎵 **Armonización automática:** Sugerencias de acordes
- 🎵 **Análisis musical:** Detección de tonalidad
- 🎵 **Escalas dinámicas:** Cambio de escala en tiempo real

---

### 📡 **7. dataHandler** - Comunicación Serie
**Archivo:** `lib/dataHandler/`

#### **Funcionalidad:**
- ✅ Comunicación con FPGA externa a 300000 baud
- ✅ Procesamiento de hasta 10 "blobs" simultáneos
- ✅ Buffer de datos optimizado (100 bytes)
- ✅ Sistema de tracking de objetos
- ✅ Conversión de datos crudos a eventos MIDI

#### **Características Técnicas:**
```cpp
static const byte MAX_BLOBS = 10;
static const byte DATA_SIZE = 100;
static const byte BLOB_SIZE = 9;
float velThreshold = 0.005;
```

#### **Mejoras Propuestas:**
- 📊 **Protocolo mejorado:** Compresión de datos y checksums
- 📊 **Análisis de latencia:** Optimización del pipeline de datos
- 📊 **Predicción de movimiento:** Interpolación para suavidad
- 📊 **Filtros adaptativos:** Eliminación inteligente de ruido
- 📊 **Logging de datos:** Grabación para análisis posterior

---

### 🛠️ **8. utils** - Utilidades Comunes
**Archivo:** `lib/utils/`

#### **Funcionalidad:**
- ✅ Funciones de debug y logging
- ✅ Mapeo y cuantización de valores
- ✅ Conversiones matemáticas comunes
- ✅ Manejo de errores básico

#### **Características Técnicas:**
```cpp
void dispInt(const char *text, int value);
int quantize(float value, int low, int high);
int pitchBendMap(float val, float range);
```

#### **Mejoras Propuestas:**
- 🔧 **Sistema de logging avanzado:** Niveles de debug configurables
- 🔧 **Matemáticas mejoradas:** Funciones de interpolación y suavizado
- 🔧 **Gestión de memoria:** Monitoreo de uso de RAM
- 🔧 **Profiling:** Medición de rendimiento de funciones
- 🔧 **Unit testing:** Framework de pruebas automatizadas

---

## 📚 **Librerías Externas**

### **Adafruit_MPU6050**
- **Propósito:** Driver oficial para sensor MPU6050
- **Versión:** Master branch
- **Funcionalidad:** Lectura de giroscopio/acelerómetro, calibración, filtros

### **Adafruit_BusIO**
- **Propósito:** Abstracción de comunicación I2C/SPI
- **Funcionalidad:** Interfaz unificada para sensores Adafruit

### **Adafruit_Unified_Sensor**
- **Propósito:** Framework común para sensores
- **Versión:** 1.1.15
- **Funcionalidad:** API estandarizada para diferentes tipos de sensores

---

## 🚀 **Mejoras Arquitectónicas Generales**

### **1. Modernización del Código**
- 🔄 **C++11/14:** Uso de características modernas del lenguaje
- 🔄 **Smart pointers:** Gestión automática de memoria
- 🔄 **Templates:** Código más genérico y reutilizable
- 🔄 **Namespaces:** Mejor organización del código

### **2. Gestión de Dependencias**
- 📦 **PlatformIO Library Manager:** Migrar a librerías oficiales
- 📦 **Versionado semántico:** Control de versiones de dependencias
- 📦 **CI/CD:** Integración y despliegue continuo

### **3. Documentación y Testing**
- 📝 **Doxygen:** Documentación automática del código
- 🧪 **Unit tests:** Pruebas automatizadas para cada módulo
- 🧪 **Integration tests:** Pruebas del sistema completo
- 📋 **Ejemplos:** Código de ejemplo para cada librería

### **4. Performance y Optimización**
- ⚡ **Profiling:** Identificación de cuellos de botella
- ⚡ **Optimización de memoria:** Uso eficiente de RAM limitada
- ⚡ **Timing crítico:** Garantías de tiempo real
- ⚡ **Poder de procesamiento:** Aprovechamiento del Cortex-M4

### **5. Extensibilidad**
- 🔌 **Plugin system:** Arquitectura de plugins para nuevas características
- 🔌 **API externa:** Interfaz para aplicaciones externas
- 🔌 **Hardware abstraction:** Soporte para diferentes plataformas
- 🔌 **Modularidad:** Componentes intercambiables

---

## 🎯 **Roadmap de Desarrollo**

### **Fase 1: Estabilización (1-2 meses)**
- Refactoring de código legacy
- Implementación de unit tests
- Mejora de documentación
- Optimización de rendimiento básica

### **Fase 2: Funcionalidades (2-3 meses)**
- Nuevos efectos visuales para LEDs
- Escalas musicales adicionales
- Mejores filtros para giroscopio
- Sistema de configuración mejorado

### **Fase 3: Modernización (3-4 meses)**
- Migración a C++ moderno
- Sistema de plugins
- API externa
- Integración con software musical

### **Fase 4: Innovación (4-6 meses)**
- Machine learning para gestos
- Soporte MIDI 2.0
- Conectividad inalámbrica
- Realidad aumentada para configuración

---

## 💡 **Conclusión**

El proyecto Arcontinuo presenta una **arquitectura sólida y modular** que ha resistido bien el paso del tiempo. Las mejoras propuestas se enfocan en:

1. **Mantenibilidad:** Código más limpio y documentado
2. **Funcionalidad:** Nuevas características para músicos
3. **Rendimiento:** Mejor uso de recursos del hardware
4. **Extensibilidad:** Facilidad para añadir nuevas funciones

El potencial de evolución es **enorme**, especialmente considerando los avances en sensores, procesamiento y estándares MIDI desde 2018.

---

*Documento generado automáticamente - Septiembre 2025*