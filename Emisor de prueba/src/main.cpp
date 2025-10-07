#include <Arduino.h>
#include <HardwareSerial.h>

HardwareSerial SerialtoS3(1);

void setup(){
  Serial.begin(115200);
  Serial.println("Inicializando emisor :D");
  SerialtoS3.begin(115200, SERIAL_8N1, 16, 17);
}


void loop(){
  while(Serial.available())
    SerialtoS3.println(Serial.readStringUntil('\n'));
  delay(100);
}