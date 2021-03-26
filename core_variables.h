#ifndef CORE_VARIABLES_H
#define CORE_VARIABLES_H

#include <Arduino.h>

/* SPEED */
extern unsigned int wpm;   // CW speed
extern unsigned int wpmPrevious; // CW speed auto send
extern unsigned int wpmMaxPaddle; // Speed limit for manual sending, in wpm. 0: switched off
extern bool speedIsSetManually;    // true if manual speed control is enabled (rotary or potentiometer)
extern bool speedManualSetEnabled;

/* KEYER MODE AND KEYING PARAMETERS */

extern byte wordspaceLU;
extern byte letterspaceLU;

#endif