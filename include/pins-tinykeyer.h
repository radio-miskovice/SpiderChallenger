/**
 * Basic PIN logic:
 *    Paddle: must be assignable to the same PCINT vector
 *    Rotary: one of the pins must be assignable to INT0 or INT1
 *    Potentiometer - must be analog PIN, assignable to read AD conversion
 **/

#ifdef _HW_TINYKEYER_OK1RR

// The following configuration is valid for OK1RR TinyKeyer hardware
#define USE_ROTARY_ENCODER
#define USE_BUTTON_ANALOG

#define PIN_PADDLE_LEFT 2    // D5, PD5, PCINT21, PCI2_vect
#define PIN_PADDLE_RIGHT 5   // D2, PD2, PCINT18, PCI2_vect
#define PIN_ROTARY_CLOCK 10  // D3, PD3, INT1
#define PIN_ROTARY_VALUE 11  // D4, PD4
#define PIN_ROTARY_SWITCH A1 // A1 for analog reading
#define PIN_MODE_LED 9
// #define PIN_POTENTIOMETER A0    // must be ADC pin A0-A7
#define PIN_KEY_LINE 13 // preferably D13 to let the diode blink
#define PIN_PTT_LINE 0  //
#define PIN_SIDETONE 4  // sidetone buzzer output


#endif