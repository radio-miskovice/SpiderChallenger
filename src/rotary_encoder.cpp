/**
 * Rotary encoder variables and handling 
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/
#include <avr/io.h>
#include <Arduino.h>

#include "pins.h"
#include "core.h"
#include "core_variables.h"
#include "rotary_encoder.h"

volatile int8_t encoder_value = 0;
volatile boolean encoder_event_pending = false ;

/* Rotary encoder vector and mask calculation */
#if PIN_ROTARY_CLOCK > 1 && PIN_ROTARY_CLOCK < 8
#define ROT_INT_VECTOR PCINT2_vect
#define ROT_INT_MASK_REG PCMSK2
#define ROT_INT_MASK (1 << (PIN_ROTARY_CLOCK))
#define ROT_PCIE_MASK (1 << PCIE2)
#define ROT_PINS PIND
#define ROT_PC_INTERRUPT

#elif PIN_ROTARY_CLOCK > 7 && PIN_ROTARY_CLOCK < 14
#define ROT_INT_VECTOR PCINT0_vect
#define ROT_INT_MASK_REG PCMSK0
#define ROT_INT_MASK (1 << (PIN_ROTARY_CLOCK - 8))
#define ROT_PCIE_MASK (1 << PCIE0)
#define ROT_PINS PINB
#define ROT_PC_INTERRUPT

#elif PIN_ROTARY_CLOCK > 13 && PIN_ROTARY_CLOCK < 20
#define ROT_INT_VECTOR PCINT1_vect
#define ROT_INT_MASK_REG PCMSK1
#define ROT_INT_MASK (1 << (PIN_ROTARY_CLOCK - 14))
#define ROT_PCIE_MASK (1 << PCIE1)
#define ROT_PINS PINC
#define ROT_PC_INTERRUPT
#else
#error "Cannot determine interrupt configuration for rotary encoder"
#endif

#if PIN_ROTARY_VALUE < 8 
#define ROT_VALUE_SHIFT PIN_ROTARY_VALUE
#elif PIN_ROTARY_VALUE < 14
#define ROT_VALUE_SHIFT PIN_ROTARY_VALUE - 8
#else
#define ROT_VALUE_SHIFT PIN_ROTARY_VALUE - 14
#endif

RotaryEncoder encoder = RotaryEncoder();

RotaryEncoder::RotaryEncoder( byte defaultValue ) {
  value = defaultValue ;
}

void RotaryEncoder::increment( byte n ) { valueIncrement = n ; isEventPending = true; }

void RotaryEncoder::update() {
  if( isEventPending ) {
    PCICR &= ~ROT_PCIE_MASK; // disable rotary interrupt while updating
    hasChanged = !!valueIncrement ;
    value += valueIncrement ;
    valueIncrement = 0 ;
    isEventPending = false ;
    PCICR |= ROT_PCIE_MASK; // enable rotary interrupt
    if( value < minValue ) value = minValue ;
    if( value > maxValue ) value = maxValue ;
  }
}

void RotaryEncoder::init() {
  pinMode( PIN_ROTARY_CLOCK, INPUT_PULLUP );
  pinMode( PIN_ROTARY_VALUE, INPUT_PULLUP );
  enableInterrupt();
}

void RotaryEncoder::enableInterrupt() {
  ROT_INT_MASK_REG = ROT_INT_MASK;
  PCICR |= ROT_PCIE_MASK;
  isEventPending = false;
  valueIncrement = 0;
}

void RotaryEncoder::disableInterrupt()
{
  PCICR &= ~ROT_PCIE_MASK;
  isEventPending = false ;
  valueIncrement = 0;
}

void check_rotary_encoder()
{
  if( encoder.isEventPending ) {
    setSpeed(wpm + encoder_value);
    encoder_value = 0;
    encoder_event_pending = false ;
    speedIsSetManually = true ; 
  }
}

#ifdef ROT_PC_INTERRUPT // if encoder interrupt is on PCINT

// Enables paddle interrupt
void rotary_interrupt_enable()
{
  ROT_INT_MASK_REG = ROT_INT_MASK;
  PCICR |= ROT_PCIE_MASK;
  encoder_event_pending = false ;
}

// Disables paddle interrupt
void rotary_interrupt_disable()
{
  PCICR &= ~ROT_PCIE_MASK;
}

ISR(ROT_INT_VECTOR)
{
  if( !encoder.isEventPending ) {
    byte state = ROT_PINS ;
    if( state & ROT_INT_MASK ) {
      state = (state >> (ROT_VALUE_SHIFT)) & 1 ;
      encoder.increment( 1 - 2*state );
    }
  }
}
#endif // ROT_PC_INTERRUPT
