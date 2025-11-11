#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <uart.h>
#include "matrix.h"
#include <wire.h>
#include <Arduino.h>

void set_clock_prescaler_div2() {
    CLKPR = 0x80;
    CLKPR = 1;
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    set_clock_prescaler_div2();
    setupPins();
    uart_init();

}
void loop()
{
    scan_loop();
}

