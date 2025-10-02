/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#include "logo.h"

Logo::Logo() {
  
}

void Logo::begin() {
  
  pixels.begin();
  setLight(255,255,0);

}

void Logo::setLight(int r, int g, int b) {

  setLightUp(r,g,b);
  setLightDown(r,g,b);
  pixels.show(); 

}

void Logo::setLightUp(int r, int g, int b) {
  
  pixels.setPixelColor(0, pixels.Color(r,g,b));
  pixels.show(); 

}

void Logo::setLightDown(int r, int g, int b) {
  
  pixels.setPixelColor(1, pixels.Color(r,g,b));
  pixels.show(); 

}

void Logo::blinkUp(int r1, int g1, int b1, int r2, int g2, int b2, int times, int del) {

  for(int i=0;i<times;i++) {
    setLightUp(r1,g1,b1);
    delay(del);
    setLightUp(r2,g2,b2);
    delay(del);
  }
  setLightUp(r1,g1,b1);  
}

void Logo::blinkDown(int r1, int g1, int b1, int r2, int g2, int b2, int times, int del) {

  for(int i=0;i<times;i++) {
    setLightDown(r1,g1,b1);
    delay(del);
    setLightDown(r2,g2,b2);
    delay(del);
  }
  setLightDown(r2,g2,b2);  
}
