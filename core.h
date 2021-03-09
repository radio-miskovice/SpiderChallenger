/**** Challenger keyer core constants and function declarations ****/

#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define VERSION "1.31" // Dec 2020

// Shared constants
#define IAMBIC_A 'A'
#define IAMBIC_B 'B'
#define EMIT_NONE 0
#define EMIT_DIT 1
#define EMIT_DAH 2
#define SEND_MODE_UNDEF   0 // mode unknown
#define SEND_MODE_AUTO    1 // automatic mode 
#define SEND_MODE_PADDLE  2 // manual mode (nothing in buffer)

#define FLAG_ENABLE_PTT 0  // ptt control enabled
#define FLAG_ENABLE_KEY 1  // keying enabled
#define FLAG_ENABLE_SPEED_CTL 2  // set speed by manual control enabled
#define FEATURE_ENABLED( features, bit ) \
   ((features) & (1 << (bit)) >> (bit))

// Buffer and memory handling
// void send_message();
void store_message(byte data);

// Speed control
void setSpeed(int wpm_set);

// Serial communication
// void check_delayed_send_response();

// Command processor
void resetInterfaces();

#endif