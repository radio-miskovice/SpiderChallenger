/* ROTARY ENCODER VARIABLES */
#include <avr/io.h>

extern volatile int8_t encoder_value ;
extern volatile boolean encoder_event_pending;

void rotary_interrupt_enable();
void rotary_interrupt_disable();
void check_rotary_encoder();