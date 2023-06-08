#include <Arduino.h>

class WinkeyProtocol {
  // winkey status
  private:
    static const byte expectAdmin[] ;
    byte isHostOpen ;
    byte bytesFetched = 0 ;
  public:
  byte fetch() ;
  byte expect(byte);
  void report(byte);
  void report(word);
};

const byte WinkeyProtocol::expectAdmin[] = { 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };