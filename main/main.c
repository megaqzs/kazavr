#if 0
#include <hal/uart.h>
#include <stdio.h>
#include "matrix.h"
#ifdef __AVR_ATmega328P__
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#endif
#include <wire.h>
#include <Arduino.h>

#ifdef __AVR_ATmega328P__
// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect
#endif

void setup() {
#ifdef __AVR_ATmega328P__
    clock_prescale_set(clock_div_2);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
#endif
    setupPins();
    uart_init();
}
void loop()
{
  scan_loop();
  delayMicroseconds(200);
#ifdef __AVR_ATmega328P__
  // disable ADC
  ADCSRA = 0;  

  // clear various "reset" flags
  MCUSR = 0;     
  // allow changes, disable reset
  WDTCSR = bit (WDCE) | bit (WDE);
  // set interrupt mode and an interval 
  WDTCSR = bit (WDIE);    // set WDIE, and 16ms seconds delay
  wdt_reset();  // pat the dog
  
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  noInterrupts ();           // timed sequence follows
  sleep_enable();
 
  // turn off brown-out enable in software
  MCUCR = bit (BODS) | bit (BODSE);
  MCUCR = bit (BODS); 
  interrupts ();             // guarantees next instruction executed
  sleep_cpu ();
  
  // cancel sleep as a precaution
  sleep_disable();
#endif
}

#endif
