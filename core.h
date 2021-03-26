/**** Challenger keyer core constants and function declarations ****/

#ifndef CORE_H
#define CORE_H

#include <stdint.h>

#define SPIDER_KEYER_VERSION "1.37" /* Spider Keyer 2020-12-20 */
#define CHALLENGER_VERSION "1.2.0"  /* Challenger firmware version */

// Shared constants
#define IAMBIC_A 'A'
#define IAMBIC_B 'B'
#define EMIT_NONE 0
#define EMIT_DIT 1
#define EMIT_DAH 2

void setSpeed(word wpm_set);
void resetInterfaces();

#endif