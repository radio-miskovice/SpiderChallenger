#ifndef PADDLE_INTERFACE_H
#define PADDLE_INTERFACE_H

#include <Arduino.h>
#include "core.h"

class PaddleInterface
{
public:
  static const byte DIT = EMIT_DIT ; 
  static const byte DAH = EMIT_DAH ;
  // volatile byte sendingMode = SEND_MODE_PADDLE; // SEND_MODE_MAN, SEND_MODE_AUTO
  // volatile byte sendingModeLast = SEND_MODE_PADDLE;
  volatile byte buffer = 0; // bit 0: dit, bit1: dah
  volatile bool wasTouched = false;
  bool isSqueezed; // ex: iambic_flag, i.e. both paddles on
  void setup();   // HW ports setup
  void reset();   // reset buffers and flags
  void enableInterrupt();  // set up and enable paddle interrupt
  void disableInterrupt(); // disable paddle interrupt and reset
  void handleInterrupt();  // handle break event, reset buffers
  void setSqueeze(); // check squeeze only
  void check(byte paddle); // check paddle 
  void serviceBuffers();    // handle buffered dits and dahs
};

extern PaddleInterface paddle ; // paddle interface singleton

#endif