/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#include <utils.h>
 
void Utils::dispInt(const char *text,int value) {

  Serial.print(text);
  Serial.println(value);
  
}

void Utils::dispFloat(const char *text,float value) {

  Serial.print(text);
  Serial.println(value);
  
}

void Utils::dispUnsignedLong(const char *text,unsigned long value) {

  Serial.print(text);
  Serial.println(value);
  
}

void Utils::disp(const char *text) {
  
  Serial.println(text);

}

void Utils::error(const char * text) {

  Serial.print("ERROR: ");
  disp(text);
  
}




int Utils::quantize(float value,int low,int high) {
  return scale(value,low,high);  
}

int Utils::scale(float value,int low,int high) {
  return (int) (value*(float)(high-low) + low);
}




int Utils::qmap(float value, int low, int high) {
  
  return (int)((float)value*value*(high-low) + (float)low);
  
}


int Utils::pitchBendMap(float val, float range) {

  float r = 0;
  float value = val/range;

  if (value >= 0) {
    r = value*value*127;
  }
  else {
    r = -value*value*127.0;
  }
  
  if (r > 127.0) r = 127;
  else if (r < -127) r = -127;
  
  return (int) r;
}

int Utils::brightnessMap(float val, float range) {

  float r = 60;
  float value = val/range;

  if (value >= 0) {
    r = value*value*67.0 + 60.0;
  }
  else {
    r = 60.0-value*value*60.0;
  }
  
  if (r > 127.0) r = 127;
  else if (r < 0.0) r = 0;
  
  return (int) r;
}
