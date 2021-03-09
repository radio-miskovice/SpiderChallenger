#ifndef CONFIG_H
#define CONFIG_H

/* CONFIGURATION */

#define CONFIG_VERSION 2

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