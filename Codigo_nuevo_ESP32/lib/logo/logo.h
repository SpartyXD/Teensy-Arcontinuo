/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#pragma once
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

class Logo {
    
  public:

    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUM_PIXELS, LOGO_PIN, NEO_GRB + NEO_KHZ800); // Pixels (Arcontinuo Logo)
    static const byte LOGO_PIN = 5;  // NeoPixel pin
    static const byte NUM_PIXELS = 2; // Number of NeoPixels

    Logo();
    void begin();
    void setLight(int r, int g, int b);
    void setLightUp(int r, int g, int b); 
    void setLightDown(int r, int g, int b); 
    void blinkUp(int, int, int, int, int ,int, int, int); 
    void blinkDown(int, int, int, int, int ,int, int, int); 
};
