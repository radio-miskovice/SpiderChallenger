
#ifndef POTENTIOMETER_CTRL_H
#define POTENTIOMETER_CTRL_H

#include <Arduino.h>
#include "features.h"
#include "pins.h"
#include "speed_controller.h"

// minimum time to elapse between measurements
#define POT_INTERVAL_MS 150

#ifdef __LGT8FX8P__
#define POT_FULL_SCALE ((1<<12) - 1)
#else
#define POT_FULL_SCALE  (1023)
#endif

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
