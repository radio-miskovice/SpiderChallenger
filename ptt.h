// PTT definitions

#if defined( PIN_PTT_OUT ) && (PIN_PTT_OUT>0)

inline void ptt_init() {
  pinMode(PIN_PTT_OUT, OUTPUT);
  digitalWrite(PIN_PTT_OUT, LOW);
}

inline void ptt_on() { digitalWrite(PIN_PTT_OUT, HIGH); }
inline void ptt_off() { digitalWrite(PIN_PTT_OUT, LOW); }

#else

inline void ptt_init() { }
inline void ptt_on() { }
inline void ptt_off() { }

#endif