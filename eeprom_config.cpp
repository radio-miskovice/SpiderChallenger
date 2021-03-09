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
// #include "paddles.h"
#include "eeprom_config.h"
#include "config.h"

#define MY_MAGIC_HEADER 0xCCCC // binary 1010 1010 1010 1010

void save_config(bool erase)
{
  uint16_t hdr;
  int ptr = 0;

  if (erase)
  {
    hdr = 0xFFFF;
    EEPROM.put(hdr, ptr);
  }
  else
  {
    hdr = MY_MAGIC_HEADER;
    EEPROM.put(ptr, hdr);
    ptr += sizeof(hdr);
    EEPROM.put(ptr++, (unsigned char)(CONFIG_VERSION));
    EEPROM.put(ptr, config);
  }
  // Beep R:
  for (char i = 2; i > 0; i--)
  {
    tone(PIN_SIDETONE, 400);
    delay(50 * (1 + 2 * (i & 1)));
    noTone(PIN_SIDETONE);
    delay(50);
  }
}

void load_config()
{
  unsigned int hdr = 0;
  int ptr = 0;
  EEPROM.get( ptr, hdr );
  ptr += sizeof(hdr);
  if (hdr == MY_MAGIC_HEADER)
  {
    unsigned char config_version = 0;
    EEPROM.get(ptr++, config_version);
    if (config_version == CONFIG_VERSION)
    {
      // The EEPROM was already written
      // Read data:
      EEPROM.get(ptr, config);
    }
  }
}
