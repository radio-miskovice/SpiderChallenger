/**
 * Link protocol handling 
 * Spider Protocol by OK1FIG
 * [CC BY-NC-4.0] Creative commons Licence 4.0
 * https://creativecommons.org/licenses/by-nc/4.0/
 **/
#include <Arduino.h>
#include <EEPROM.h>
#include "pins.h"
#include "config.h"
#include "command.h"
#include "core.h"
#include "core_variables.h"
#include "morse.h"
#include "send_buffer.h"

#include "speed_control.h"
#include "keyer_interface.h"
#include "paddle_interface.h"
#include "protocol.h"

Protocol protocol = Protocol();

/*
 * Jumping to 0x0000 will restart the whole program
 */
void soft_reset()
{
  void (*reboot)() = 0x0000;
  (*reboot)();
}

Protocol::Protocol()
{
  fifo = CharacterFIFO();
}

/** check if received byte is a command and act accordingly
 *  if not a command, just push it to the character FIFO **/
bool Protocol::isNotCommand(char rcvd)
{
  if (expectCmd)
  { // expecting command after Esc
    cmdBuffer[cmdLength++] = rcvd;
    if (cmdLength >= 2)
    {
      cmdLength = 0;
      expectCmd = false;
      execute(cmdBuffer[0], cmdBuffer[1], false);
    }
    return false; // it was command
  }
  else
  { // not waiting for command bytes after Esc
    if (rcvd == 0x1B)
    { // Esc character received
      expectCmd = true;
      cmdLength = 0;
      return false ;
    }
  }
  return true ;
}

/** Check Serial for input bytes and process them **/
void Protocol::checkInput() { 
  bool needResponse = true; 
  char rcvd ;
  while (Serial.available() > 0 && fifo.canTake())
  {
    if( !fifo.hasMore() ) {
      paddle.reset();
      paddle.enableInterrupt();
    }
    rcvd = Serial.read();
    if (isNotCommand( rcvd )) {
      fifo.push(rcvd);
    }
    if (needResponse)
    {
      delay(12);
      sendStatus();
      needResponse = false ;
      wasResponded++ ;
    }
  }
}

/** Execute command 
 * @param command the command
 * @param data command data
 * @param isBuffered set to true if the commend was fetched from FIFO
 */
void Protocol::execute(byte command, byte data, bool isBuffered)
{
  switch (command)
  {

  // force PTT on or off (no effect unless USE_PTT or PIN_PTT_LINE are defined!)
  case CMD_SET_PTT:
    fifo.reset(); // empty buffer
    sendStatus(true);
    keyerInterface.forcePtt(data > 0 ? HIGH : LOW);
    break;

  // force KEY on or off
  case CMD_SET_KEY:
    fifo.reset();
    keyerInterface.forcePtt(data == 2);
    keyerInterface.forceKey(data > 0);
    if (data == 0)
    {
      keyerInterface.setKey(LOW); // unforce KEY
    }
    else
      paddle.wasTouched = false;
    sendStatus();
    break;

  // change speed explicitly or reset to manual control
  case CMD_SPEED_CHANGE:
    if (data == 255)
    {
      speedIsSetManually = true;
      setSpeed(speedControl->getValue());
    }
    else
    {
      if (data == 0)
        wpm = wpmPrevious; // reset to previous speed
      else if (isBuffered)
      {
        wpmPrevious = wpm;
        setSpeed(data);
      }
      else
      {
        setSpeed(data);
        speedIsSetManually = false;
        sendStatus();
      }
    }
    break;

  // keyer reset
  case CMD_BREAK_IMMY:
    resetInterfaces();
    break;

  case CMD_SET_LEAD_TIME:
    config.pttLeadMs = data * 5;
    configIsDirty = true;
    break;

  case CMD_SET_TAIL_TIME:
    config.pttTailMs = data * 5;
    configIsDirty = true;
    break;

  case CMD_SET_HANG_TIME:
    config.pttHangTimeWSPct = data;
    configIsDirty = true;
    break;

  case CMD_SET_WEIGHTING:
    config.weightingPct = data;
    configIsDirty = true;
    break;

  case CMD_ENABLE_FEATURES:
    keyerInterface.enablePtt(data & 1);
    keyerInterface.enableKey(data & 2);
    speedManualSetEnabled = ((data & 4) != 0);
    break;

  case CMD_SET_PADDLES_TRIGGER_PTT:
    resetInterfaces();
    config.paddleTriggerPtt = (data != 0);
    configIsDirty = true;
    break;

  case CMD_SET_SIDETONE_AUTOMATIC:
    if (data > 10)
    {
      config.toneAutoHz = data * 10;
      configIsDirty = true;
    }
    break;

  case CMD_SET_SIDETONE_MANUAL:
    if (data > 10)
    {
      config.toneManualHz = data * 10;
      configIsDirty = true;
    }
    break;

  case CMD_SET_IAMBIC_MODE:
    if (data == 0)
    {
      if (config.paddleMode != IAMBIC_A)
      {
        config.paddleMode = IAMBIC_A;
        configIsDirty = true;
      }
    }
    else if (data == 1)
    {
      if (config.paddleMode != IAMBIC_B)
      {
        config.paddleMode = IAMBIC_B;
        configIsDirty = true;
      }
    }
    break;

  // warm boot
  case CMD_RESET:
    soft_reset();
    break;

  // report status
  case CMD_PING:
    sendStatus(true);
    break;

  // short beep
  case CMD_BEEP:
    tone(PIN_SIDETONE, 750);
    delay(33);
    noTone(PIN_SIDETONE);
    break;

  // report self to PC
  case CMD_GET_SIGNATURE:
    // TODO: implement as method
    reportText(data);
    break;

  case CMD_SET_FEEDBACK:
    canSendStatus = (data > 0);
    sendStatus();
    break;

  case CMD_SET_LOW_LIMIT:
    if (data == 0)
      config.speedMinWpm = 12;
    else
      config.speedMinWpm = data;
    configIsDirty = true;
    break;

  case CMD_SET_HIGH_LIMIT:
    if (data == 0)
      config.speedMaxWpm = 40;
    else
      config.speedMaxWpm = data;
    configIsDirty = true;
    break;

  case CMD_SET_MANUAL_SENDING_LIMIT:
    wpmMaxPaddle = data;
    break;

  case CMD_SET_PADDLES_SWAPPED:
    config.isPaddleSwapped = (data != 0);
    configIsDirty = true;
    break;

  case CMD_FARNSWORTH:
    data = ( data % 10 ) ;
    letterspaceLU = 3 * (data + 1) ;
    wordspaceLU = 7 * (data + 1) ;
    break ;

  case CMD_SAVE_CONFIG:
    // saveConfig(!data); // data == 0 will erase old configuration
    break;

  case CMD_STORE_MSG:
    storeMessage(data);
    break;
  }
}

bool Protocol::isSendingBuffer() {
  return fifo.hasMore();
}

void Protocol::resetSendBuffer()
{
  fifo.reset();
  paddle.disableInterrupt();
}

void Protocol::sendStatus(bool forceSend)
{
  if (canSendStatus || forceSend)
  {
    byte statusBits;
    byte wpmByte;
    statusBits = 0x80 ;
    if( fifo.hasMore() || expectCmd ) statusBits |= 0x20 ;
    statusBits |= (keyerInterface.getInterfaceStatus() << 3);
    statusBits |= paddle.wasTouched ? 0x04 : 0 ;
    wpmByte = wpm & 0x7F;
    Serial.write(statusBits);
    Serial.write(wpmByte);
  }
}

void Protocol::serviceSendBuffer()
{
  char c;
  if (fifo.hasMore())
  {
    c = fifo.shift();
    if (c >= ' ')
      morseEngine.sendAsciiChar(c);
    else
    {
      if (fifo.hasMore())
      {
        char data = fifo.shift();
        execute(c, data, true);
      }
      else
        fifo.unshift(); // if the second byte is not available, return the first byte back to FIFO
    }
    if( !fifo.hasMore() ) {
      // report empty buffer
      paddle.disableInterrupt();
      sendStatus();
    }
  }
}


// append one byte to stored message
void Protocol::storeMessage(byte data)
{
  // Stores button-message to EEPROM. Use half of the EEPROM size, start at the middle, at 256:
  int i;
  if (data == 0)
  { // Mark the whole as unused
    for (i = 256; i < 512; i++)
    {
      EEPROM.write(i, 0xFF);
    }
  }
  else
  {
    i = 256;
    while (EEPROM.read(i) != 0xFF && i < 511)
      i++; // Find first unoccupied position
    if (i < 512)
      EEPROM.write(i, data);
  }
}

void Protocol::sendMessage()
{
  int i = 256;
  do
  {
    char c = EEPROM.read(i++);
    if (c != 0xFF)
      fifo.push(c);
    else
      break;
  } while (i < 512);
}

void Protocol::reportText(byte kind)
{
  switch (kind)
  {
  case 1: // new: protocol ID
    Serial.println("Spider/2");
    break;

  case 2: // new: firmware ID
    Serial.print("Spider Challenger [");
    Serial.print(CHALLENGER_VERSION);
    Serial.println("]");
    break;

  // verbose status
  case 17:
    Serial.print("Iambic mode ");
    Serial.println(config.paddleMode == IAMBIC_A ? 'A' : 'B');
    Serial.print("Speed ");
    Serial.print(wpm);
    Serial.println("WPM");
    Serial.print("Buffer contains ");
    Serial.print(fifo.getLength());
    Serial.print(" chars, ");
    Serial.print(fifo.getFree());
    Serial.println(" bytes free.");
    break;

  default: // legacy ID
    Serial.print("Spider Keyer (Arduino Nano) by OK1FIG [");
    Serial.print(SPIDER_KEYER_VERSION); // report compatible emulation version
    Serial.print("]"); // Don't change this to work seamlessly with HamRacer
  }
}
