
#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <Arduino.h>
#include "send_buffer.h"

class Protocol {
  private:

    byte cmdBuffer[2];
    byte cmdLength = 0;
    bool canSendStatus = false;
    word wasResponded = 0 ;

  public:
    bool expectCmd = false;
    CharacterFIFO fifo;
    Protocol();
    bool isNotCommand(char rcvd);
    void checkInput();
    void executeCommand(byte command, byte data, bool isBuffered);
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