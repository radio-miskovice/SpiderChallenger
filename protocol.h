
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include "send_buffer.h"

class Protocol {
  private:
    char rcvd;
    byte cmdBuffer[2];
    byte cmdLength = 0;
    bool expectCmd = false;
    bool canSendStatus = false;
    CharacterFIFO fifo;

  public:
    Protocol();
    void serviceSendBuffer();
    void resetSendBuffer();
    void sendStatus(bool forceSend = false);
    void execute(byte command, byte data, bool isBuffered);
    void checkCommand();
    void checkInput();
    void storeMessage(byte data);
    void sendMessage();
    void reportText(byte kind);
};

extern Protocol protocol ;

#endif