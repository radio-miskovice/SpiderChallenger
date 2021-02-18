/**
 * Challenger keyer configuration persistence
 * Derived from original code by Petr Maly, OK1FIG
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/
#include <avr/io.h>
#include <EEPROM.h>
#include "pins.h"
#include "core.h"
#include "core_variables.h"
#include "eeprom_config.h"

void save_config(bool erase)
{
  uint16_t hdr;
  uint8_t  ptr = 0;

  if (erase)
  {
    hdr = 65535;
    EEPROM_PUT_NEXT(hdr, ptr);
  }
  else
  {
    hdr = MY_MAGIC_HEADER;
    // Write a header to recognize that EEPROM was written:
    EEPROM_PUT_NEXT(hdr, ptr);
    EEPROM_PUT_NEXT(keyer_mode, ptr);
    EEPROM_PUT_NEXT(sidetone_hz_man, ptr);
    EEPROM_PUT_NEXT(sidetone_hz_auto, ptr);
    EEPROM_PUT_NEXT(ptt_tail_time, ptr);
    EEPROM_PUT_NEXT(ptt_lead_time, ptr);
    EEPROM_PUT_NEXT(ptt_hang_time_wsu_pct, ptr);
    EEPROM_PUT_NEXT(wpm_limit_low, ptr);
    EEPROM_PUT_NEXT(wpm_limit_high, ptr);
    EEPROM_PUT_NEXT(paddles_trigger_ptt, ptr);
    EEPROM_PUT_NEXT(weighting_pct, ptr);
    EEPROM_PUT_NEXT(paddles_swapped, ptr);
  }
  // Beep R:
  for( char i = 2; i > 0;  i--) {
    tone(PIN_SIDETONE, 400);
    delay(50 * (1 + 2*(i&1)));
    noTone(PIN_SIDETONE);
    delay(50);
  }
}

void load_config()
{
  unsigned int hdr = 0;
  int ptr = 0;
  EEPROM_GET_NEXT(hdr, ptr);

  if (hdr == MY_MAGIC_HEADER)
  { // The EEPROM was already written
    // Read data:
    EEPROM_GET_NEXT(keyer_mode, ptr);
    EEPROM_GET_NEXT(sidetone_hz_man, ptr);
    EEPROM_GET_NEXT(sidetone_hz_auto, ptr);
    EEPROM_GET_NEXT(ptt_tail_time, ptr);
    EEPROM_GET_NEXT(ptt_lead_time, ptr);
    EEPROM_GET_NEXT(ptt_hang_time_wsu_pct, ptr);
    EEPROM_GET_NEXT(wpm_limit_low, ptr);
    EEPROM_GET_NEXT(wpm_limit_high, ptr);
    EEPROM_GET_NEXT(paddles_trigger_ptt, ptr);
    EEPROM_GET_NEXT(weighting_pct, ptr);
    EEPROM_GET_NEXT(paddles_swapped, ptr);
  }
}
