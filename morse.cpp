
/** MORSE CODE CONVERSION TABLE 
 * How to read code:
 * Each code is binary encoded. It is interpreted from LSB to MSB (LSB - first dit or dash)
 * 0 = dit, 1 = dash
 * The most significant 1 in the code is end marker => 0x01 means nothing to send.
 * 0x00 represents NULL or invalid morse code.
 * Code is interpreted by reading LSB, then shift right
 * If the code is zero after shift right, it is end of character. Otherwise what was the LSB is sent.
 * Maximum code length is 7 elements.
 * This code table is compatible with Winkeyer 2 protocol, includes some rarely used characters
 * and prosigns (+ = AR, & = AS, * = BK, '(' = KN, > = SK)
 */
const unsigned char CODE[] = {
    //-- special characters --//
    0x01, // space; will send 3U letter pause
    0x73, // !
    0x52, // " AU
    0,    // #
    0xC8, // $ VU
    0,    // %
    0x22, // & AS
    0x5E, // ' JN
    0x2D, // ( KN
    0x6D, // ) KK
    0xD1, // * BK
    0x2A, // + AR
    0x73, // , = ! --..--
    0x61, // -
    0x6A, // .
    0x29, // stroke /
    //--- numbers --//
    0x3F, // 0
    0x3E, // 1
    0x3C, // 2
    0x38, // 3
    0x30, // 4
    0x20, // 5
    0x21, // 6
    0x23, // 7
    0x27, // 8
    0x2F, // 9
    //--- punctuation ---//
    0x47, // : OS
    0x55, // ; NC
    0x2A, // < = + = AR
    0x31, // = BT
    0x68, // > SK
    0x4C, // ?
    0x56, // @
    0x06, // A
    0x11, // B
    0x15, // C
    0x09, // D
    0x02, // E
    0x14, // F
    0xB,  // G
    0x10, // H
    0x04, // I
    0x1E, // J
    0x0D, // K
    0x12, // L
    0x07, // M
    0x05, // N
    0x0F, // O
    0x16, // P
    0x1B, // Q
    0x0A, // R
    0x08, // S
    0x03, // T
    0x0C, // U
    0x18, // V
    0x0E, // W
    0x19, // X
    0x1D, // Y
    0x13, // Z
    0x0, // [
    0x0, // backslash
    0x0, // ]
    0x0, // ^ caret
    0x6C, // _ UK underscore 
};

/**
 * @param ascii ASCII letter to be converted 
 * @return zero if ascii is not defined in Morse code, otherwise returns binary morse code
 */
unsigned char ascii2code(char ascii)
{
  if (ascii < 0x20 || ascii >= 0x7B )
    return 0;
  if( ascii > 0x60 ) ascii -= 0x20 ;
  ascii -= 0x20 ;
  return( CODE[ ascii ] );
}

/**
 * Converts selected UTF-8 characters to morse code
 * Valid codes only for German letters ä, ö, ü and Russian letters ш, ч, ю, я 
 * 
 * @param prefix UTF-8 code page prefix. Only Latin-1 and Cyrillic prefixes are accepted.
 * @param utf8char the second UTF-8 byte. Only German and Russian letters specified above are accepted.
 * @return unsigned char binary morse code for accepted letters, otherwise returns zero (invalid morse code)
 * 
 * Latin-1 letters 
 * Ä U+00C4  ä U+00E4  morse code .-.- (binary 11010, hex 0x16)
 * Ö U+00D6  ö U+00F6  morse code ---. (binary 10111, hex 0x17)
 * Ü U+00DC  ü U+00FC  morse code ..-- (binary 11100, hex 0x1C)
 * 
 * Cyrillic letters
 * Ч U+0427  ч U+0447  morse code ---. (binary 10111, hex 0x17)
 * Ш U+0428  ш U+0448  morse code ---- (binary 11111, hex 0x1F)
 * Ю U+042E  ю U+044E  morse code ..-- (binary 11100, hex 0x1C)
 * Я U+042F  я U+044F  morse code .-.- (binary 11010, hex 0x16)
 */

unsigned char utf8_to_code(unsigned char prefix, unsigned char utf8char) {
  return 0 ;
}