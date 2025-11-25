/** Arcontinuo 1512 
 *  Rodrigo F. Cadiz
 *  2018
 *  Last update: Mar-05-2018
 */

#include "buttons.h"
#include "settings.h"
#include "pitchHandler.h"
#include "midiHandler.h"

Buttons::Buttons() {
  
}

void Buttons::setSettings(Settings * s) {
  settings = s;
}

void Buttons::setPitchHandler(PitchHandler *p) {
  pitchHandler = p;
}

void Buttons::begin() {

  for (int i=0; i<NUM_BUTTONS; i++) {
    pinMode(btnPins[i], INPUT_PULLUP);
  }
  
}


void Buttons::check() {

  for(int i=0;i<NUM_BUTTONS;i++) {

    if (time_pressed[i] == 0 || (millis() - time_pressed[i]) > 100) { // Filter out fast repetitions

      int buttonState = digitalRead(btnPins[i]);

      if (buttonState != state[i]) { // Button changed

        time_pressed[i] = millis();

        if (buttonState == LOW) { // Button was just pressed
          buttonPressed(i);
          state[i] = buttonState;
          usbMIDI.sendControlChange(MidiHandler::TX_BUTTON_BASE+i,127,1);
          
        }

        else { // Button was just released
          buttonReleased(i);
          time_pressed[i] = 0;
          state[i] = buttonState;
          usbMIDI.sendControlChange(MidiHandler::TX_BUTTON_BASE+i,0,1);
        }
         
      }

      else { // Button not changed

         if (buttonState == LOW) { // Button is pressed

          /*if (Buttons.time_pressed[i] > (unsigned long) 600000) {

           buttonLongPressed(i);   
            Buttons.time_pressed[i] = 0;
          }

          else {
            Buttons.time_pressed[i] += millis();
          }*/
        }
        // Do nothing otherwise
      
      }
    }
  }
}


void Buttons::buttonPressed(int i) {

  switch(i) {

    case MIDDLE_LEFT:
      if (state[LEFT_LEFT] == LOW) {
        settings->presetUp();
        disp("Preset Up");
      }
      else {
        pitchHandler->base_transposition += 12;
        disp("Transpose up");
      }
      break;

    case MIDDLE_RIGHT:
      if (state[LEFT_LEFT] == LOW) {
        settings->presetDown();
        disp("Preset down");
      }
      else {
        pitchHandler->base_transposition -=12;
        disp("Transpose down");
      }
      break;

    case RIGHT_RIGHT: 
      settings->changePerformanceMode();
      disp("Changing performance mode");
      break;

    case LEFT_LEFT: 
      disp("Selector");
      break;

      
   }

}

void Buttons::buttonReleased(int i) {

  //dispInt("Button released ",i);
  
}

void Buttons::buttonLongPressed(int i) {

   switch(i) {

    case LEFT_LEFT:
      if (configmode == 0) {
        settings->stopPreset();
      }
      break;

    case RIGHT_RIGHT:
      if (configmode == 1) {
        settings->startPreset();
      }
      break;
    
   }
  
}


/*
void Buttons::checkButtons2() {

  int buttonState;

  for (int i=0; i<Control.NUM_BUTTONS; i++){
    
    if (Control.time_since_last_pressed[i] == 0 || (millis() - Control.time_since_last_pressed[i]) > 100) { // Filter out fast repetitions
      
      buttonState = digitalRead(Control.btnPins[i]); // Get state of button 

      if (buttonState != Control.last_button_state[i]) { // If state has changed
        Control.time_since_last_pressed[i] = millis();
 
        if (buttonState == LOW) {
          Control.last_button_state[i] = LOW;
          disp("LOW");
        }
        else if (buttonState == HIGH) {
          Control.last_button_state[i] = HIGH;


          
          Control.button_status[i] = !Control.button_status[i];
           configure(i);
          disp("HIGH");
        }
      }

      else {  // State is the same as before
        if (buttonValue == LOW) {
          Control.time_since_last_pressed[i] += millis();
          if (Control.time_since_last_pressed[i] > (unsigned long) 350000) {
            if (i == Buttons.LEFT_LEFT) 
              startPreset();
            else if (i == Buttons.RIGHT_RIGHT)
              stopPreset();
          }
        }
      }

      
    }

    
  }
}*/
