
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
    if (textLength < 14)  text[textLength++] = c ;
    text[textLength] = 0; 
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
  setSpeed(config.commandWpm) ;
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
  textLength = 0;
  digitalWrite(PIN_MODE_LED, LOW);
  tone(PIN_SIDETONE, 2048);
  delay(125);
  tone(PIN_SIDETONE, 256);
  delay(125);
  noTone(PIN_SIDETONE);
}

/** Check current command and execute if complete **/
void CommandModeInterpreter::service() {
  int w;
  if( isActive )  {
    if( textLength > 0 ) {
      Serial.println(text);
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
        case 'E':
          w = wpmSaved ;
          morseEngine.sendAsciiChar(' ');
          text[2] = w % 10 + '0';
          text[1] = (w / 10) % 10 + '0';
          text[3] = 0;
          morseEngine.sendString( text + 1 );
          morseEngine.sendString( " WPM");
          textLength = 0 ;
          isDone = 0;
          break;
        case 'W': // weighting
          if( textLength >= 3 ) {
            w = atoi( text+1 );
            if( w>10 && w <90 ) { 
              config.weightingPct = w ;
              isDone = 1;
            }
            else { isDone = 2 ; } // out of range
          } else { isDone = 0 ;}  // not a complete number yet
          break ;
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
