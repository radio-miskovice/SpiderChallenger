/*  Spider Challenger keyer software
    Version 1 (2021-03-08) by Jindra, OK4RM
    CC-BY-NC 4.0

    Inspired by Spider Keyer (version 1.xx) by Petr OK1FIG,
    Intentionally to be compatible with Spider Keyer protocol.

*/
#include <Arduino.h>
#include "features.h"       // define features to use
#include "pins.h"           // define pin layout of the target hardware
#include "core.h"           // shared defines
#include "config.h"         // config structure; TODO: merge with eeprom_config

#include "morse.h"          // morse codec engine and timing control
#include "protocol.h"       // protocol handler with integrated send buffer and command processor (Spider Protocol v. 2)
#include "command_mode.h"   // command mode interpreter

// HW interfaces
#include "paddle_interface.h" // paddle interface (port and interrupt definitions and handling)
#include "keyer_interface.h"  // keyer & PTT interface (port definitions and handling)
#include "speed_control.h"    // polymorphic speed control (potentiometer or rotary encoder)
#include "mode_button.h"      // TODO: create polymorphic button handler
// #include "powerbank.h"     // powerbank dummy load management



/* SPEED SETTINGS */
word wpm = 25;        // current speed in WPM
word wpmPrevious = 25 ; // speed set manually before changed by buffered command  
word wpmMaxPaddle = 0;  // maximum speed for paddle keying in WPM. 0: switched off
bool speedIsSetManually = true; // true if manual speed control is enabled (rotary or potentiometer)
bool speedManualSetEnabled = true ;

/* KEYER MODE AND KEYING PARAMETERS */

byte wordspaceLU = 7;
byte letterspaceLU = 3;
byte isSendingBuffer = 0;

/* SENDING MODE */
// bool isCommandMode = false;

long btn_toggle; // Debounce the message sending button
unsigned long last_powerbank;

/**
 * Initial setup 
 */
void setup()
{
  Serial.begin(57600); // primary_serial_port_baud_rate
  #ifdef _HW_CHALLENGER_V1
  Serial.println("Challenger HW");
  #endif
  // Serial.println("Challenger.");
  // unsigned long initTime = millis();
  // command mode LED
  commandMode.setup();
  paddle.setup();
  keyerInterface.setup();
  // powerBank.setup();
  // KEY line
  
  pinMode(PIN_SIDETONE, OUTPUT);
  digitalWrite(PIN_SIDETONE, LOW);
  digitalWrite(PIN_MODE_LED, HIGH);
  // loadConfig();
  speedControl->init();
  speedControl->setMaxValue( config.speedMaxWpm );
  speedControl->setMinValue( config.speedMinWpm );
  setSpeed( speedControl->getValue() ); // in case of potentiometer, read potentiometer, otherwise use initial values
  resetInterfaces();
  // Serial.print("Setup completed in ");
  // Serial.print( millis() - initTime );
  // Serial.println(" ms");
  // digitalWrite(PIN)
  tone(PIN_SIDETONE, 512);
  delay(100);
  tone(PIN_SIDETONE, 1024);
  delay(100);
  noTone(PIN_SIDETONE);
  digitalWrite(PIN_MODE_LED, LOW);
  morseEngine.sendString("HI");
}

/* Main loop */
void loop()
{
  paddle.handleInterrupt(); // handle paddle break if occurred
  paddle.check(paddle.DIT); // check paddle state
  paddle.check(paddle.DAH); // check paddle state
  paddle.serviceBuffers();  // handle paddle memory accordingly
  
  protocol.checkInput();    // check input stream and handle incoming data if necessary
  paddle.serviceBuffers();  // again handle paddle memory
  protocol.serviceSendBuffer();   // handle any data in send buffer
  keyerInterface.checkPttTail();  // turn off PTT after PTT tail time elapsed after last key up
  speedControl->update();   // update speed control values
  if( speedManualSetEnabled && speedControl->hasChanged ) {
    setSpeed( speedControl->getValue() );
  }
  checkAnalogButton();
  serviceCommandButton();
  commandMode.append(morseEngine.decodeKeyedCharacter()) ;
  commandMode.service();
}

/* RESET ALL CONTROLS AND OUTPUTS */
void resetInterfaces()
{
  /* Makes a total reset, puts all to inactive and resets the status flags. Should be called
    in need to fetch the device to a "defined state". */
  noTone(PIN_SIDETONE);
  noInterrupts();
  keyerInterface.forcePtt( false ); // release forced PTT if necessary; set PTT off; set pttTime = 0
  keyerInterface.setPtt( LOW );
  keyerInterface.forceKey( false );   // release forced KEY if necessary; set KEY off
  keyerInterface.setKey( LOW );
  protocol.resetSendBuffer();
  paddle.reset();           // clear paddle buffers
  paddle.enableInterrupt(); // clear paddle semaphores
  wpmPrevious = wpm ;
  interrupts();
}

/**
 * 
 **/
void setSpeed(uint16_t wpm_set)
{ bool isChanged ;
  if (wpm_set < config.speedMinWpm)
    wpm_set = config.speedMinWpm;
  if (wpm_set > config.speedMaxWpm)
    wpm_set = config.speedMaxWpm;
  isChanged = ( wpm != wpm_set );
  wpm = wpm_set;
  if( isChanged ) protocol.sendStatus();
}
