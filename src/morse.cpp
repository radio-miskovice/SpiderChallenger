/**
 * Morse code converter and keying processor
 * Loosely based on code developed by Goody, K3NG and Petr, OK1FIG
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 * Jindrich Vavruska, jindrich@vavruska.cz
 **/
#include <Arduino.h>

#include "pins.h"
#include "core_variables.h" // need sending_mode
#include "core.h"           // need control_element_duration()
#include "config.h"
#include "protocol.h"
#include "paddle_interface.h"
#include "keyer_interface.h"
#include "command_mode.h"
#include "morse.h"

MorseEngine morseEngine = MorseEngine();

/** MORSE CODE CONVERSION TABLE
 * How to read code:
 * Each morse code character is binary encoded. It is interpreted from MSB to LSB
 * (MSB - first dit or dash) 0 = dit, 1 = dash.
 *
 * The least significant bit 1 in the code is end marker ("stop bit") => 0x80 means nothing to send.
 * 0x00 represents NULL, or invalid morse code.
 * Code is interpreted by reading MSB, then shift left. Width must be limited to 8 bits! This is different
 * from "inverse" binary encoding using shift right, which ends with 0x01 or 0x00 regardless of data width.
 *
 * Maximum possible code length is hence 7 elements.
 * This code table is compatible with K3NG (Winkeyer 2 protocol?), includes some rarely used characters
 * and prosigns (+ = AR, & = AS, * = BK, '(' = KN, > = SK)
 *
 * characters [, ], \ are encoded as 0 (no prosign or special code) and they are reserved for use
 * in text-mode commands in serial link protocol. However, this module has nothing to do with the protocol,
 * it only converts ASCII to morse code and processes morse code.
 *
 */
const byte CODE[] = {
    //-- special characters --//

    0x80,        // space; will send 3U letter pause
    0b11001110,  // !
    0b01001100,  // " AU
    0,           // #
    0b00010011,  // $ VU
    0,           // %
    0b01000100,  // & AS
    0b01111010,  // ' JN
    0b10110100,  // ( KN
    0b10110110,  // ) KK
    0b10001011,  // * BK
    0b01010100,  // + AR
    0b11001110,  // , = ! --..--
    0b10000110,  // -
    0b01010110,  // .
    0b10010100,  // stroke /    //--- numbers --//
    0b11111100,  // 0
    0b01111100,  // 1
    0b00111100,  // 2
    0b00011100,  // 3
    0b00001100,  // 4
    0b00000100,  // 5
    0b10000100,  // 6
    0b11000100,  // 7
    0b11100100,  // 8
    0b11110100,  // 9    //--- punctuation ---//
    0b11100010,  // : OS
    0b10101010,  // ; NC
    0b01010100,  // < = + = AR
    0b10001100,  // = BT
    0b00010110,  // > SK
    0b00110010,  // ?
    0b01101010,  // @ AC
    0b01100000,  // A
    0b10001000,  // B
    0b10101000,  // C
    0b10010000,  // D
    0b01000000,  // E
    0b00101000,  // F
    0b11010000,  // G
    0b00001000,  // H
    0b00100000,  // I
    0b01111000,  // J
    0b10110000,  // K
    0b01001000,  // L
    0b11100000,  // M
    0b10100000,  // N
    0b11110000,  // O
    0b01101000,  // P
    0b11011000,  // Q
    0b01010000,  // R
    0b00010000,  // S
    0b11000000,  // T
    0b00110000,  // U
    0b00011000,  // V
    0b01110000,  // W
    0b10011000,  // X
    0b10111000,  // Y
    0b11001000,  // Z
    0,           // [
    0,           // backslash
    0,           // ]
    0,           // ^ caret
    0b001101100, // _ underscore UK
};

unsigned long last_element_ms = 0 ;


/**
 * @param ascii ASCII letter to be converted
 * @return zero if ascii is not defined in Morse code, otherwise returns binary morse code
 */
unsigned int MorseEngine::asciiToCode(byte ascii)
{
  if (ascii < 0x20 || ascii >= 0x7B) return 0;
  if (ascii == '|') return 0x180; // half space
  if (ascii > 0x60) ascii -= 0x20;
  ascii -= 0x20;
  return (CODE[ascii]);
}

/**
 * Converts selected UTF-8 characters to morse code
 * Valid codes only for German letters ä, ö, ü and Russian letters ш, ч, ю, я
 *
 * @param prefix UTF-8 code page prefix. Only Latin-1 and Cyrillic prefixes are accepted.
 * @param utf8Char the second UTF-8 byte. Only German and Russian letters specified above are accepted.
 * @return unsigned char binary morse code for accepted letters, otherwise returns zero (invalid morse code)
 *
 * Latin-1 letters
 * Ä U+00C4  ä U+00E4  C384, C3A4 : morse code .-.- (binary 0b01011000, hex 0x58)
 * Ö U+00D6  ö U+00F6  C396, C3B6 : morse code ---. (binary 0b11101000, hex 0xE8)
 * Ü U+00DC  ü U+00FC  C39C, C3BC : morse code ..-- (binary 0b00111000, hex 0x38)
 *
 * Cyrillic letters
 * Ч U+0427  ч U+0447  D0A7, D0C7 : morse code ---. (binary 0b11101000, hex 0xE8)
 * Ш U+0428  ш U+0448  D0A8, D0C8 : morse code ---- (binary 0b11111000, hex 0xF8)
 * Ю U+042E  ю U+044E  D0AE, D0CE : morse code ..-- (binary 0b00111000, hex 0x38)
 * Я U+042F  я U+044F  D0AF, D0CF : morse code .-.- (binary 0b01011000, hex 0x58)
 */
unsigned int MorseEngine::utf8ToCode(byte prefix, byte utf8Char)
{
  unsigned int utf8Code = 256 * prefix + utf8Char;
  switch (utf8Code) {
    case 0xC384:
    case 0xC3A4:
    case 0xD0AF:
    case 0xD0CF:
      return 0x58;

    case 0xC396:
    case 0xC3B6:
    case 0xD0A7:
    case 0xD0C7:
      return 0xE8;

    case 0xC39C:
    case 0xC3BC:
    case 0xD0AE:
    case 0xD0CE:
      return 0x38;

    case 0xD0A8:
    case 0xD0C8:
      return 0xF8;
  }
  return 0;
}

/** Send DIT & send DAH in a single function and using only integer arithmetic
 * @param element DIT or 1 = dit, anything else = dah
 * @param collect (optional, default false) if true, element will be collected for subsequent character decode
 **/
void MorseEngine::sendMorseElement(byte element, bool collect) {
  unsigned int elementLength;
  unsigned int totalLength;

  if ( collect && morseCodeEmitted == 0) { morseCodeEmitted = 0x01; } // set start bit
  if (element == DIT) // DIT
  {
    keyerInterface.currentlyEmitting = EMIT_DIT;
    elementLength = 100 * config.weightingPct / 50;
    totalLength = 200;
    if (collect)
    {
      morseCodeEmitted *= 2;
    } // shift morse code for decoder, LSB set to 0 (dit)
  }
  else if(element == DAH) {
    keyerInterface.currentlyEmitting = EMIT_DAH;
    elementLength = 6 * config.weightingPct ; // dah:dit = 300, 300 / 50 = 6
    totalLength = 400;
    if (collect)
    {
      morseCodeEmitted = (morseCodeEmitted * 2) | 0x01; // // shift morse code for decoder, LSB set to 1 (dah)
    }
  }
  keyerInterface.setKey( HIGH ); // ex: key(1)
  keyerInterface.holdElementDuration(elementLength, wpm);
  keyerInterface.setKey(LOW); // key(0);
  keyerInterface.holdElementDuration(totalLength - elementLength, wpm);
  keyerInterface.currentlyEmitting = EMIT_NONE;
  lastElementMs = millis();
  // letterspace_pending = true ;
}

/**
 * Send morse code by reading binary bits and sending dits and dahs
 * */
void MorseEngine::sendMorseCode(word morse_code)
{
  byte next;
  byte code = morse_code & 0xFF;
  isSendingBuffer = 1 ;
  if (morse_code == 0) // invalid morse code - do nothing
    return;
  if (code == 0x80) // special morse code "space" or "half space" 0x0180
  {
    morse_code = morse_code >> 8;
    // half letter space
    if (morse_code == 1)
      keyerInterface.holdElementDuration(50, wpm);
    else
      keyerInterface.holdElementDuration(100 * (wordspaceLU - letterspaceLU - 2), wpm);
  }
  while (code != 0x80)
  {
    next = code & 0x80;
    sendMorseElement(next == 0 ? EMIT_DIT : EMIT_DAH ); // any value next > 0 counts as dash
    code *= 2;
  }
  // finally add letterspace
  keyerInterface.holdElementDuration(100 * (letterspaceLU - 1), wpm);
  isSendingBuffer = 0;
}

void MorseEngine::sendString(const char* text)
{
  while( (*text) != 0 ) {
    sendAsciiChar( *text );
    text++ ;
  }
}

/**
 * @param ascii ASCII character to be sent
 *
 * If the ascii has no morse code, sending mode is not changed and nothing happens
 **/
void MorseEngine::sendAsciiChar(byte ascii)
{
  unsigned int morse_code = asciiToCode(ascii);
  if (morse_code)
  {
    sendMorseCode(morse_code);
  }
  else
    keyerInterface.holdElementDuration(50, wpm);
}

/**
 * Decodes morse code collected from paddles. Collected code has high stop bit
 * and LSB last morse code element, LSB-aligned. Therefore it must be shifted in order
 * to match morse codes in conversion table, MSB-aligned and the stop bit must be added
 * after the lowest morse code bit.
 * This method is called in holdElementDuration because there's plenty of time to complete.
 **/
char MorseEngine::decodeKeyedCharacter()
{
  unsigned long elapsed = lastElementMs - millis();
  /* if time elapsed since last element + element space > 1.5 unit, convert character
    * the actual conversion takes place both paddles are up because there is
    * plenty of time to complete the transformation
    **/
  if( morseCodeEmitted == 0x01 ) {
    lastElementMs = millis();
    return 0;
  }
  else if (elapsed * wpm / 1200 <= 250) return 0 ; // nothing was keyed yet
  /* phase 1 - bit realignment */
  unsigned int target = morseCodeEmitted * 0x100 + 0x80;
  while (target & 0xFE00)              // shift right one bit until all significant bits of the source disappear
    target /= 2;
  unsigned char morse = target & 0xFF; // mask off the leftover source stop bit
  /* phase 2 - morse code lookup in code table */
  if (morse == 0x80) return( ' ' ); // unlikely to happen;
  unsigned int size = sizeof(CODE) / sizeof(CODE[0]);
  // look up morse code
  morseCodeEmitted = 0x01;          // prepare empty morse code for new collection
  lastElementMs = millis();
  for (unsigned char result = 0; result < size; result++)
  {
    if (morse == CODE[result]) {
      return (result + 0x20);
    }
  }
  return 0 ;
}
