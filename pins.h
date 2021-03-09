// Keyer PIN configuration

/**
 * Basic PIN logic:
 *    Paddle: must be assignable to the same PCINT vector
 *    Rotary: one of the pins must be assignable to INT0 or INT1
 *    Potentiometer - must be analog PIN, assignable to read AD conversion
 **/
#define PIN_PADDLE_LEFT   2     // D5, PD5, PCINT21, PCI2_vect
#define PIN_PADDLE_RIGHT  5     // D2, PD2, PCINT18, PCI2_vect
#define PIN_ROTARY_CLOCK  10    // D3, PD3, INT1
#define PIN_ROTARY_VALUE  11    // D4, PD4
#define PIN_ROTARY_SWITCH A1    // A1 for analog reading
#define PIN_MODE_LED      9
// #define PIN_POTENTIOMETER A0    // must be ADC pin A0-A7
#define PIN_KEY_LINE       13    // preferably D13 to let the diode blink
#define PIN_PTT_LINE       0     //
#define PIN_SIDETONE      4     // sidetone buzzer output

#define USE_BUTTON_ANALOG

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
