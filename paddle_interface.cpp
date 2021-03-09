/**
 * Paddle variables and handling 
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/
#include <Arduino.h>
#include <avr/io.h>

#include "pins.h"
#include "core.h"
#include "core_variables.h"

#include "config.h"
#include "protocol.h"
#include "keyer_interface.h"
#include "paddle_interface.h"
#include "morse.h"

#if (PIN_PADDLE_LEFT > 1) && (PIN_PADDLE_LEFT < 8)
#define LEFT_GROUP 2
#elif PIN_PADDLE_LEFT < 14
#define LEFT_GROUP 0
#elif PIN_PADDLE_LEFT < 20
#define LEFT_GROUP 1
#else
#error "No PCINT group for PADDLE_LEFT"
#endif

#if (PIN_PADDLE_RIGHT > 1) && (PIN_PADDLE_RIGHT < 8)
#define RIGHT_GROUP 2
#define GROUP_SHIFT 0
#elif PIN_PADDLE_RIGHT < 14
#define RIGHT_GROUP 0
#define GROUP_SHIFT 8
#elif PIN_PADDLE_RIGHT < 20
#define RIGHT_GROUP 1
#define GROUP_SHIFT 14
#else
#error "No PCINT group for PADDLE_RIGHT"
#endif

#if (LEFT_GROUP) != (RIGHT_GROUP)
#error "Left and right paddles are no in the same PCINT group!"
#endif 

#if (RIGHT_GROUP) == 0

#define PADDLE_INT_VECTOR PCINT0_vect
#define PADDLE_INT_MASK_REG PCMSK0
#define PADDLE_PCIE_BIT (1 << (PCIE0))
#define PADDLE_LEFT_BIT (1 << (PIN_PADDLE_LEFT - (GROUP_SHIFT)))
#define PADDLE_RIGHT_BIT (1 << (PIN_PADDLE_RIGHT - (GROUP_SHIFT)))
#define PADDLE_INT_MASK ((PADDLE_LEFT_BIT) | (PADDLE_RIGHT_BIT))
#define PADDLE_PINS PINB

#elif (RIGHT_GROUP) == 1

#define PADDLE_INT_VECTOR PCINT1_vect
#define PADDLE_INT_MASK_REG PCMSK1
#define PADDLE_PCIE_BIT (1 << (PCIE1))
#define PADDLE_LEFT_BIT (1 << (PIN_PADDLE_LEFT - (GROUP_SHIFT)))
#define PADDLE_RIGHT_BIT (1 << (PIN_PADDLE_RIGHT - (GROUP_SHIFT)))
#define PADDLE_INT_MASK ((PADDLE_LEFT_BIT) | (PADDLE_RIGHT_BIT))
#define PADDLE_PINS PINC

#elif (RIGHT_GROUP) == 2

#define PADDLE_INT_VECTOR PCINT2_vect
#define PADDLE_INT_MASK_REG PCMSK2
#define PADDLE_PCIE_BIT (1 << (PCIE2))
#define PADDLE_LEFT_BIT (1 << (PIN_PADDLE_LEFT - (GROUP_SHIFT)))
#define PADDLE_RIGHT_BIT (1 << (PIN_PADDLE_RIGHT - (GROUP_SHIFT)))
#define PADDLE_INT_MASK ((PADDLE_LEFT_BIT) | (PADDLE_RIGHT_BIT))
#define PADDLE_PINS PIND

#else 

#error "Paddle interrupt setup failed"
// NOTE: if the paddles are not in the same PCINT group, interrupt data will not be generated!

#endif

PaddleInterface paddle = PaddleInterface();
volatile byte paddleState = 0 ;
bool paddleBreakIsPending = false;

void PaddleInterface::setup() {
  pinMode(PIN_PADDLE_LEFT, INPUT_PULLUP);
  pinMode(PIN_PADDLE_RIGHT, INPUT_PULLUP);
}

void PaddleInterface::reset()
{
  buffer = 0;
  isSqueezed = false;
}

    // activate interrupt
void PaddleInterface::enableInterrupt()
{
  if ((PCIFR & PADDLE_PCIE_BIT) == 0)
  {
    PADDLE_INT_MASK_REG = PADDLE_INT_MASK;
    PCIFR &= ~(PADDLE_PCIE_BIT); // delete interrupt flag if it was set
    PCICR |= PADDLE_PCIE_BIT;
    reset();
  }
}

void PaddleInterface::disableInterrupt()
{
  PCICR &= ~(PADDLE_PCIE_BIT);
  PCIFR &= ~(PADDLE_PCIE_BIT); // delete interrupt flag if it was set
}

void PaddleInterface::handleInterrupt()
{
  if (paddleBreakIsPending)
  {
    paddleBreakIsPending = !(digitalRead(PIN_PADDLE_LEFT) && digitalRead(PIN_PADDLE_RIGHT));
  }
}

void PaddleInterface::setSqueeze() {
  isSqueezed = (config.paddleMode == IAMBIC_A) && digitalRead(PIN_PADDLE_LEFT) == LOW && digitalRead(PIN_PADDLE_RIGHT) == LOW;
}

void PaddleInterface::check(byte paddle)
{
  byte pin = (paddle == DIT)
                 ? (config.isPaddleSwapped ? PIN_PADDLE_LEFT : PIN_PADDLE_RIGHT)
                 : (config.isPaddleSwapped ? PIN_PADDLE_RIGHT : PIN_PADDLE_LEFT);
  byte paddle_state = digitalRead(pin);
  if (paddle_state == 0 && !paddleBreakIsPending) // after paddle break, do not fill dit or dah memory until paddles released
  {                                               // paddle squeezed, do something
    buffer |= paddle ;
    keyerInterface.forceKey(false); // if the key line was forced, release it
    // Update last_event only if it was set by ptt():
    keyerInterface.resetPttEvent();
    if( wpmMaxPaddle > 0 && wpm > wpmMaxPaddle ) { wpm = wpmMaxPaddle; }
  }
}

void PaddleInterface::serviceBuffers() { 
  if ((config.paddleMode == IAMBIC_A) && isSqueezed && digitalRead(PIN_PADDLE_LEFT) && digitalRead(PIN_PADDLE_RIGHT)) // release condition
  {
    isSqueezed = false;
    buffer = 0;
  }
  else
  {
    if (buffer & DIT)
    {
      buffer &= ~DIT; // reset buffer
      morseEngine.sendMorseElement(DIT);
    }
    if (buffer & DAH)
    {
      buffer &= ~DAH;
      morseEngine.sendMorseElement(DAH);
    }
  }
}

ISR(PADDLE_INT_VECTOR) { 
  if( !paddleBreakIsPending ) {
    paddleBreakIsPending = true;
    paddle.wasTouched = true;
    paddle.buffer = 0;
    paddle.isSqueezed = false ;
    protocol.resetSendBuffer();
    PCICR &= ~(PADDLE_PCIE_BIT); // disable further interrupt; must be enabled when Protocol starts sending from buffer 
  }
}

