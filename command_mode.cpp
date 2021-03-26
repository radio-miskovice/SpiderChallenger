
#include <Arduino.h>
#include "pins.h"
#include "config.h"
#include "core_variables.h"
#include "morse.h"
#include "command_mode.h"

CommandModeInterpreter commandMode;

/**
 * Append character to command buffer
 * @param c character to be appended 
 **/
void CommandModeInterpreter::append(char c) {
  if( c!=0 && isActive ) { 
    if (textLength < 15)  text[textLength++] = c ;
  }
}

/** Setup necessary pins  **/
void CommandModeInterpreter::setup() {
  isActive = false;
  textLength = 0;
  pinMode(PIN_MODE_LED, OUTPUT);
  digitalWrite(PIN_MODE_LED, LOW);
}

/** Initialize command mode **/
void CommandModeInterpreter::init() { 
  isActive = true;  
  textLength = 0 ; 
  wpmSaved = wpm;
  wpm = config.commandWpm ;
  digitalWrite(PIN_MODE_LED, HIGH);
  tone(PIN_SIDETONE, 256);
  delay(125);
  tone(PIN_SIDETONE, 2048);
  delay(125);
  noTone(PIN_SIDETONE);
}

/** Terminate command mode **/
void CommandModeInterpreter::exit() { 
  isActive = false;
  if( config.commandWpm != wpm ) {
    configIsDirty = true ;
    config.commandWpm = wpm ;
  }
  wpm = wpmSaved ;
  digitalWrite(PIN_MODE_LED, LOW);
  tone(PIN_SIDETONE, 2048);
  delay(125);
  tone(PIN_SIDETONE, 256);
  delay(125);
  noTone(PIN_SIDETONE);
}

/** Check current command and execute if complete **/
void CommandModeInterpreter::service() {
  if( isActive )  {
    if( textLength > 0 ) {
      byte isDone = 0 ;
      switch( text[0] ) {
        case 'A' : 
        case 'B' :
          config.paddleMode = text[0] ;
          isDone = 1 ;
          break ;
        case 'N':
          config.isPaddleSwapped = !config.isPaddleSwapped;
          isDone = 1 ;
          break;
        default:
          isDone = 2 ;
      }
      if( isDone > 0 ) {
        morseEngine.sendAsciiChar(' ');
        morseEngine.sendAsciiChar(isDone == 1 ? 'R' : '?');
        textLength = 0 ;
      }
    }
  }
}
