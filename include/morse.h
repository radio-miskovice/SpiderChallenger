// Morse codec engine
#ifndef MORSE_H
#define MORSE_H

#include <Arduino.h>
#include "core.h"

class MorseEngine
{
private:
  static const byte DIT = EMIT_DIT ;
  static const byte DAH = EMIT_DAH ;
  
  byte morseCodeEmitted = 1 ; // collected morse character for morse decoder
  unsigned long lastElementMs = 0 ;
  word wpm ;
  byte wordspaceLU = 3 ;
  byte letterspaceLU = 1 ; 
  byte weightingPct = 50 ;
  bool isSendingBuffer = false ;

public:
  unsigned int asciiToCode(byte ascii); // convert printable ASCII char to morse code
  unsigned int utf8ToCode(byte prefix, byte utf8Char); // convert Ä, Ö, Ü and Cyrillic letters Ч, Ш, Ю and Я
  void sendMorseElement(byte element, bool collect = false ); // emit one morse element (dit or dah, followed by pause)
  void sendMorseCode(word morse_code); // emit one complete morse code character
  void sendAsciiChar(byte ascii); // convert ASCII char to morse code and emit
  void sendString(const char *text);
  //  byte getLastCodeFromPaddle(); // collect last morse char played on paddle
  char decodeKeyedCharacter(); // decode morse character played on paddle
  word getWpm() ;
  void setWpm( word );
  void setWeighting(byte);
  byte getWeighting();
};

extern MorseEngine morseEngine ;

#endif