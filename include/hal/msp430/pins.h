#pragma once
#ifdef ENERGIA_MSP_EXP430G2453
#include <msp430.h> // Essential for PxOUT/PxDIR definitions

typedef struct {
    unsigned short group:4;
    unsigned short pins:8;
} pingroup_t;

enum {
    P1 = 1,
    P2,
    P3
} port_data;

enum {
    PIN0 = BIT0,
    PIN1 = BIT1,
    PIN2 = BIT2,
    PIN3 = BIT3,
    PIN4 = BIT4,
    PIN5 = BIT5,
    PIN6 = BIT6,
    PIN7 = BIT7
} pins_data;

// Macros for Pin definitions
#define P1_0 ((pingroup_t) {P1, PIN0})
#define P1_1 ((pingroup_t) {P1, PIN1})
#define P1_2 ((pingroup_t) {P1, PIN2})
#define P1_3 ((pingroup_t) {P1, PIN3})
#define P1_4 ((pingroup_t) {P1, PIN4})
#define P1_5 ((pingroup_t) {P1, PIN5})
#define P1_6 ((pingroup_t) {P1, PIN6})
#define P1_7 ((pingroup_t) {P1, PIN7})
#define P2_0 ((pingroup_t) {P2, PIN0})

// Set Output High/Low
inline void setPinState(pingroup_t pins, int state) {
    if (state) {
        switch (pins.group) {
            case P1:
                P1OUT |= pins.pins;
                break;
            case P2:
                P2OUT |= pins.pins;
                break;
            case P3:
                P3OUT |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case P1:
                P1OUT &= ~pins.pins;
                break;
            case P2:
                P2OUT &= ~pins.pins;
                break;
            case P3:
                P3OUT &= ~pins.pins;
                break;
        }
    }
}

#define setPinPull setPinState

// Set Direction (1 for Output, 0 for Input)
inline void setPinDir(pingroup_t pins, int output) {
    if (output) {
        switch (pins.group) {
            case P1:
                P1DIR |= pins.pins;
                break;
            case P2:
                P2DIR |= pins.pins;
                break;
            case P3:
                P3DIR |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case P1:
                P1DIR &= ~pins.pins;
                break;
            case P2:
                P2DIR &= ~pins.pins;
                break;
            case P3:
                P3DIR &= ~pins.pins;
                break;
        }
    }
}

// Get Input State
inline unsigned int getPinState(pingroup_t pins) {
    unsigned int val = 0;
    switch (pins.group) {
        case P1:
            val = (P1IN & pins.pins);
            break;
        case P2:
            val = (P2IN & pins.pins);
            break;
        case P3:
            val = (P3IN & pins.pins);
            break;
    }
    return val;
}

// MSP430 Specific: Internal Resistor Control
inline void setPinPullEn(pingroup_t pins, int enable) {
    if (enable) {
        switch (pins.group) {
            case P1:
                P1REN |= pins.pins;
                break;
            case P2:
                P2REN |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case P1:
                P1REN &= ~pins.pins;
                break;
            case P2:
                P2REN &= ~pins.pins;
                break;
        }
    }
}
#endif
