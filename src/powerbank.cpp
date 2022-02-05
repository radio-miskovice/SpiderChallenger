#include <Arduino.h>
#include "pins.h"
#include "powerbank.h"

#if (defined( PIN_PB_DUMMY1 ) && (PIN_PB_DUMMY1 > 0)) || (defined(PIN_PB_DUMMY2) && (PIN_PB_DUMMY2>0))
#define USE_POWERBANK
#endif

#ifndef POWERBANK_INTERVAL
#define POWERBANK_INTERVAL 45
#endif

Powerbank powerBank = Powerbank();

void Powerbank::set(byte level)
{
#if defined(PIN_PB_DUMMY1) && (PIN_PB_DUMMY1 > 0)
  digitalWrite(PIN_PB_DUMMY1, level);
#endif

#if defined(PIN_PB_DUMMY2) && (PIN_PB_DUMMY2 > 0)
  digitalWrite(PIN_PB_DUMMY2, level);
#endif

  lastEventMs = millis();
}

void Powerbank::setup()
{
#if defined(PIN_PB_DUMMY1) && (PIN_PB_DUMMY1 > 0)
  pinMode(PIN_PB_DUMMY1, OUTPUT);
#endif

#if defined(PIN_PB_DUMMY2) && (PIN_PB_DUMMY2 > 0)
  pinMode(PIN_PB_DUMMY2, OUTPUT);
#endif
}

void Powerbank::check()
{
  if (millis() < lastEventMs)
    lastEventMs = 0; // millis() overrun mitigation
  if ((millis() - lastEventMs) > (1000UL * POWERBANK_INTERVAL))
  {
    currentLevel = 1 - currentLevel; // toggle
    set(currentLevel);
    lastEventMs = millis();
  }
}

