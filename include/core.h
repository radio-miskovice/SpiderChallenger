/**** Challenger keyer core constants and function declarations ****/

#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define SPIDER_KEYER_VERSION "9.99" /* Dummy Spider Keyer version to prevent reflash  */
#define CHALLENGER_VERSION "1.2.1"  /* Challenger firmware version */

// Shared constants
#define IAMBIC_A 'A'
#define IAMBIC_B 'B'
#define EMIT_NONE 0
#define EMIT_DIT 1
#define EMIT_DAH 2

void setSpeed(uint16_t wpm_set);
void resetInterfaces();

#endif