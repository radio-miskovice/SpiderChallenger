/**
 * Basic PIN logic:
 *    Paddle: must be assignable to the same PCINT vector
 *    Rotary: one of the pins must be assignable to INT0 or INT1
 *    Potentiometer - must be analog PIN, assignable to read AD conversion
 **/

#ifdef _HW_CHALLENGER_V1

// The following configuration is valid for OK1RR TinyKeyer hardware
#define USE_ROTARY_ENCODER
#define USE_BUTTON_DIGITAL

#define PIN_PADDLE_LEFT   2  // D2, PD2, PCINT18, PCI2_vect
#define PIN_PADDLE_RIGHT  3  // D3, PD3, PCINT19, PCI2_vect
#define PIN_ROTARY_CLOCK 10  // D10, PB2, PCINT2, PCI0_vect
#define PIN_ROTARY_VALUE 11  // D11, PB3 -- no interrupt on this pin
#define PIN_ROTARY_SWITCH 7  // D7, PB1, PCINT23,  PCI2_vect
#define PIN_MODE_LED A0
// #define PIN_POTENTIOMETER A0    // must be ADC pin A0-A7
#define PIN_KEY_LINE A2  // preferably D13 to let the diode blink
#define PIN_PTT_LINE A1  //
#define PIN_SIDETONE A5  // sidetone buzzer output


#endif