#ifndef CONFIG_H
#define CONFIG_H

#include <EEPROM.h>

/* CONFIGURATION */
#define MAGIC_HEADER   0xCF 
#define CONFIG_VERSION 2

void saveConfig(bool erase);
void loadConfig();

struct Config
{
  char paddleMode ;
  unsigned int  toneManualHz ;
  unsigned int  toneAutoHz ;
  unsigned int  pttTailMs ;
  unsigned int  pttLeadMs ;
  unsigned int  pttHangTimeWSPct ;
  unsigned char speedMinWpm ;
  unsigned char speedMaxWpm ;
  bool paddleTriggerPtt ;
  byte weightingPct ;
  bool isPaddleSwapped ;
};

extern bool configIsDirty ;
extern Config config ;

#endif