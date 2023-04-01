#include <Arduino.h>

// #define USE_LGT_EEPROM_API

#include <EEPROM.h>
#include <Arduino.h>
#include "pins.h"
#include "defaults.h"
#include "config.h"

bool configIsDirty = false ;

// CONFIGURATION - these variables will be written to EEPROM
Config config = Config() ;

Config::Config() {
  // first try to read config from EEPROM
  // if not possible, fill default values
  setDefaults();
}

void Config::setDefaults() {
  magicHeader = MAGIC_HEADER;
  version = VERSION;
  toneManualHz = DEFAULT_TONE_MANUAL_HZ;
  toneAutoHz = DEFAULT_TONE_MANUAL_HZ;
  pttTailMs = DEFAULT_PTT_TAIL_TIME_MS;
  pttLeadMs = DEFAULT_PTT_LEAD_TIME_MS;
  pttHangTimeWSPct = DEFAULT_PTT_HANG_TIME_WS_PCT;
  paddleMode = 'B';
  speedMinWpm = DEFAULT_WPM_LOW_LIMIT;
  speedMaxWpm = DEFAULT_WPM_HIGH_LIMIT;
  paddleTriggerPtt = true;
  weightingPct = 50;
  isPaddleSwapped = false;
  commandWpm = 25;
};

word Config::fetchWord() {
  word f = EEPROM.read(eeptr++);
  return (EEPROM.read(eeptr++) + (f<<8));
}

byte Config::fetchByte()
{
  return (EEPROM.read(eeptr++));
}

void Config::putWord(word x)
{
  EEPROM.write(eeptr++, (x >> 8));
  EEPROM.write(eeptr++, x & 0xFF );
}

void Config::putByte(byte b)
{
  EEPROM.write(eeptr++, b);
}
/*
void eeUpdate8( word addr, byte data ) {
  byte d = EEPROM.read(addr);
  if( d != data ) EEPROM.write(addr, data);
}

void eeUpdateBlock( word addr, byte *p, word length ) {
  while( length > 0 ) {
    eeUpdate8( addr, *p );
    p++;
    addr++;
  }
}
*/

#define loadEEWordBE(target, index) \
  target = EEPROM.read(++index);    \
  target = (target << 8) + EEPROM.read(++index);

int Config::getFromEEPROM()
{
  // multibyte values are saved as little endian!
  // EEPROM.begin();
  eeptr = 0 ;
  magicHeader = fetchByte();
  version = fetchByte();
  if( magicHeader != MAGIC_HEADER || version > VERSION ) 
    return 0 ;
  toneManualHz = fetchWord();
  toneAutoHz = fetchWord();
  pttTailMs = fetchWord();
  pttLeadMs = fetchWord();
  pttHangTimeWSPct = fetchWord();
  paddleMode = fetchByte();
  speedMinWpm = fetchByte();
  speedMaxWpm = fetchByte();
  paddleTriggerPtt = (0 != fetchByte());
  weightingPct = fetchByte();
  isPaddleSwapped = (0 != fetchByte());
  commandWpm = fetchByte();
  return eeptr ;
}

void Config::saveToEEPROM()
{
  // multibyte values are saved as little endian!
  // EEPROM.begin();
  eeptr = 0;

  putByte( magicHeader );
  putByte( version );
  putWord( toneManualHz );
  putWord( toneAutoHz );
  putWord( pttTailMs );
  putWord( pttLeadMs );
  putWord( pttHangTimeWSPct );
  putByte( paddleMode );
  putByte( speedMinWpm );
  putByte( speedMaxWpm );
  putByte( paddleTriggerPtt ? 1 : 0);
  putByte( weightingPct );
  putByte( isPaddleSwapped ? 1 : 0 );
  putByte( commandWpm );
}