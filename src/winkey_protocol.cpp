#include <Arduino.h>
#include "send_buffer.h"

enum FetchProgressPhase : byte { FETCH_ANY, EXPECT_SUFFIX, EXPECT_PARAMS, EXECUTE };
class WinkeyProtocol {
  // winkey status
  private:
    FetchProgressPhase phase = FETCH_ANY;
    byte bytesExpected ; // total bytes to be read into cmdBuffer
    bool readingCommand = false ;
    byte isHostOpen ;
    byte bytesFetched = 0 ;
    byte cmdBuffer[16];
    word command ;
    byte cmdLength = 0;
    bool canSendStatus = false;
    word wasResponded = 0;
    void respondZero() ;
    void ignore();
    void executeAdminCommand();
    void executeKeyerCommand();

public:
  bool expectCmd = false;
  CharacterFIFO fifo;
  WinkeyProtocol();
  bool isNotCommand(char rcvd);
  void checkInput();
  void execute();
  bool isSendingBuffer();
  void reportText(byte kind);
  void resetSendBuffer();
  void sendMessage();
  void sendStatus(bool forceSend = false);
  void sendResponse(byte);
  void sendResponse(byte*, byte length);
  void serviceSendBuffer();
  void storeMessage(byte data);
};

/**
 * Reads serial port if character is available, except if previous character is still waiting to be processed.
 * Command characters are taken immediately into command buffer. All other characters are sent to circular text buffer
 * or stored temporarily in pendingChar in case of buffer congestion.
 */
void WinkeyProtocol::checkInput() {
  char input ;
  word commandCode ;
  bool canReadMore = (phase != EXECUTE) && ( fifo.canTake() || phase != FETCH_ANY ); // we can read if command fetch is in progress or text buffer can consume char
  // repeat as long as we can
  while( canReadMore && Serial.available()>0 ) {
    // read one character 
    input = Serial.read();
    switch( phase ) {
      case FETCH_ANY:
        // if the byte is 0x00 or 0x01, start fetching command
        if( input < 2 ) {
          command = input ;         // copy byte into word
          command = command * 256 ; // shift command prefix 8 bits up
          phase = EXPECT_SUFFIX ;
        }
        // otherwise push it to text buffer
        else { fifo.push(input); } 
        break ;
      case EXPECT_SUFFIX:
        command = command + input ; // complete command code
        bytesFetched = 0 ;
        switch( commandCode ) {
            // Load Defaults - 15 bytes
            case 0x010F:
              bytesExpected = 15; break ;
            // Set Potentiometer Range
            case 0x0105:
              bytesExpected = 3; break ;
            // 2-byte commands
            case 0x0104:
            case 0x011B:
              bytesExpected = 2; break ;
            // 1-byte commands
            case 0x0000:
            case 0x0004:
            case 0x000E:
            case 0x0101:
            case 0x0102:
            case 0x0103:
            case 0x0106:
            case 0x0109:
            case 0x010B:
            case 0x010C:
            case 0x010D:
            case 0x010E:
            case 0x0110:
            case 0x0111:
            case 0x0112:
            case 0x0114:
            case 0x0116:
            case 0x0117:
            case 0x0118:
            case 0x0119:
            case 0x011A:
            case 0x011C:
            case 0x011D:
              bytesExpected = 1; break ;
            default:
              bytesExpected = 0 ;
        }
        if( bytesExpected > 0 ) { phase = EXPECT_PARAMS ; }
        else { phase = EXECUTE ; cmdBuffer[0] = 0 ; }
        break ;
      case EXPECT_PARAMS:
        if( bytesExpected > 0 ) {
          cmdBuffer[ bytesFetched ] = input ;
          bytesFetched++ ;
          if( command == 0x0116 && bytesFetched == 1 && input == 3 ) bytesExpected++ ; // command Buffer Pointer Command has extra byte if parameter == 3
          bytesExpected-- ;
        }
        if( bytesExpected == 0 ) phase = EXECUTE ;
    }
  }
}

void WinkeyProtocol::execute() {
  if( phase != EXECUTE ) return ;
  // TODO: execute command
  switch( command ) {
  case 0x04: // Admin: Send Echo
  case 0x05: // Admin: Paddle A2D
  case 0x06: // Admin: Speed A2D
  case 0x09: // Admin: Get Calibration
    sendResponse(cmdBuffer[0]);
    break;

  default:
    ignore();
  }
  phase = FETCH_ANY ;
}

void WinkeyProtocol::ignore() {
}

void WinkeyProtocol::respondZero() {
  sendResponse(0);
}
