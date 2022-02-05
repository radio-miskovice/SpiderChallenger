/* ROTARY ENCODER VARIABLES */
#ifndef ROTARY_ENCODER_H 
#define ROTARY_ENCODER_H 

#include <avr/io.h>
#include <Arduino.h>

#include "speed_controller.h"

class RotaryEncoder : public SpeedController {
  private: 
    byte valueIncrement = 0;
  public: 
    volatile bool isEventPending = false ;
    RotaryEncoder( byte defaultValue = 25 );
  void init() override;
  void update() override;
  void increment(byte n);
  void enableInterrupt();
  void disableInterrupt();
} ;

extern RotaryEncoder encoder ;

#endif 