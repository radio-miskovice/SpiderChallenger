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

void check_rotary_encoder() {
  if( encoder_event_pending ) {
    speed_set(wpm + encoder_value);
    encoder_value = 0;
    encoder_event_pending = false ;
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
  if( !encoder_event_pending ) {
    byte state = ROT_PINS ;
    if( state & ROT_INT_MASK ) {
      state = (state >> (ROT_VALUE_SHIFT)) & 1 ;
      encoder_value = 1 - 2*state ;
      encoder_event_pending = true ;
    }
  }
}
#endif // ROT_PC_INTERRUPT

// class RotaryEncoder {
//   private:
//     char clockPin ;
//     char valuePin ;
//     char switchPin ;
//     bool switchIsAnalog ;
//     unsigned char isrVector ;
//     unsigned char pcieMask ;
//     unsigned char pciMaskRegister ;
//   public:
//     RotaryEncoder(char theClockPin, char theValuePin)
//     {
//       clockPin = theClockPin;
//       valuePin = theValuePin;
//       switchPin = -1;
//     }
//     RotaryEncoder(char theClockPin, char theValuePin, char theSwitchPin, bool analogMode = false)
//     {
//       clockPin = theClockPin;
//       valuePin = theValuePin;
//       switchPin = theSwitchPin;
//       switchIsAnalog = analogMode ;
//     }
//     void setupArduino()
//     {
//       if (clockPin >= 0) pinMode(clockPin, INPUT_PULLUP);
//       if (valuePin >= 0) pinMode(valuePin, INPUT_PULLUP);
//       if (switchPin >= 0) {
//         if( switchIsAnalog ) pinMode(clockPin, INPUT);
//         else pinMode(clockPin, INPUT_PULLUP);
//       }
//     }
//     void setupInterrupt() {

//     }
// };