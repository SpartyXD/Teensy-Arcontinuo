/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#include "dataHandler.h"
#include "settings.h"
#include "midiHandler.h"
#include <vector>
#include <algorithm>  // Para std::remove
#include "pitchHandler.h"

/*
if (Serial1.available()) {
    incomingByte = Serial1.read();  // will not be -1
    // actually do something with incomingByte
  }
*/

void DataHandler::checkSerialData() {

  // It seems this is not necessary at all!!!! Check if it solves init problem...
  /*
  while (Serial.available()){       // Stuff coming from USB
    Serial1.write(Serial.read());   // Send it to FPGA
  }*/
  
  while (Serial1.available()) {     // FPGA outputs something

    int data = Serial1.read();    
    
    if(data==255){
      
      ind = 0;
      //Serial.println();
      }
    
    else{
      //Serial.print(data);
      //Serial.print("-");  
      inputData[ind]=data;
      ind++;
      
      if(ind==DATA_SIZE-1){ 
        
        processData(); 
      }
    }
  }
}


void DataHandler::processData() {

  //printInputData();
  
  //updateSettingsData(); //send new config values by MIDI

  if (settings->numBlobs < 10) {

    readBlobData();
    //fixFirstBlob(); 
    //outputMidiData(); 
    copyLastBlobData();
    //printBlobData();
  }
  else { 
    dispInt("Wrong blob number ", settings->numBlobs);
  }
}


/*
void DataHandler::processData() {
  // Verificar si todos los valores de inputData[10] a inputData[19] son cero
  bool hasActiveBlob = false;
  
  for (int i = 10; i < 20; i++) {
      if (inputData[i] != 0) {  
          hasActiveBlob = true;
          break;
      }
  }

  // Solo procesar si hay al menos un blob detectado
  if (hasActiveBlob) {
      readBlobData();
      copyLastBlobData();
  }
}*/



void DataHandler::updateSettingsData() {
  
  dispInt("thr ", settings->thr);
  if (settings->thr != inputData[0]) {
    settings->thr = inputData[0];   
    dispInt("thr ", settings->thr);
    usbMIDI.sendControlChange(MidiHandler::TX_THRESHOLD,settings->thr,MidiHandler::MAIN_MIDI_CHANNEL);
  }

  if (settings->percentageTop != inputData[1]) {
    settings->percentageTop = inputData[1];
    dispInt("ptop ", settings->percentageTop);
    usbMIDI.sendControlChange(MidiHandler::TX_PERCENTAGE_TOP,settings->percentageTop,MidiHandler::MAIN_MIDI_CHANNEL);
  }

  if (settings->percentageBottom != inputData[2]) {
    settings->percentageBottom = inputData[2];
    dispInt("pbot ", settings->percentageBottom);
    usbMIDI.sendControlChange(MidiHandler::TX_PERCENTAGE_BOTTOM,settings->percentageBottom,MidiHandler::MAIN_MIDI_CHANNEL);
  }

  if (settings->minArea != inputData[3]) {
    settings->minArea = inputData[3];
    dispInt("minArea ", settings->minArea);
    usbMIDI.sendControlChange(MidiHandler::TX_MIN_AREA,settings->minArea,MidiHandler::MAIN_MIDI_CHANNEL);
  }

  if (settings->minDist != inputData[4]) {
    settings->minDist = inputData[4];
    dispInt("minDist ", settings->minDist);
    usbMIDI.sendControlChange(MidiHandler::TX_MIN_DIST,settings->minDist,MidiHandler::MAIN_MIDI_CHANNEL);
  }

  if (settings->topLines != inputData[5]) {
    settings->topLines = inputData[5];
    dispInt("toplines ", settings->topLines);
    usbMIDI.sendControlChange(MidiHandler::TX_TOP_LINES,settings->topLines,MidiHandler::MAIN_MIDI_CHANNEL);
  }

  if (settings->bottomLines != inputData[6]) {
    settings->bottomLines = inputData[6];
    dispInt("botlines ", settings->bottomLines);
    usbMIDI.sendControlChange(MidiHandler::TX_BOTTOM_LINES,settings->bottomLines,MidiHandler::MAIN_MIDI_CHANNEL);
  }
  
  if (settings->timeTracker != inputData[7]) {
    settings->timeTracker = inputData[7];
    dispInt("ttr ", settings->timeTracker);
    usbMIDI.sendControlChange(MidiHandler::TX_TRACKER_TIME,settings->timeTracker,MidiHandler::MAIN_MIDI_CHANNEL);
  }

if (settings->distTracker != inputData[8]) {
    settings->distTracker = inputData[8];
    dispInt("dtr ", settings->distTracker);
    usbMIDI.sendControlChange(MidiHandler::TX_TRACKER_DIST,settings->distTracker,MidiHandler::MAIN_MIDI_CHANNEL);
  }

  if (settings->numBlobs != inputData[9]) {
    settings->numBlobs = inputData[9];
    //dispInt("numBlobs ", settings->numBlobs);
    usbMIDI.sendControlChange(MidiHandler::TX_NUM_BLOBS,settings->numBlobs,MidiHandler::MAIN_MIDI_CHANNEL);
  }
  usbMIDI.send_now();   

}


#include <map>
#include <vector>
#include <algorithm>  // Para std::remove

struct BlobState {
    bool active;
    bool released;
    int pitch;
    int channel;
    float lastX;
    float lastY;
    float lastZ;
};


std::vector<int> activeBlobs;
std::map<int, BlobState> blobMemory;
bool allNotesOffSent = false;  
int noBlobCycles = 0;  // 🔥 Contador de ciclos sin blobs activos

void DataHandler::readBlobData() {
  bool hasActiveBlobs = false; 
  float dx = 0, dy = 0, dz = 0;
  static float prevPosX[MAX_BLOBS] = {0}, prevPosY[MAX_BLOBS] = {0}, prevPosZ[MAX_BLOBS] = {0};
  static const float sampleRate = 1.0f; // 100 Hz
  static const unsigned long updateInterval = 50; // Calcular derivada cada 50 ms
  static unsigned long lastUpdateTime = 0;
  unsigned long currentTime = millis();
  bool updateDerivatives = (currentTime - lastUpdateTime) >= updateInterval;

  if (updateDerivatives) lastUpdateTime = currentTime;
  

    
  for (int i = 0; i < MAX_BLOBS; i++) {
      int id = (int)inputData[i * BLOB_SIZE + DATA_HEAD];  
      blobData[i * 9 + 0] = (float)id;

      if (id > 0) {  // Blob detectado
          float posy_int = (float)inputData[i * BLOB_SIZE + DATA_HEAD + 1] / 9.0f;
          float posx_int = (float)inputData[i * BLOB_SIZE + DATA_HEAD + 3] / 44.0f;
          float z = (float)inputData[i * BLOB_SIZE + DATA_HEAD + 5] / 10.0f;

          blobData[i * 9 + 1] = constrain(posy_int, 0.0f, 1.0f);
          blobData[i * 9 + 3] = constrain(posx_int, 0.0f, 1.0f);
          blobData[i * 9 + 5] = constrain(z, 0.0f, 1.0f);

          // Debug actualizado (opcional)
          /*
          Serial.print("DEBUG: ");
          Serial.print("[ID:"); Serial.print(inputData[i * BLOB_SIZE + DATA_HEAD] );
          Serial.print(" X:"); Serial.print(blobData[i * 9 + 1]);
          Serial.print(" Y:"); Serial.print(blobData[i * 9 + 3]);
          Serial.print(" Z:"); Serial.print(blobData[i * 9 + 5]);
          Serial.print("]  ");
          Serial.println();
          */

          if (updateDerivatives) {
              dx = (posx_int - prevPosX[i]) / (updateInterval / 1000.0f);
              dy = (posy_int - prevPosY[i]) / (updateInterval / 1000.0f);
              dz = (z - prevPosZ[i]) / (updateInterval / 1000.0f);

              blobData[i * 9 + 7] = dx;
              blobData[i * 9 + 8] = dy;
              blobData[i * 9 + 9] = dz;

              prevPosX[i] = posx_int;
              prevPosY[i] = posy_int;
              prevPosZ[i] = z;
          }
          hasActiveBlobs = true;
      } else {
          // Si el blob desaparece, asegurarse de poner `z` en 0
          blobData[i * 9 + 5] = 0.0f;
      }
  }
  checkBlobStatus(hasActiveBlobs);
}


/*void DataHandler::readBlobData() {
    bool hasActiveBlobs = false; 
    float dx = 0;
    float dy = 0;
    float dz = 0;
    static float prevPosX[MAX_BLOBS] = {0};
    static float prevPosY[MAX_BLOBS] = {0};
    static float prevPosZ[MAX_BLOBS] = {0};
    static const float sampleRate = 1.0f; // 100 Hz
    static const float dt = 1.0f / sampleRate;
    static const int FIR_ORDER = 10;
    static float firCoeff[FIR_ORDER] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
    static float filterX[MAX_BLOBS][FIR_ORDER] = {{0}};
    static float filterY[MAX_BLOBS][FIR_ORDER] = {{0}};
    static float filterZ[MAX_BLOBS][FIR_ORDER] = {{0}};
    static unsigned long lastUpdateTime = 0;
    static const unsigned long updateInterval = 50; // Calcular derivada cada 50 ms

    unsigned long currentTime = millis(); // Obtener tiempo actual en milisegundos
    bool updateDerivatives = (currentTime - lastUpdateTime) >= updateInterval;
    if (updateDerivatives) {
        lastUpdateTime = currentTime;
    }

    for (int i = 0; i < MAX_BLOBS; i++) {
        int id = (int)inputData[i * BLOB_SIZE + DATA_HEAD];  
        blobData[i * 9 + 0] = (float)id;

        float posy_int = (float)inputData[i * BLOB_SIZE + DATA_HEAD + 1] / 9.0f;
        float posx_int = (float)inputData[i * BLOB_SIZE + DATA_HEAD + 3] / 44.0f;
        float z = (float)inputData[i * BLOB_SIZE + DATA_HEAD + 5] / 10.0f;

        // Aplicar filtro FIR
        for (int j = 1; j < FIR_ORDER; j++) {
            filterX[i][j - 1] = filterX[i][j];
            filterY[i][j - 1] = filterY[i][j];
            filterZ[i][j - 1] = filterZ[i][j];
        }
        filterX[i][FIR_ORDER - 1] = posx_int;
        filterY[i][FIR_ORDER - 1] = posy_int;
        filterZ[i][FIR_ORDER - 1] = z;

        float smoothedX = 0, smoothedY = 0, smoothedZ = 0;
        for (int j = 0; j < FIR_ORDER; j++) {
            smoothedX += firCoeff[j] * filterX[i][j];
            smoothedY += firCoeff[j] * filterY[i][j];
            smoothedZ += firCoeff[j] * filterZ[i][j];
        }

        blobData[i * 9 + 1] = constrain(smoothedY, 0.0f, 1.0f);
        blobData[i * 9 + 3] = constrain(smoothedX, 0.0f, 1.0f);
        blobData[i * 9 + 5] = constrain(smoothedZ, 0.0f, 1.0f);

        // Calcular la derivada solo si ha pasado suficiente tiempo
        if (updateDerivatives) {
            dx = (smoothedX - prevPosX[i]) / (updateInterval / 1000.0f);
            dy = (smoothedY - prevPosY[i]) / (updateInterval / 1000.0f);
            dz = (smoothedZ - prevPosZ[i]) / (updateInterval / 1000.0f);

            blobData[i * 9 + 7] = dx;
            blobData[i * 9 + 8] = dy;
            blobData[i * 9 + 9] = dz;

            // Actualizar valores previos
            prevPosX[i] = smoothedX;
            prevPosY[i] = smoothedY;
            prevPosZ[i] = smoothedZ;
        }

        if (id > 0 && smoothedZ > 0.05) { 
            hasActiveBlobs = true;
        }
    }
    checkBlobStatus(hasActiveBlobs);
}*/

/*void DataHandler::checkBlobStatus(bool hasActiveBlobs) {
  const float PRESSURE_THRESHOLD = 0.1;
  const float POSITION_THRESHOLD = 0.01;
  const float MAX_D = 1.0; // Rango asumido para dx, dy, dz
  static int lastActiveBlobCount = -1; // 🚨 Última cantidad de blobs activos registrados

  int activeBlobCount = 0; // Contador de blobs activos en este ciclo

  if (!hasActiveBlobs) {
      noBlobCycles++;  
      if (noBlobCycles >= 2 && !allNotesOffSent) {
          for (int channel = 1; channel <= 16; channel++) {
              usbMIDI.sendControlChange(123, 0, channel);
          }

          for (auto& [blobId, state] : blobMemory) {
              if (state.active) {
                  usbMIDI.sendNoteOff(state.pitch, 0, state.channel);
                  state.active = false;
                  state.released = true;
              }
          }
          activeBlobs.clear();
          blobMemory.clear();
          allNotesOffSent = true;  
      }

      activeBlobCount = 0; // No hay blobs activos
  } else {
      noBlobCycles = 0;
      allNotesOffSent = false; 

      for (int i = 0; i < MAX_BLOBS; i++) {
          int id = (int)blobData[i * 9 + 0];  
          float x = blobData[i * 9 + 1];
          float y = blobData[i * 9 + 3];
          float z = blobData[i * 9 + 5]; 
          float dx = blobData[i * 9 + 7];
          float dy = blobData[i * 9 + 8];
          float dz = blobData[i * 9 + 9];

          if (id <= 0) continue;  

          int mapped_dx = constrain((dx / (2 * MAX_D)) * 127 + 64, 0, 127);
          int mapped_dy = constrain((dy / (2 * MAX_D)) * 127 + 64, 0, 127);
          int mapped_dz = constrain((dz / (2 * MAX_D)) * 127 + 64, 0, 127);

          int pitch = pitchHandler->getPitch(x, y);
          
          Serial.println(z);
          
          if (blobMemory.find(id) != blobMemory.end() && blobMemory[id].active && z < PRESSURE_THRESHOLD) {

              
              usbMIDI.sendNoteOff(blobMemory[id].pitch, 0, blobMemory[id].channel);
              blobMemory[id].active = false;
              blobMemory[id].released = true;

              activeBlobs.erase(std::remove(activeBlobs.begin(), activeBlobs.end(), id), activeBlobs.end());
              continue;
          }

          if (z >= PRESSURE_THRESHOLD) {  
              if (blobMemory.find(id) == blobMemory.end() || !blobMemory[id].active) {
                  int channel = id + 1;
                  blobMemory[id] = {true, false, pitch, channel, x, y, z};

                  sendMidiOn(id, x, y, z, mapped_dx, mapped_dy, mapped_dz);

                  if (std::find(activeBlobs.begin(), activeBlobs.end(), id) == activeBlobs.end()) {
                      activeBlobs.push_back(id);
                  }
              } else {
                  bool positionChanged = (fabs(x - blobMemory[id].lastX) > POSITION_THRESHOLD || 
                                          fabs(y - blobMemory[id].lastY) > POSITION_THRESHOLD);
                  bool pressureChanged = (fabs(z - blobMemory[id].lastZ) > 0.01);

                  if (positionChanged) {
                      updateMidiOnOff(id, x, y, z, mapped_dx, mapped_dy, mapped_dz);
                      blobMemory[id].lastX = x;
                      blobMemory[id].lastY = y;
                  }

                  if (pressureChanged) {
                      sendMPEData(id, x, y, z);
                      blobMemory[id].lastZ = z;
                  }
              }
              activeBlobCount++; // 🚨 Contamos este blob como activo
          }
      }
  }

  // 🚨 Enviar CC120 por el canal 1 solo si cambia la cantidad de blobs activos
  if (activeBlobCount != lastActiveBlobCount) {
      usbMIDI.sendControlChange(103, activeBlobCount, 1); // Enviar cantidad de blobs activos por CC103 canal 1
      lastActiveBlobCount = activeBlobCount; // Actualizar el último valor registrado
  }
}*/

void DataHandler::checkBlobStatus(bool hasActiveBlobs) {
  const float POSITION_THRESHOLD = 0.01;
  const float MAX_D = 1.0;
  static int lastActiveBlobCount = -1;

  int activeBlobCount = 0;
  std::vector<int> currentBlobs; // Lista de blobs activos en esta iteración

  // 🚨 Guardamos los blobs actuales de `blobData`
  for (int i = 0; i < MAX_BLOBS; i++) {
      int id = (int)blobData[i * 9 + 0];
      if (id > 0) {
          currentBlobs.push_back(id);
      }
  }

  // 🔥 Apagar blobs individuales que pasaron de ID >= 1 a 0
  for (auto it = blobMemory.begin(); it != blobMemory.end();) {
      int id = it->first;

      // Si el blob ya no está en la lista actual, significa que desapareció
      if (std::find(currentBlobs.begin(), currentBlobs.end(), id) == currentBlobs.end()) {
          //Serial.print("🔥 Blob ");
          //Serial.print(id);
          //Serial.println(" desapareció. Enviando Note Off...");

          usbMIDI.sendNoteOff(blobMemory[id].pitch, 0, blobMemory[id].channel);
          sendMidiOff(id);

          it = blobMemory.erase(it); // 🚀 Borrar solo este blob de la memoria
      } else {
          ++it;
      }
  }

  if (!hasActiveBlobs) {
      noBlobCycles++;
      if (noBlobCycles >= 2 && !allNotesOffSent) {
          for (int channel = 1; channel <= 16; channel++) {
              usbMIDI.sendControlChange(123, 0, channel);
          }

          for (auto& [blobId, state] : blobMemory) {
              if (state.active) {
                  usbMIDI.sendNoteOff(state.pitch, 0, state.channel);
                  sendMidiOff(blobId);
                  state.active = false;
                  state.released = true;
              }
          }
          blobMemory.clear();
          allNotesOffSent = true;
      }
      activeBlobCount = 0;
  } else {
      noBlobCycles = 0;
      allNotesOffSent = false;

      for (int i = 0; i < MAX_BLOBS; i++) {
          int id = (int)blobData[i * 9 + 0];
          float x = blobData[i * 9 + 1];
          float y = blobData[i * 9 + 3];
          float z = blobData[i * 9 + 5];
          float dx = blobData[i * 9 + 7];
          float dy = blobData[i * 9 + 8];
          float dz = blobData[i * 9 + 9];

          if (id <= 0) continue; // Ignorar si el ID es 0

          int mapped_dx = constrain((dx / (2 * MAX_D)) * 127 + 64, 0, 127);
          //int mapped_dy = constrain((dy / (2 * MAX_D)) * 127 + 64, 0, 127);
          int mapped_dy = constrain((dy * 127 * 10), -127, 127); 
          int mapped_dz = constrain((dz / (2 * MAX_D)) * 127 + 64, 0, 127);

          int pitch = pitchHandler->getPitch(x, y);

          // 🔥 Si el blob es nuevo o estaba apagado, activarlo
          if (blobMemory.find(id) == blobMemory.end() || !blobMemory[id].active) {
              int channel = id + 1;
              blobMemory[id] = {true, false, pitch, channel, x, y, z};

              sendMidiOn(id, x, y, z, mapped_dx, mapped_dy, mapped_dz);
          } else {
              bool positionChanged = (fabs(x - blobMemory[id].lastX) > POSITION_THRESHOLD ||
                                      fabs(y - blobMemory[id].lastY) > POSITION_THRESHOLD);
              bool pressureChanged = (fabs(z - blobMemory[id].lastZ) > 0.01);

              if (positionChanged) {
                  updateMidiOnOff(id, x, y, z, mapped_dx, mapped_dy, mapped_dz);
                  blobMemory[id].lastX = x;
                  blobMemory[id].lastY = y;
              }

              if (pressureChanged) {
                  sendMPEData(id, x, y, z);
                  blobMemory[id].lastZ = z;
              }
          }
          activeBlobCount++;
      }
  }

  // 🚨 Enviar CC120 por el canal 1 solo si cambia la cantidad de blobs activos
  if (activeBlobCount != lastActiveBlobCount) {
      usbMIDI.sendControlChange(103, activeBlobCount, 1);
      lastActiveBlobCount = activeBlobCount;
  }
}

// 🔥 Función para enviar datos MPE (Pitch Bend y AfterTouch)
void DataHandler::sendMPEData(int id, float x, float y, float z) {
    int channel = id + 1;  // Cada blob usa un canal distinto
    int pitchBendValue = round(x * 16383); // Rango completo del pitch bend MIDI

    usbMIDI.sendAfterTouch(127 * z, channel);  // Enviar presión MPE
    usbMIDI.sendPitchBend(pitchBendValue - 8192, channel);  // Enviar pitch bend MPE , de 0 a 16383
}

void DataHandler::sendMidiOn(int id, float x, float y, float z, float dx, float dy, float dz) {
  int pitch = pitchHandler->getPitch(x, y);
  int channel = id + 1;  // Canal MIDI exclusivo por blob

  usbMIDI.sendNoteOn(pitch, 127 * z, channel);  // Enviar nota MIDI
  usbMIDI.sendPitchBend((z * 16383) - 8192, channel);  // Mover pitch según X
  
  // 🎛️ Enviar posición x, y, z directamente por MIDI CC
  usbMIDI.sendControlChange(110, (int)(x * 127), channel);   // Enviar X     (CC 110)
  usbMIDI.sendControlChange(111, (int)(y * 127), channel);   // Enviar Y     (CC 111)
  usbMIDI.sendControlChange(112, (int)(z * 127), channel);   // Enviar Z     (CC 112)
  usbMIDI.sendControlChange(113, (int)(dx * 127), channel);  // Enviar dx    (CC 113)
  usbMIDI.sendControlChange(114, (int)(dy * 127), channel);  // Enviar dy    (CC 114)
  usbMIDI.sendControlChange(115, (int)(dz * 127), channel);  // Enviar dz    (CC 115)
  usbMIDI.sendControlChange(116, (int)(127), channel);  // blob activo  (CC 116)
}

void DataHandler::sendMidiOff(int id) {
  if (blobMemory.find(id) != blobMemory.end() && blobMemory[id].active) {
      int channel = blobMemory[id].channel;
      int pitch = blobMemory[id].pitch;

      usbMIDI.sendNoteOff(pitch, 0, channel);  // Apagar la nota

      // 🎛️ También enviar CC con valores en cero para X, Y y Z
      //usbMIDI.sendControlChange(110, 0, channel);  
      //usbMIDI.sendControlChange(111, 0, channel);  
      //usbMIDI.sendControlChange(112, 0, channel); 
      //usbMIDI.sendControlChange(113, 0, channel);
      //usbMIDI.sendControlChange(114, 0, channel); 
      //usbMIDI.sendControlChange(115, 0, channel);   
      usbMIDI.sendControlChange(116, 0, channel); 
      
  }
}

void DataHandler::updateMidiOnOff(int id, float x, float y, float z,float dx, float dy, float dz) {
  int pitch = pitchHandler->getPitch(x, y);
  int channel = id + 1;

  if (blobMemory[id].pitch != pitch) {
      usbMIDI.sendNoteOff(blobMemory[id].pitch, 0, channel);
      usbMIDI.sendNoteOn(pitch, 127 * z, channel);
      blobMemory[id].pitch = pitch;
  }

  sendMPEData(id, x, y, z);  // Mantiene el MPE
  
  // 🎛️ Actualizar valores X, Y y Z en CCs
  usbMIDI.sendControlChange(110, (int)(x * 127), channel);  
  usbMIDI.sendControlChange(111, (int)(y * 127), channel);  
  usbMIDI.sendControlChange(112, (int)(z * 127), channel);  
  usbMIDI.sendControlChange(113, (int)(dx * 127), channel);  
  usbMIDI.sendControlChange(114, (int)(dy * 127), channel);  
  usbMIDI.sendControlChange(115, (int)(dz * 127), channel); 
  usbMIDI.sendControlChange(116, (int)(127), channel);  

  //Debug
  //Serial.print("dx: ");
  //Serial.print(dx);
  //Serial.print(",dy: ");
  //Serial.println(dy);
}

/*
// 🔥 Función para enviar "Note On"
void DataHandler::sendMidiOn(int id, float x, float y, float z) {
    int pitch = pitchHandler->getPitch(x, y);
    int channel = id + 1;  // Canal MIDI exclusivo por blob

    usbMIDI.sendNoteOn(pitch, 127 * z, channel);
    usbMIDI.sendPitchBend((x * 16383) - 8192, channel);  // Mover pitch según X
}

// 🔥 Función para actualizar nota sin apagarla
void DataHandler::updateMidiOnOff(int id, float x, float y, float z) {
    int pitch = pitchHandler->getPitch(x, y);
    int channel = id + 1;

    if (blobMemory[id].pitch != pitch) {
        usbMIDI.sendNoteOff(blobMemory[id].pitch, 0, channel);
        usbMIDI.sendNoteOn(pitch, 127 * z, channel);
        blobMemory[id].pitch = pitch;
    }

    sendMPEData(id, x, y, z);
}
*/



bool DataHandler::checkSerialBuffer(int index) {

  bool value = true;

  // STRUCTURE: 0 -> 255, 1 -> Threshold, 2 -> NumBlobs, 3+9 -> ID1, 12+9 -> ID3, etc 
  value = ((int) inputData[index + POS_ID_1] == 1) 
          && ((int) inputData[index + POS_ID_2] == 2) 
          && ((int) inputData[index + POS_ID_3] == 3) 
          && ((int) inputData[index + POS_ID_4] == 4) 
          && ((int) inputData[index + POS_ID_5] == 5) 
          && ((int) inputData[index + POS_ID_6] == 6) 
          && ((int) inputData[index + POS_ID_7] == 7)
          && ((int) inputData[index + POS_ID_8] == 8)
          && ((int) inputData[index + POS_ID_9] == 9)
          && ((int) inputData[index + POS_ID_10] == 10);



  return value;

}

void DataHandler::outputMidiData() {

  bool newOn = false;
  bool newOff = false;

  for (int i=0;i<MAX_BLOBS;i++) {

    int id = (int) blobData[i*6+0];

    float x = blobData[i*6+1];
    float y = blobData[i*6+2];
    float z = blobData[i*6+3];
    //float zmax = blobData[i*6+4];
    float area = blobData[i*6+5];

    
    if (z == 0 && lastZ[i]>0) {   // 
      Serial.print(i);
      Serial.println(" Off");
      
      newOff = true;
      //usbMIDI.sendNoteOff(60,0,id+1);
      usbMIDI.sendControlChange(MidiHandler::TX_BLOB_ACTIVE,0,id+1);
      }

    else if(z > 0 && lastZ[i]==0){
      Serial.print(i);
      Serial.println(" On");
      
      newOn = true;      
      //usbMIDI.sendNoteOn(60,90,id+1);
      usbMIDI.sendControlChange(MidiHandler::TX_BLOB_ACTIVE,1,id+1);
      }


      
    //usbMIDI.sendControlChange(control, value, channel);
     if (x != lastX[i]) usbMIDI.sendControlChange(MidiHandler::TX_BLOB_X,127*x,id+1);
     if (y != lastY[i]) usbMIDI.sendControlChange(MidiHandler::TX_BLOB_Y,127*y,id+1);
     if (z != lastZ[i]) usbMIDI.sendControlChange(MidiHandler::TX_BLOB_Z,127*z,id+1);
     if (area != lastA[i]) usbMIDI.sendControlChange(MidiHandler::TX_BLOB_A,127*area,id+1); 
    
      lastX[i] = x;
      lastY[i] = y;
      lastZ[i] = z;
      lastA[i] = area;
      
  }

  if(newOn)
    usbMIDI.sendControlChange(MidiHandler::TX_ON_OFF,1,MidiHandler::MAIN_MIDI_CHANNEL);
  if(newOff)
    usbMIDI.sendControlChange(MidiHandler::TX_ON_OFF,0,MidiHandler::MAIN_MIDI_CHANNEL);
  }


void DataHandler::setSettings(Settings * s) {

  settings = s;
  
}  

void DataHandler::setPitchHandler(PitchHandler * p) {

  pitchHandler = p;
  
} 

void DataHandler::fixFirstBlob() {
  
  // Fix spurious 
  if (settings->oneFinger == 1) { 

    if (settings->numBlobs == 1) {
    
      if ((blobData[1] == 0) && (blobData[2] == 0) && (blobData[3] == 0)) {

            blobData[1] = max(max(blobData[7],blobData[13]),blobData[19]);
            blobData[2] = max(max(blobData[8],blobData[14]),blobData[20]);
            blobData[3] = max(max(blobData[9],blobData[15]),blobData[21]);                        
            blobData[4] = max(max(blobData[10],blobData[16]),blobData[22]);
            blobData[5] = max(max(blobData[11],blobData[17]),blobData[23]);
        }
      }
    }
}

void DataHandler::copyLastBlobData() {
  
    memcpy(lastBlobData, blobData, sizeof(float)*60);

}

/*
void DataHandler::sendMidiOn(int id,float x,float y,float z) {

  int pitch = pitchHandler->getPitch(x,y);
  pitches[id-1] = pitch;
  slidePitches[id-1] = pitch;
  centerPitchBend[id-1] = 0;
  usbMIDI.sendNoteOn(pitches[id-1],127*z,id+1);

  
  
}

void DataHandler::sendMidiOff(int id) {

  if (settings->performanceMode != Settings::MODE_SLIDE) {
    if (slidePitches[id-1] != pitches[id-1]) 
      usbMIDI.sendNoteOff(slidePitches[id-1],0,id+1); 
  }   
  usbMIDI.sendNoteOff(pitches[id-1],0,id+1);  

}

void DataHandler::updateMidiOnOff(int id, float x, float y, float z, float dx, float dy,float bloblife, float velx, float vely) {

  byte oldPitch = slidePitches[id-1];
  byte newPitch = pitchHandler->getPitch(x,y);
  byte newPitchPrevious = pitchHandler->getPreviousPitch(x,y);
  byte newPitchNext = pitchHandler->getNextPitch(x,y);
  //dispFloat("vely ",vely);
  
  //dispFloat("inity ",inity[id-1]);

  //dispInt("prev pitch ",newPitchPrevious);
  //dispInt("next pitch ",newPitchNext);

  //dispInt("old pitch ",oldPitch);
  //dispInt("next pitch ",newPitch);
    
  if (settings->performanceMode == Settings::MODE_SLIDE) {

    if (oldPitch == newPitch) {
      if (bloblife > 3*BLOB_LIFE_THRESHOLD) {
        if (vely > velThreshold) { 
          usbMIDI.sendPitchBend(dy*4*PITCH_BEND_SEMITONE+centerPitchBend[id-1],id+1);
        }
        else {
          inity[id-1] = y;
        }
      }
      slideCross = false;
    }

    else {        
      int distance = (int)newPitch - (int)pitches[id-1];
      int steps = 50;
      float pitchBend = dy*4*PITCH_BEND_SEMITONE;
      float delta = (PITCH_BEND_SEMITONE*distance - pitchBend)/steps;
      for (int i=0;i<steps;i++) {
        usbMIDI.sendPitchBend(pitchBend,id+1);  
        pitchBend += delta;       
      }
      usbMIDI.sendPitchBend(distance*PITCH_BEND_SEMITONE,id+1);        

      if (slideCross == false) {
        slideCross = true;
        life[id-1] = 0;
        inittime[id-1] = millis();
        inity[id-1] = y;
        distancey[id-1] = 0;
        centerPitchBend[id-1] = distance*PITCH_BEND_SEMITONE;
      }
    
    
    }

    int brightness = 1.2*dx*63 + 63;
    
    usbMIDI.sendControlChange(MidiHandler::TX_BRIGHTNESS,brightness,id+1);
    usbMIDI.sendAfterTouch(z*127,id+1);    
    slidePitches[id-1] = newPitch;


    
  }

  else if (settings->performanceMode == Settings::MODE_DISCRETE) {

    if (oldPitch == newPitch) {
      if (bloblife > BLOB_LIFE_THRESHOLD) {
        if (vely > velThreshold) { 
        //usbMIDI.sendPitchBend(dy*4*PITCH_BEND_SEMITONE+centerPitchBend[id-1],id+1);
        }
        else {
          inity[id-1] = y;
        }
      }
      slideCross = false;
    }

    else {        
      int distance = (int)newPitch - (int)pitches[id-1];
      int steps = 50;
      float pitchBend = dy*4*PITCH_BEND_SEMITONE;
      float delta = (PITCH_BEND_SEMITONE*distance - pitchBend)/steps;
      for (int i=0;i<steps;i++) {
        //usbMIDI.sendPitchBend(pitchBend,id+1);  
        pitchBend += delta;       
      }
      //usbMIDI.sendPitchBend(distance*PITCH_BEND_SEMITONE,id+1);        

      if (slideCross == false) {
        slideCross = true;
        life[id-1] = 0;
        inittime[id-1] = millis();
        inity[id-1] = y;
        distancey[id-1] = 0;
        centerPitchBend[id-1] = distance*PITCH_BEND_SEMITONE;
        usbMIDI.sendNoteOff(oldPitch,0,id+1);
        usbMIDI.sendNoteOn(newPitch,127*z,id+1);
      }
    }

    int brightness = 1.2*dx*63 + 63;
    
    usbMIDI.sendControlChange(MidiHandler::TX_BRIGHTNESS,brightness,id+1);
    usbMIDI.sendAfterTouch(z*127,id+1);    
    slidePitches[id-1] = newPitch;
  }

  else if (settings->performanceMode == Settings::MODE_DISCRETE_SLIDE) {

   if (oldPitch == newPitch) {
      if (bloblife > BLOB_LIFE_THRESHOLD) {
        if (vely > velThreshold) { 
          usbMIDI.sendPitchBend(dy*4*PITCH_BEND_SEMITONE+centerPitchBend[id-1],id+1);
        }
        else {
          inity[id-1] = y;
        }
      }
      slideCross = false;
    }

    else {        
      int distance = (int)newPitch - (int)pitches[id-1];
      int steps = 50;
      float pitchBend = dy*4*PITCH_BEND_SEMITONE;
      float delta = (PITCH_BEND_SEMITONE*distance - pitchBend)/steps;
      for (int i=0;i<steps;i++) {
        usbMIDI.sendPitchBend(pitchBend,id+1);  
        pitchBend += delta;       
      }
      usbMIDI.sendPitchBend(distance*PITCH_BEND_SEMITONE,id+1);        

      if (slideCross == false) {
        slideCross = true;
        life[id-1] = 0;
        inittime[id-1] = millis();
        inity[id-1] = y;
        distancey[id-1] = 0;
        centerPitchBend[id-1] = 0;
        usbMIDI.sendNoteOff(oldPitch,0,id+1);
        usbMIDI.sendNoteOn(newPitch,127*z,id+1);
        pitches[id-1] = newPitch;
      }
    
    
    }

    int brightness = 1.2*dx*63 + 63;
    
    usbMIDI.sendControlChange(MidiHandler::TX_BRIGHTNESS,brightness,id+1);
    usbMIDI.sendAfterTouch(z*127,id+1);    
    slidePitches[id-1] = newPitch;
    //dispInt("new Pitch ",newPitch);

    

  }

  
}
*/

void DataHandler::printBlobData() {

  for(int i=0;i<20;i++) {
    Serial.print(blobData[i]);
    Serial.print(",");
  }
  Serial.println(".");
}

void DataHandler::printInputData() {

  for(int i=0;i<2*(DATA_SIZE-1);i++) {
    Serial.print(inputData[i]);
    Serial.print(",");
  }
  Serial.println(".");
}
