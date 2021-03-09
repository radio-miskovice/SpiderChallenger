#include "features.h"

#ifdef USE_POTENTIOMETER

// use potentiometer as speed control
#include "potentiometer.h"
SpeedController *speedControl = &potentiometer;

#elif defined(USE_ROTARY_ENCODER)

// use rotary encoder as speed control
#include "rotary_encoder.h"
SpeedController *speedControl = &encoder;

#endif // USE_POTENTIOMETER
