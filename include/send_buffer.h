#ifndef SEND_BUFFER_H
#define SEND_BUFFER_H

#include <Arduino.h>
#include <avr/io.h>

/** FIFO class to implement circular buffer **/
class CharacterFIFO
{
private:
  char buffer[256];
  uint8_t head = 0;
  uint8_t tail = 0;

public:
  void reset();  // reset buffer content - empty buffer
  void push(char x); // push new character at the end of buffer
  char shift();      // read character from 
  void unshift();
  uint8_t getLength();
  uint8_t getFree();
  bool hasMore();
  bool canTake();
};

// extern CharacterFIFO sendBuffer ;

#endif