/**
 * Analog button or rotary encoder switch 
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/
#include <Arduino.h>
#include "pins.h"

#define ANALOG_BTN_READ_INTERVAL 10  // minimum interval between two reads

bool isButtonPressed = false ;
bool buttonEvent = false ;

unsigned long lastButtonReadMs = 0 ;

void checkAnalogButton() {
  bool pressed ;
  if( buttonEvent ) {
    lastButtonReadMs = 0 ;
    return ; // do not do anything until event is cleared
  }
  if( millis() - lastButtonReadMs < ANALOG_BTN_READ_INTERVAL ) return ; // do not do anything until the minimum period elapsed
  pressed = analogRead( PIN_ROTARY_SWITCH ) < 100 ;
  // if the vale is the same, do nothing
  if( isButtonPressed ) {
    if( pressed ) return ; // was pressed and remains pressed
    else {
      isButtonPressed = pressed ;
      buttonEvent = true ;
      lastButtonReadMs = millis();
    }
  }
  else { // button is not pressed 
    if( !pressed ) return ; // was not pressed and remains not pressed
    else {
      isButtonPressed = !isButtonPressed ;
      lastButtonReadMs = millis();
    }
  }
}
