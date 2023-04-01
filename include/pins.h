// Keyer PIN configuration

#if defined(_HW_TINYKEYER_OK1RR)
  #include "pins-tinykeyer.h"
#elif defined(_HW_CHALLENGER_V1)
  #include "pins-challenger-v1.h"
#elif defined(_HW_SPIDERKEYER_V1)
  #include "pins-spider-v1.h"
#else
  #error "No hardware definition"
#endif




// DO NOT EDIT AFTER THIS LINE!!!

/** Paddle interrupt vector and mask calculations 
 * PADDLE_INT_VECTOR ..... PCINTx vector to use for interrupt
 * PADDLE_INT_MASK_REG ... PCIMSKx register to use for paddle interrupt
 * PADDLE_LEFT_BIT ... ... which bit is PIN_PADDLE_LEFT in PCIMSK and PINx
 * PADDLE_RIGHT_BIT .. ... which bit is PADDLE_RIGHT in PCIMSK and PINx
 * PADDLE_INT_MASK ... ... mask to apply to PCIMSKx register
 * PADDLE_PCIE_MASK .. ... enable bit to apply to PCICR register
 * PADDLE_PINS ... ... ... PINx register to read both paddles in one op
 */

#if PIN_ROTARY_SWITCH > 1 && PIN_ROTARY_SWITCH < 8
  #define ROT_SW_INT_VECTOR PCINT2_vect
  #define ROT_SW_INT_MASK_REG PCMSK2
  #define ROT_SW_INT_MASK     (1 << (PIN_ROTARY_SWITCH) )
  #define ROT_SW_PCIE_MASK (1 << PCIE2)
  #define ROT_SW_PC_INTERRUPT
#elif PIN_ROTARY_SWITCH > 7 && PIN_ROTARY_SWITCH < 14
  #define ROT_SW_INT_VECTOR PCINT0_vect
  #define ROT_SW_INT_MASK_REG PCMSK0
  #define ROT_SW_INT_MASK (1 << (PIN_ROTARY_SWITCH - 8))
  #define ROT_SW_PCIE_MASK (1 << PCIE0)
  #define ROT_SW_PC_INTERRUPT
#elif PIN_ROTARY_SWITCH > 13 && PIN_ROTARY_SWITCH < 20
  #define ROT_SW_INT_VECTOR PCINT1_vect
  #define ROT_SW_INT_MASK_REG PCMSK1
  #define ROT_SW_INT_MASK (1 << (PIN_ROTARY_SWITCH - 14))
  #define ROT_SW_PCIE_MASK (1 << PCIE1)
  #define ROT_SW_PC_INTERRUPT
#endif
