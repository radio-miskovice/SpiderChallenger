#include "features.h"
#include "rotary_encoder.h"
#include "potentiometer.h"
#include "core_variables.h"

// this is just a replacement, to avoid #ifdef in the main program
inline void check_speed_control()
{
#ifdef USE_POTENTIOMETER
  check_potentiometer();
#elif defined(USE_ROTARY_ENCODER)
  check_rotary_encoder();
#endif
}

// different behavior for potentiometer vs. encoder
inline byte get_wpm_from_controls()
{
#ifdef USE_POTENTIOMETER
  return pot_value_wpm();
#else
  return wpm;
#endif
}
