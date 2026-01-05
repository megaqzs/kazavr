#include <hal/atmega328p/pins.h>
#include <hal/msp430/pins.h>

#define LOW 0
#define HIGH 1
#define OUTPUT 1
#define INPUT 0

// usefull for pin group manipulation
#define ACTIVE_LSB(x) ((x)&(~((x)-1)))
