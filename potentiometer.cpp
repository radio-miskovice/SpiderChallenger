
#include <Arduino.h>
#include "pins.h"
#include "core.h"
#include "core_variables.h"

#ifdef USE_POTENTIOMETER 

#define POT_CHECK_INTERVAL_MS 150
#define POT_CHANGE_THRESHOLD  0.9

/* POTENTIOMETER */
byte pot_wpm_read_last;
unsigned long last_pot_check_time;
int pot_full_scale_reading;
bool speed_set_by_pot;

void check_potentiometer()
{
  if (FEATURE_ENABLED( enabled_features, FLAG_ENABLE_POT ))
    if ((millis() - last_pot_check_time) > POT_CHECK_INTERVAL_MS)
    {
      byte pot_value_wpm_read = pot_value_wpm();
      if ((abs(pot_value_wpm_read - pot_wpm_read_last) > POT_CHANGE_THRESHOLD))
      {
        speed_set_by_pot = true;
        speed_set(pot_value_wpm_read);
        pot_wpm_read_last = pot_value_wpm_read;
      }
      last_pot_check_time = millis();
    }
}

byte pot_value_wpm()
{
  int pot_read = analogRead(PIN_POTENTIOMETER);
  byte return_value = map(pot_read, 0, pot_full_scale_reading, wpm_limit_low, wpm_limit_high);
  return return_value;
}

inline void set_potentiometer_defaults() {
  pot_wpm_read_last = 0;
  last_pot_check_time = 0;
  pot_full_scale_reading = 1023; //default_pot_full_scale_reading;
  speed_set_by_pot = true;
}

#else // potentiometer not used at all

inline void check_potentiometer() { }
inline void set_potentiometer_defaults() {}

#endif 