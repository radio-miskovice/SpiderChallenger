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
#include "eeprom_config.h"  // configuration load and save

#include "morse.h"            // morse codec engine and timing control

#include "paddle-interface.h" // paddle interface (port and interrupt definitions and handling)
#include "keyer-interface.h"  // keyer & PTT interface (port definitions and handling)
#include "speed_control.h"    // polymorphic speed control (potentiometer or rotary encoder)
#include "analog_button.h"    // TODO: create polymorphic button handler
#include "config.h"           // config structure; TODO: merge with eeprom_config
#include "protocol.h"         // protocol handler with integrated send buffer and command processor (Spider Protocol v. 2)

// internally used constants

// #define KEY_UP 0
// #define KEY_DOWN 1

// CONFIGURATION - these variables will be written to EEPROM
Config config = {
    .paddleMode = IAMBIC_B, 
    .toneManualHz = 750,
    .toneAutoHz = 750, 
    .pttTailMs = 5,
    .pttLeadMs = 30,
    .pttHangTimeWSPct = 90,
    .speedMinWpm = 12,
    .speedMaxWpm = 36,
    .paddleTriggerPtt = true, 
    .weightingPct = 50,
    .isPaddleSwapped = false
};

bool configIsDirty = false ;

/* SPEED SETTINGS */
unsigned int wpm = 25;        // current speed in WPM
unsigned int wpmPrevious = 25 ; // speed set manually before changed by buffered command  
unsigned int wpmMaxPaddle = 0;  // maximum speed for paddle keying in WPM. 0: switched off
bool speedIsSetManually = true; // true if manual speed control is enabled (rotary or potentiometer)
bool speedManualSetEnabled = true ;

/* KEYER MODE AND KEYING PARAMETERS */

byte wordspaceLU = 6;
byte letterspaceLU = 3;

/* SENDING MODE */
bool isCommandMode = false;

long btn_toggle; // Debounce the message sending button
unsigned long last_powerbank;

/**
 * Initial setup 
 */
void setup()
{
  // command mode LED
  pinMode(PIN_MODE_LED, OUTPUT);

  // Paddles
  pinMode(PIN_PADDLE_LEFT, INPUT_PULLUP);
  pinMode(PIN_PADDLE_RIGHT, INPUT_PULLUP);

  // KEY line
  pinMode(PIN_KEY_LINE, OUTPUT);
  
  // PTT line
  #if defined(PIN_PTT_OUT) && (PIN_PTT_OUT > 0)
  pinMode(PIN_PTT_OUT, OUTPUT);
  digitalWrite(PIN_PTT_OUT, LOW);
  #endif
  
  #ifdef USE_POWERBANK
  pinMode (dummy_1, OUTPUT);
  pinMode (dummy_2, OUTPUT);
  digitalWrite (dummy_1, LOW);
  digitalWrite (dummy_2, LOW);
  #endif
  // pinMode (button, INPUT_PULLUP);

  pinMode(PIN_SIDETONE, OUTPUT);
  digitalWrite(PIN_SIDETONE, LOW);
  digitalWrite(PIN_MODE_LED, HIGH);
  load_config();
  speedControl->init();
  speedControl->setMaxValue( config.speedMaxWpm );
  speedControl->setMinValue( config.speedMinWpm );
  setSpeed( speedControl->getValue() ); // in case of potentiometer, read potentiometer, otherwise use initial values
  resetInterfaces();
  Serial.begin(57600); // primary_serial_port_baud_rate
  tone(PIN_SIDETONE, 512);
  delay(46);
  tone(PIN_SIDETONE, 1024);
  delay(184);
  noTone(PIN_SIDETONE);
  digitalWrite(PIN_MODE_LED, LOW);
}

/* Main loop */
void loop()
{
  checkAnalogButton();
  service_command_button();
  paddle.handleInterrupt(); // properly handle paddle break if occurred
  paddle.check(paddle.DIT); // check paddle state
  paddle.check(paddle.DAH); // check paddle state
  paddle.serviceBuffers();  // handle paddle memory accordingly
  
  protocol.checkInput();    // check input stream and handle incoming data if necessary
  paddle.serviceBuffers();  // again handle paddle memory

  protocol.serviceSendBuffer();   // handle any data in send buffer
  keyerInterface.checkPttTail();  // turn off PTT after PTT tail time elapsed after last key up
  speedControl->update();   // update speed control values
  if( speedManualSetEnabled ) {
    setSpeed( speedControl->getValue() );
  }
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
  paddle.sendingModeLast = SEND_MODE_PADDLE;
  paddle.sendingMode = SEND_MODE_PADDLE;
  interrupts();
}

/**
 * 
 **/
void setSpeed(int wpm_set)
{
  if (wpm_set < config.speedMinWpm)
    wpm_set = config.speedMinWpm;
  if (wpm_set > config.speedMaxWpm)
    wpm_set = config.speedMaxWpm;
  wpm = wpm_set;
  protocol.sendStatus();
}
