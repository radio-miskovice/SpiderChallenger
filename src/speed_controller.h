#ifndef SPEED_CONTROLLER_H 
#define SPEED_CONTROLLER_H

#include <Arduino.h>

class SpeedController {
  protected:
    byte value ;
    byte minValue = 0;
    byte maxValue = 0;
  public:
    bool hasChanged = false ;
    virtual void init() {};
    virtual void update() {};
    byte getValue() { hasChanged = false; return value; };
    void setMaxValue( byte max ) { maxValue = max ; };
    void setMinValue( byte min ) { minValue = min ; };
};

#endif