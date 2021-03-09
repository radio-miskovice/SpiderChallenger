
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
    void checkCommand();
    void checkInput();
    void execute(byte command, byte data, bool isBuffered);
    bool isSendingBuffer();
    void reportText(byte kind);
    void resetSendBuffer();
    void sendMessage();
    void sendStatus(bool forceSend = false);
    void serviceSendBuffer();
    void storeMessage(byte data);
};

extern Protocol protocol ;

#endif