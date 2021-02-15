/* ROTARY ENCODER VARIABLES */
#include <avr/io.h>

extern volatile int8_t rot_value ;
extern volatile boolean rot_event_pending;

void rotary_interrupt_enable();
void rotary_interrupt_disable();
void check_rotary_encoder();