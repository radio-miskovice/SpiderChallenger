#include <Arduino.h>
#include "send_buffer.h"
#include "utilities.h"
#include "config.h"
// #include "core_variables.h"
#include "morse.h"

enum FetchProgressPhase : byte { FETCH_ANY, EXPECT_CMD, EXPECT_PARAMS, EXECUTE };
enum WinkeyStatusMode : byte { WK1, WK2 };
const word WINKEY_SIDETONE_FREQ = 4000 ;

class WinkeyProtocol {
  private:
    static const byte WK_REVISION = 22 ; // Winkey protocol revision number
    FetchProgressPhase phase = FETCH_ANY;
    WinkeyStatusMode wkStatusMode = WK1 ;
    byte bytesExpected ;    // total bytes to be read into cmdParam
    byte bytesFetched = 0 ; // total bytes fetched into cmdParam
    word command ;          // command bytes, prefix = MSB
    byte cmdParam[16];      // command parameter(s)
    byte _isHostOpen ;
    bool _isStatusDirty = false;
    word wasResponded = 0;
    bool _sidetonePaddleOnly = false ;
    void ignore();
    void setSidetone(byte);
    void setWpmSpeed(byte);
    void setWeighting(byte);
public:
  bool expectCmd = false;
  CharacterFIFO fifo;
  WinkeyProtocol();
  bool isHostOpen();
  void checkInput();
  void executeCommand();
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
 * @returns {bool} true if host is open, false otherwise
 */
bool WinkeyProtocol::isHostOpen() { return _isHostOpen ; }

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
          phase = EXPECT_CMD ;
        }
        // otherwise push it to text buffer
        else { fifo.push(input); } 
        break ;
      case EXPECT_CMD:
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
        else { phase = EXECUTE ; cmdParam[0] = 0 ; }
        break ;
      case EXPECT_PARAMS:
        if( bytesExpected > 0 ) {
          cmdParam[ bytesFetched ] = input ;
          bytesFetched++ ;
          if( command == 0x0116 && bytesFetched == 1 && input == 3 ) bytesExpected++ ; // command Buffer Pointer Command has extra byte if parameter == 3
          bytesExpected-- ;
        }
        if( bytesExpected == 0 ) phase = EXECUTE ;
    }
    canReadMore = (phase != EXECUTE) && (fifo.canTake() || phase != FETCH_ANY);
  }
}

/**
 * Execute command fetched in command buffer
 */
void WinkeyProtocol::executeCommand() {
  if( phase != EXECUTE ) return ;
  // TODO: execute command
  switch( command ) {
  case 0x04: // Admin: Send Echo - cmdParam[0] contains character to be echoed
  case 0x05: // Admin: Paddle A2D
  case 0x06: // Admin: Speed A2D
  case 0x09: // Admin: Get Calibration
    sendResponse(cmdParam[0]); // cmdParam[0] contains zero in commands with no params
    break;
  // Reset
  case 0x01:
    reboot_cpu();
    break;
  // Host Open
  case 0x02:
    _isHostOpen = true;
    sendResponse(WK_REVISION);
    break; 
  // Host Close
  case 0x03:
    _isHostOpen = false;
    break;
  // Status Mode WK1
  case 0x0A:
    wkStatusMode = WK1;
    break;
  // Status Mode WK2
  case 0x0B:
    wkStatusMode = WK2;
    break;
  // Sidetone Control
  case 0x101:
    _sidetonePaddleOnly = (cmdParam[0] & 0x80) != 0 ;
    cmdParam[0] = cmdParam[0] & 0x0F ;
    if( cmdParam[0] != 0 ) {
      config.toneManualHz = 4000 / cmdParam[0] ;
      if( ! _sidetonePaddleOnly ) config.toneAutoHz = config.toneManualHz ;
      else config.toneAutoHz = 0 ;
    }
    break ;
  // set WPM
  case 0x102:
    morseEngine.setWpm(cmdParam[0]) ;
    break ;
  // set weighting
  case 0x103:
    morseEngine.setWeighting(cmdParam[0]);
    break;

  // all unimplemented commands w/o response  
  default:
    ignore();
  }
  phase = FETCH_ANY ;
}

void WinkeyProtocol::ignore() {
}
