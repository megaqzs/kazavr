#ifdef __AVR_ATmega328P__
#include <hal/uart.h>
#include <pins.h>
#include <stdio.h>
#include <matrix.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <delay.h>
#include <wire.h>

// watchdog interrupt
ISR (WDT_vect) 
{
   wdt_disable();  // disable watchdog
}  // end of WDT_vect

int main()
{
  clock_prescale_set(clock_div_2);
  setPinState(LED_BUILTIN, HIGH);
  mdelayc(500);
  setPinState(LED_BUILTIN, LOW);
  setupPins();
  uart_init();
  while (true) {
    scan_loop();
    udelayc(200);
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
    cli();           // timed sequence follows
    sleep_enable();
   
    // turn off brown-out enable in software
    MCUCR = bit (BODS) | bit (BODSE);
    MCUCR = bit (BODS); 
    sei();             // guarantees next instruction executed
    sleep_cpu ();
    
    // cancel sleep as a precaution
    sleep_disable();
  }
}
#endif
