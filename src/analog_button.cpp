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
#include "command_mode.h"

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
      buttonEvent = (millis() - lastButtonReadMs > 250 ) ? BTN_EVENT_LONG : BTN_EVENT_SHORT ;
      lastButtonReadMs = millis();
      // Serial.print("Button press ");
      // Serial.println( buttonEvent == BTN_EVENT_SHORT ? "short" : "long" );
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
    if (!commandMode.isActive && (buttonEvent == BTN_EVENT_LONG)) // send message 
    { // BTN_EVENT_LONG
      protocol.sendMessage();
    }
    else
    {
      if (commandMode.isActive)
      {
        digitalWrite(PIN_MODE_LED, LOW);
        commandMode.exit();
      }
      else
      {
        commandMode.init();
      }
    }
    buttonEvent = 0;
  }
}