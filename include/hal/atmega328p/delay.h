#ifdef __AVR_ATmega328P__
#  define cdelayc(x) __builtin_avr_delay_cycles(x)
#endif

