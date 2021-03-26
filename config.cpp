#include <Arduino.h>
#include "pins.h"
#include "defaults.h"
#include "config.h"

bool configIsDirty = false ;

// CONFIGURATION - these variables will be written to EEPROM
Config config = {
    .paddleMode = 'B',
    .toneManualHz = DEFAULT_TONE_MANUAL_HZ,
    .toneAutoHz   = DEFAULT_TONE_MANUAL_HZ,
    .pttTailMs    = DEFAULT_PTT_TAIL_TIME_MS,
    .pttLeadMs    = DEFAULT_PTT_LEAD_TIME_MS,
    .pttHangTimeWSPct = DEFAULT_PTT_HANG_TIME_WS_PCT,
    .speedMinWpm  = DEFAULT_WPM_LOW_LIMIT,
    .speedMaxWpm  = DEFAULT_WPM_HIGH_LIMIT,
    .paddleTriggerPtt = true,
    .weightingPct = 50,
    .isPaddleSwapped  = false,
    .commandWpm = 25
  };

void saveConfig(bool erase)
{
  byte hdr;
  int ptr = 0;

  if (erase)
  {
    hdr = 0xFF;
    EEPROM.put(hdr, ptr);
  }
  else
  {
    hdr = MAGIC_HEADER;
    EEPROM.put(ptr, hdr);
    ptr += sizeof(hdr);
    EEPROM.put(ptr++, (byte)(CONFIG_VERSION));
    EEPROM.put(ptr, config);
  }
  // Beep R:
  for (char i = 3; i > 0; i--)
  {
    tone(PIN_SIDETONE, 2048);
    delay(i==2 ? 150 : 50);
    noTone(PIN_SIDETONE);
    delay(50);
  }
}

void loadConfig()
{
  byte hdr = 0;
  int ptr = 0;
  EEPROM.get(ptr, hdr);
  ptr += sizeof(hdr);
  if (hdr == MAGIC_HEADER)
  {
    byte configVersion = 0;
    EEPROM.get(ptr++, configVersion);
    if (configVersion >= CONFIG_VERSION)
    {
      EEPROM.get(ptr, config);
    }
    else
    {
      EEPROM.put(0, (byte)0xFF);
    }
  }
  if (hdr != 0xFF)
  {
    EEPROM.put(0, (byte)0xFF); // erase if data do not match
  }
}