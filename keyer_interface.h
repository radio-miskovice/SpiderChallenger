#ifndef KEYER_INTERFACE_H
#define KEYER_INTERFACE_H

#include <Arduino.h>
#include <avr/io.h>

class KeyingInterface
{
private:
  bool isPttForced = false;
  bool isPttEnabled = false;
  word pttTime = 0;
  word pttLastEventMs = 0;
  byte pttState = LOW;
  bool isKeyForced = false;
  byte key_state = LOW;
  bool is_key_enabled = true;

public:
  byte being_sent = EMIT_NONE;
  void setup() ; // HW setup of all ports
  void reset() ; // reset PTT, KEY, being sent
  void enablePtt(bool is_enabled); // enable or disable PTT line action
  void forcePtt( bool isForced );  // force-on PTT
  void setPtt(byte state);  // set PTT line LOW (off) or HIGH (on)
  void checkPttTail();
  void resetPttEvent();

  void enableKey(bool is_enabled); // enable or disable KEY line action
  void forceKey(bool isForced);    // force-on KEY
  void setKey(byte state);         // set KEY line LOW (off) or HIGH (on)
  
  byte getInterfaceStatus();   // return bitwise status of PTT (bit 1) and KEY (bit 0)

  void holdElementDuration(unsigned int length, int speed_wpm);
};

extern KeyingInterface keyerInterface ;
#endif 