#include "send_buffer.h"

/**
 * Resets send buffer to empty state
 **/
void CharacterFIFO::reset() {
    head = 0;
    tail = 0;
}

/**
 * Push a character into buffer
 **/
void CharacterFIFO::push(char x) { buffer[head++] = x; }

/** 
 * Read the first character available for reading and increment tail
 **/
char CharacterFIFO::shift() {
  if (head == tail) return 0;
  else return buffer[tail++];
}

/** Return character last read back to buffer */
void CharacterFIFO::unshift() {  tail-- ; }

/** Return number of chars in the buffer **/
uint8_t CharacterFIFO::getLength() { return (head - tail); }

/** Return number of free chars in the buffer **/
uint8_t CharacterFIFO::getFree() { return (255 - getLength()); }

/** Return current head **/
bool CharacterFIFO::hasMore() { return (tail != head); }

/** Return current tail **/
bool CharacterFIFO::canTake() { return ( getLength() < 255 ); }
