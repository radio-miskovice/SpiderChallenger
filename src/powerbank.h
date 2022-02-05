#ifndef POWERBANK_H
#define POWERBANK_H

#include <Arduino.h>

class Powerbank
{
private:
  unsigned long lastEventMs = 0;
  byte currentLevel = HIGH;
  void set(byte level);
public:
  void setup();
  void check();
};

extern Powerbank powerBank ;

#endif