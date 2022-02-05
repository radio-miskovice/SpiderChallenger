// uncomment features you want to use

// #define USE_POTENTIOMETER
#define USE_ROTARY_ENCODER

#if defined(USE_POTENTIOMETER) && defined(USE_ROTARY_ENCODER)
#error "Feature conflict: cannot use both potentiometer and rotary encoder"
#endif
