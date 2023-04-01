#include <Arduino.h>

#include "pins.h"
#include "config.h"
#include "core.h"
#include "core_variables.h"
#include "protocol.h"
#include "morse.h"
#include "command_mode.h"
#include "paddle_interface.h"
#include "keyer_interface.h"

#ifndef USE_PTT
#if defined(PIN_PTT_LINE) && (PIN_PTT_LINE) != 0

#define USE_PTT

#endif
#endif

KeyingInterface keyerInterface = KeyingInterface();

void KeyingInterface::setup() {
  pinMode( PIN_KEY_LINE, OUTPUT );
  digitalWrite( PIN_KEY_LINE, LOW );
  #ifdef USE_PTT
  pinMode( PIN_PTT_LINE, OUTPUT );
  digitalWrite( PIN_PTT_LINE, LOW );
  #endif
}

void KeyingInterface::forcePtt(bool isForced)
{
  isPttForced = isForced ;
  if( isForced ) setPtt( HIGH );
}

void KeyingInterface::forceKey(bool isForced)
{
  isKeyForced = isForced ;
  if( isForced ) setKey(HIGH);
}

/**
 * Enable PTT line action. If PTT is disabled, it will be turned off as well.
 * If the PTT feature is not configured (USE_PTT not defined or PIN_PTT_LINE == 0 ),
 * no port pin will actually be written.
 *  
 * @param is_enabled PTT will be enabled if true, otherwise disabled.
 **/
void KeyingInterface::enablePtt(bool is_enabled)
{
  isPttEnabled = is_enabled;
  if (!is_enabled)
  {
    pttState = LOW;
#ifdef USE_PTT
    digitalWrite(PIN_PTT_LINE, LOW); // if PTT disabled, turn PTT off
#endif
  }
}

/**
 * Enable KEY line action. If KEY is disabled, it will be turned off as well
 * @param is_enabled KEY will be enabled if true, otherwise disabled.
 **/
void KeyingInterface::enableKey(bool is_enabled)
{
  isKeyEnabled = is_enabled;
  if (!is_enabled)
  {
    keyState = LOW;
    digitalWrite(PIN_KEY_LINE, LOW);
  }
}

/**
 * Set KEY to desired state, HIGH (on) or LOW (off).
 * If PTT is enabled and KEY activates PTT, PTT lead time is included.
 * @param state the new state (HIGH or LOW).
 **/
void KeyingInterface::setKey(byte state)
{
  if (state == keyState)
    return; // if state did not change, do nothing
  keyState = state;
  if (state == LOW)
  {
    noTone(PIN_SIDETONE);
  }
  else
  { // state == HIGH, KEY ON
    word pitch = config.toneManualHz;
    if (isSendingBuffer != 0)
      pitch = config.toneAutoHz;

    if (commandMode.isActive)

    {
      pitch = 2048; // in command mode use high pitch
      state = LOW;  // in command mode no actual keying
    }
    if (!isKeyEnabled)
      state = LOW; // if keying is not enabled, no actual keying
    tone(PIN_SIDETONE, pitch);
  }
  if (state && (config.paddleTriggerPtt || protocol.isSendingBuffer()))
  {
    setPtt(HIGH); // this includes PTT lead time!
  }
  digitalWrite(PIN_KEY_LINE, state); // for KEY ON depends on other circumstances
}

/**
 * Set PTT to desired state, HIGH (on) or LOW (off).
 * If the PTT feature is not configured (USE_PTT not defined or PIN_PTT_LINE == 0 ),
 * no port pin will actually be written and ptt lead time will not be applied.
 * @param state the new state (HIGH or LOW).
 **/
void KeyingInterface::setPtt(byte state)
{
  if (state == pttState)
    return; // do nothing if PTT state not changed
  pttState = state;
  if (isPttEnabled)
  {
#ifdef USE_PTT
    digitalWrite(PIN_PTT_LINE, state);
    if (state)
      delay(config.pttLeadMs);
#endif
    pttLastEventMs = millis();
    if (state)
      pttTime = pttLastEventMs;
    else
      pttTime = 0;
  }
}

/**
 * In case PTT is ON, this method will check if it 
 * has to be turned OFF (when KEY is off and PTT hang and/or tail time elapsed).
 */
void KeyingInterface::checkPttTail()
{
  unsigned long currentTime = millis(); // fix the current moment
  if (keyState == HIGH)
  {
    pttTime = currentTime;
  }
  else
  { // KEY is off
    if (pttState == HIGH && !isPttForced)
    { // PTT is on as result of KEY on
      unsigned long hangTime = 0UL;
      if (currentTime - pttTime >= (config.pttTailMs + hangTime))
      {
        setPtt(LOW);
      }
    }
  }
}

void KeyingInterface::resetPttEvent()
{
  if (pttLastEventMs != 0)
    pttLastEventMs = millis();
}

byte KeyingInterface::getInterfaceStatus()
{
  return (pttState << 1 | keyState);
}

void KeyingInterface::holdElementDuration(unsigned int length, int speed_wpm)
{
  unsigned long start = millis();
  unsigned long ticks = 12 * length / speed_wpm;
  while (((millis() - start) < ticks))
  {
    paddle.setSqueeze(); // set squeeze flag for later comparison
    checkPttTail();
    switch( currentlyEmitting ) {
      case EMIT_DIT:
        paddle.check(paddle.DAH);
        break;
      case EMIT_DAH:
        paddle.check(paddle.DIT);
        break;
      default:
        paddle.check(paddle.DAH);
        paddle.check(paddle.DIT);
    }
  }

  // check if both paddles were released while in IAMBIC_A squeeze
  if ((config.paddleMode == IAMBIC_A) && paddle.isSqueezed && digitalRead(PIN_PADDLE_LEFT) && digitalRead(PIN_PADDLE_RIGHT))
  {
    paddle.reset();
  }
}