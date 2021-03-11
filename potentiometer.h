
#ifndef POTENTIOMETER_CTRL_H
#define POTENTIOMETER_CTRL_H

#include <Arduino.h>
#include "features.h"
#include "pins.h"
#include "speed_controller.h"

// minimum time to elapse between measurements
#define POT_INTERVAL_MS 150
#define POT_FULL_SCALE  1023

class Potentiometer : public SpeedController
{
  private:
  unsigned long lastMeasurementMs = 0 ;
  public:
  void init() override ;
  void update() override;
};

#ifdef USE_POTENTIOMETER
extern Potentiometer potentiometer ;
#endif

#endif
