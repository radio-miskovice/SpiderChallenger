/**
 * Analog button or rotary encoder switch 
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/
#include <Arduino.h>
#include "pins.h"
#include "analog_button.h"
#include "core_variables.h"
#include "core.h"
#include "protocol.h"

#define BTN_EVENT_SHORT 1
#define BTN_EVENT_LONG 2
#define ANALOG_BTN_READ_INTERVAL 10  // minimum interval between two reads

byte buttonEvent = 0 ;
bool isButtonPressed = false ;
unsigned long lastButtonReadMs = 0 ;

void checkAnalogButton() {
  bool pressed ;
  if( buttonEvent > 0 ) {
    lastButtonReadMs = 0 ;
    return ; // do not do anything until event is cleared
  }
  if( millis() - lastButtonReadMs < ANALOG_BTN_READ_INTERVAL ) return ; // do not do anything until the minimum period elapsed
  pressed = analogRead( PIN_ROTARY_SWITCH ) < 100 ;
  // if the vale is the same, do nothing
  if( isButtonPressed ) {
    if( pressed ) return ; // was pressed and remains pressed
    else { // button is not pressed 
      isButtonPressed = pressed ;
      buttonEvent = (millis() - lastButtonReadMs > 500 ) ? BTN_EVENT_LONG : BTN_EVENT_SHORT ;
      lastButtonReadMs = millis();
      Serial.print("Button press ");
      Serial.println( buttonEvent == BTN_EVENT_SHORT ? "short" : "long" );
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

void serviceCommandButton() {
  if (buttonEvent > 0)
  {
    if (isCommandMode || (buttonEvent == BTN_EVENT_LONG))
    {
      byte x = 1 - digitalRead(PIN_MODE_LED);
      digitalWrite(PIN_MODE_LED, x);
      if (!x)
      {
        tone(PIN_SIDETONE, 2048);
        delay(125);
        tone(PIN_SIDETONE, 256);
        delay(125);
        noTone(PIN_SIDETONE);
        isCommandMode = false;
      }
      else
      {
        tone(PIN_SIDETONE, 256);
        delay(125);
        tone(PIN_SIDETONE, 2048);
        delay(125);
        noTone(PIN_SIDETONE);
        isCommandMode = true;
      }
    }
    else
    { // BTN_EVENT_SHORT
      protocol.sendMessage();
    }
    buttonEvent = false;
  }
}