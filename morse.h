// Morse codec engine
#ifndef MORSE_CODEC_H
#define MORSE_CODEC_H

#include <Arduino.h>
#include "core.h"

class MorseEngine
{
private:
  static const byte DIT = EMIT_DIT ;
  static const byte DAH = EMIT_DAH ;
  static const int DECODER_SIZE = 15 ;
  char decodedString[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  unsigned char morseCodeEmitted = 0 ; // collected morse character for morse decoder
  unsigned char lastMorseCode = 0 ;
  bool letterSpacePending = false ;
  unsigned long lastElementMs = 0 ;
  bool wordSpacePending = false ;

public:
  unsigned int asciiToCode(char ascii); // convert printable ASCII char to morse code
  unsigned int utf8ToCode(unsigned char prefix, unsigned char utf8Char); // convert Ä, Ö, Ü and Cyrillic letters Ч, Ш, Ю and Я
  void sendMorseElement(byte element); // emit one morse element (dit or dah, followed by pause)
  void sendMorseCode(unsigned int morse_code); // emit one complete morse code character
  void sendAsciiChar(byte ascii); // convert ASCII char to morse code and emit
  byte getLastCodeFromPaddle(); // collect last morse char played on paddle
  void decodeKeyedCharacter();  // decode morse character played on paddle
  void appendDecodedCharacter(char);
  char* getDecodedString();
};

extern MorseEngine morseEngine ;

#endif