#ifndef CONFIG_H
#define CONFIG_H

#include <EEPROM.h>
#include <Arduino.h>

/* CONFIGURATION */
#define MAGIC_HEADER   0xCF
#define VERSION 0x01

class Config
{
  private:
    int eeptr = 0;
    word fetchWord();
    byte fetchByte();
    void putWord(word x);
    void putByte(byte x);

  public:
    byte magicHeader ;
    byte version ;
    word toneManualHz ;
    word toneAutoHz ;
    word pttTailMs ;
    word pttLeadMs ;
    word pttHangTimeWSPct ;
    char paddleMode ;
    byte speedMinWpm ;
    byte speedMaxWpm ;
    bool paddleTriggerPtt ;
    byte weightingPct ;
    bool isPaddleSwapped ;
    byte commandWpm ;
    Config();
    void saveToEEPROM();
    int getFromEEPROM();
    void setDefaults();
};

extern bool configIsDirty ;
extern Config config ;

#endif