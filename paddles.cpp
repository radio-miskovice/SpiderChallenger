#include <avr/io.h>
#include <Arduino.h>

#include "pins.h"
#include "core.h"
#include "core_variables.h"

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

volatile bool paddles_touched ;
volatile byte paddle_status ;

// Enables paddle interrupt
void enable_paddle_int()
{
  PADDLE_INT_MASK_REG = PADDLE_INT_MASK;
  PCICR |= PADDLE_PCIE_BIT;
}

// Disables paddle interrupt
void disable_paddle_int()
{
  PCICR &= ~(PADDLE_PCIE_BIT);
}

ISR(PADDLE_INT_VECTOR)
{
  if(!paddles_touched) { // debounce check
    paddles_touched = true;
    paddle_status = PADDLE_PINS & PADDLE_INT_MASK;
    if (sending_mode != SEND_MODE_PADDLE)
    {
      sending_mode = SEND_MODE_PADDLE;
      sending_mode_last = SEND_MODE_PADDLE;
      serial_buffer_head = 0;
      serial_buffer_tail = 0;
    }
  }
}

