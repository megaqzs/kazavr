#pragma once
#ifdef __AVR_ATmega328P__
#include <avr/io.h>
typedef struct {
 unsigned short group:2;
 unsigned short pins:8;
} pingroup_t;

#define bit(x) (1<<(x))
#define PB 1
#define PC 2
#define PD 3

#define GPIN0 1
#define GPIN1 2
#define GPIN2 4
#define GPIN3 8
#define GPIN4 16
#define GPIN5 32
#define GPIN6 64
#define GPIN7 128

#define GPD0 ((pingroup_t) {PD, GPIN0})
#define GPD1 ((pingroup_t) {PD, GPIN1})
#define GPD2 ((pingroup_t) {PD, GPIN2})
#define GPD3 ((pingroup_t) {PD, GPIN3})
#define GPD4 ((pingroup_t) {PD, GPIN4})
#define GPD5 ((pingroup_t) {PD, GPIN5})
#define GPD6 ((pingroup_t) {PD, GPIN6})
#define GPD7 ((pingroup_t) {PD, GPIN7})

#define GPB0 ((pingroup_t) {PB, GPIN0})
#define GPB1 ((pingroup_t) {PB, GPIN1})
#define GPB2 ((pingroup_t) {PB, GPIN2})
#define GPB3 ((pingroup_t) {PB, GPIN3})
#define GPB4 ((pingroup_t) {PB, GPIN4})
#define GPB5 ((pingroup_t) {PB, GPIN5})

#define GPC0 ((pingroup_t) {PC, GPIN0})
#define GPC1 ((pingroup_t) {PC, GPIN1})
#define GPC2 ((pingroup_t) {PC, GPIN2})
#define GPC3 ((pingroup_t) {PC, GPIN3})
#define GPC4 ((pingroup_t) {PC, GPIN4})
#define GPC5 ((pingroup_t) {PC, GPIN5})
#define GPC6 ((pingroup_t) {PC, GPIN6})

#define RESET PC6
#define TX PD1
#define RX PD0

#define D0 GPD0
#define D1 GPD1
#define D2 GPD2
#define D3 GPD3
#define D4 GPD4
#define D5 GPD5
#define D5 GPD5
#define D6 GPD6
#define D7 GPD7
#define LED_BUILTIN GPB5
#define D8 GPB0
#define D9 GPB1
#define D10 GPB2
#define D11 GPB3
#define D12 GPB4
#define D13 GPB5
#define A0 GPC0
#define A1 GPC1
#define A2 GPC2
#define A3 GPC3
#define A4 GPC4
#define A5 GPC5

// inline the pin control functions in order to let the compiler optimize away the switch case
static inline void setPinPullEn(pingroup_t pins, int state) {
    if (state) {
        switch (pins.group) {
            case PB:
                PORTB |= pins.pins;
                break;
            case PC:
                PORTC |= pins.pins;
                break;
            case PD:
                PORTD |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case PB:
                PORTB &= ~pins.pins;
                break;
            case PC:
                PORTC &= ~pins.pins;
                break;
            case PD:
                PORTD &= ~pins.pins;
                break;
        }
    }
}

static inline void setPinDir(pingroup_t pins, int state) {
    if (state) {
        switch (pins.group) {
            case PB:
                DDRB |= pins.pins;
                break;
            case PC:
                DDRC |= pins.pins;
                break;
            case PD:
                DDRD |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case PB:
                DDRB &= ~pins.pins;
                break;
            case PC:
                DDRC &= ~pins.pins;
                break;
            case PD:
                DDRD &= ~pins.pins;
                break;
        }
    }

}

static inline unsigned int getPinPullEn(pingroup_t pins) {
    unsigned int val = 0;
    switch (pins.group) {
        case PB:
            val = pins.pins & PORTB;
            break;
        case PC:
            val = pins.pins & PORTC;
            break;
        case PD:
            val = pins.pins & PORTD;
            break;
    }
    return val;
}

// 1 is input 0 is output
static inline unsigned int getPinDir(pingroup_t pins) {
    unsigned int val = 0;
    switch (pins.group) {
        case PB:
            val = pins.pins & DDRB;
            break;
        case PC:
            val = pins.pins & DDRC;
            break;
        case PD:
            val = pins.pins & DDRD;
            break;
    }
    return val;
}

// get input pin state otherwise get output value
static inline unsigned int getPinState(pingroup_t pins) {
    unsigned int val = 0;
    switch (pins.group) {
        case PB:
            val = pins.pins & PINB;
            break;
        case PC:
            val = pins.pins & PINC;
            break;
        case PD:
            val = pins.pins & PIND;
            break;
    }
    return val;
}

static inline unsigned int getPinPull(pingroup_t pins) {
    return 0;
}

static inline void setPinPull(pingroup_t pins, int state) {
}

#define setPinState setPinPullEn
#endif
