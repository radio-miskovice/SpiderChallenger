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

#include "morse.h"
#include "command_mode.h"
#include "config.h"
#include "protocol.h"

#include "keyer_interface.h"
#include "paddle_interface.h"

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
volatile bool paddleBreakIsPending = false;

void PaddleInterface::setup()
{
  pinMode(PIN_PADDLE_LEFT, INPUT_PULLUP);
  pinMode(PIN_PADDLE_RIGHT, INPUT_PULLUP);
}

void PaddleInterface::reset()
{
  buffer = 0;
  isSqueezed = false;
  wasTouched = false;
}

// activate interrupt
void PaddleInterface::enableInterrupt()
{
  noInterrupts();
  if ((PCIFR & PADDLE_PCIE_BIT) != 0)
  {
    PCIFR |= PADDLE_PCIE_BIT ; // delete interrupt flag if it was set
  }
  PADDLE_INT_MASK_REG = PADDLE_INT_MASK;
  PCICR |= PADDLE_PCIE_BIT;
  interrupts();
}

void PaddleInterface::disableInterrupt()
{
  PCICR &= ~(PADDLE_PCIE_BIT);
  PCIFR |= PADDLE_PCIE_BIT; // clear interrupt flag if it was set
}

void PaddleInterface::handleInterrupt()
{
  PCIFR |= PADDLE_PCIE_BIT ; // reset interrupt flag
  if (paddleBreakIsPending)
  { bool paddleReleased ;
    delay(10);
    paddleReleased = digitalRead(PIN_PADDLE_LEFT) && digitalRead(PIN_PADDLE_RIGHT);
    if( paddleReleased ) {
      paddleBreakIsPending = false ; // reset pending paddle break flag
      wasTouched = true ;    // indicate that paddle was touched (important for correct status message)
      wpm = wpmPrevious ;    // restore previously set speed
      protocol.sendStatus(); 
    }
    else {
      protocol.resetSendBuffer(); // stop buffer sending
    }
  }
}

void PaddleInterface::setSqueeze()
{
  isSqueezed = (config.paddleMode == IAMBIC_A) && digitalRead(PIN_PADDLE_LEFT) == LOW && digitalRead(PIN_PADDLE_RIGHT) == LOW;
}

/** Check paddle status
 *  Also detects squeeze condition (both paddles pressed simultaneously).
 *  Resets paddle break semaphore on release paddles 
 * 
 * **/
void PaddleInterface::check(byte paddleValue)
{
  byte paddles = digitalRead(PIN_PADDLE_LEFT) * 2 + digitalRead(PIN_PADDLE_RIGHT);
  bool keyIsForced = keyerInterface.isKeyForced ; // remember original force state
  if( paddleBreakIsPending ) {
    wasTouched = true ;
    delay(10);
    if ((digitalRead(PIN_PADDLE_LEFT) == HIGH) && (digitalRead(PIN_PADDLE_RIGHT) == HIGH)) // paddle release condition, completes the break
    {                                
      paddleBreakIsPending = false ; // reset break flag
      protocol.sendStatus();
    }
    else { // still waiting for release
      protocol.resetSendBuffer();
    }
    return ; // do nothing while break did not complete
  }
  // if( commandMode.isActive && paddles == 3 && buffer == 0 ) { // decode character if idle
  //   morseEngine.decodeKeyedCharacter();
  // }
  paddles = paddles & (config.isPaddleSwapped ? (paddleValue == DIT ? DAH : DIT) : paddleValue);
  if (paddles == 0) 
  {
    // after paddle break, do not fill dit or dah memory until paddles are released
    buffer |= paddleValue; // set appropriate paddle bit
    paddle.wasTouched = true ;
    keyerInterface.forceKey(false); // if the key line was forced, release it
    // Update last_event only if it was set by ptt():
    keyerInterface.resetPttEvent();
    // manual speed limit, if set
    if (wpmMaxPaddle > 0 && wpm > wpmMaxPaddle)
    {
      wpm = wpmMaxPaddle;
    }
    if( keyIsForced ) protocol.sendStatus(); 
  }
}

/**
 * Emit morse element based on paddle buffers, starting with DIT, followed by DAH
 * In iambic mode A check paddle release and reset squeeze flag accordingly
 **/
void PaddleInterface::serviceBuffers()
{
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
      morseEngine.sendMorseElement(DIT, true);
    }
    if (buffer & DAH)
    {
      buffer &= ~DAH;
      morseEngine.sendMorseElement(DAH, true);
    }
  }
}

ISR(PADDLE_INT_VECTOR)
{
  if (!paddleBreakIsPending) // only process if the flag is not set yet
  {
    if (protocol.isSendingBuffer()) // only valid if sending from buffer
    {
      paddleBreakIsPending = true;
      paddle.wasTouched = true;
      paddle.buffer = 0;
      PCICR &= ~PADDLE_PCIE_BIT; // disable further interrupt; must be enabled when Protocol starts sending from buffer
      PCIFR |= PADDLE_PCIE_BIT ;
    }
  }
}
