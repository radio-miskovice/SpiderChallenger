# Spider Challenger Keyer

Based on Arduino Nano. Original motivation was to implement Spider Keyer firmware to work on TinyKeyer hardware. 
While Spider Keyer uses potentiometer as speed control, TinyKeyer uses rotary encoder. Why all the fuss? Contest 
program HamRacer does not work with the original TinyKeyer because the protocol is not compatible. 

*Version 1* is rewritten to use rotary encoder. All code is refactored to be more easily maintainable than the
original Spider Keyer. Changed interrupt definition to use PCINT (pin change interrupt) for both the 
paddles and the rotary encoder.

To maintain HW compatibility with TinyKeyer, button is implemented using ADC (contrary to SpiderKeyer).

Further steps:

1. Implement potentiometer and give the user both options, to use either rotary encoder or potentiometer as speed control.
2. Implement digital button option, to provide for more universal hardware compatibility.
3. Implement command mode, if possible K3NG compatible.
