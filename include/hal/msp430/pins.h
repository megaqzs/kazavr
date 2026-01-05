#pragma once
#ifdef ENERGIA_MSP_EXP430G2453
#include <msp430.h> // Essential for PxOUT/PxDIR definitions

typedef struct {
    unsigned short group:4;
    unsigned short pins:8;
} pingroup_t;

#define PORT1 1
#define PORT2 2
#define PORT3 3

#define GPIN0 1
#define GPIN1 2
#define GPIN2 4
#define GPIN3 8
#define GPIN4 16
#define GPIN5 32
#define GPIN6 64
#define GPIN7 128

// Macros for Pin definitions
#define P1_0 {PORT1, GPIN0}
#define P1_1 {PORT1, GPIN1}
#define P1_2 {PORT1, GPIN2}
#define P1_3 {PORT1, GPIN3}
#define P1_4 {PORT1, GPIN4}
#define P1_5 {PORT1, GPIN5}
#define P1_6 {PORT1, GPIN6}
#define P1_7 {PORT1, GPIN7}

#define P2_0 {PORT2, GPIN0}
#define P2_1 {PORT2, GPIN1}
#define P2_2 {PORT2, GPIN2}
#define P2_3 {PORT2, GPIN3}
#define P2_4 {PORT2, GPIN4}
#define P2_5 {PORT2, GPIN5}
#define P2_6 {PORT2, GPIN6}
#define P2_7 {PORT2, GPIN7}

#define P3_0 {PORT3, GPIN0}
#define P3_1 {PORT3, GPIN1}
#define P3_2 {PORT3, GPIN2}
#define P3_3 {PORT3, GPIN3}
#define P3_4 {PORT3, GPIN4}
#define P3_5 {PORT3, GPIN5}
#define P3_6 {PORT3, GPIN6}
#define P3_7 {PORT3, GPIN7}

// Set Output High/Low
static inline void setPinState(pingroup_t pins, int state) {
    if (state) {
        switch (pins.group) {
            case PORT1:
                P1OUT |= pins.pins;
                break;
            case PORT2:
                P2OUT |= pins.pins;
                break;
            case PORT3:
                P3OUT |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case PORT1:
                P1OUT &= ~pins.pins;
                break;
            case PORT2:
                P2OUT &= ~pins.pins;
                break;
            case PORT3:
                P3OUT &= ~pins.pins;
                break;
        }
    }
}

#define setPinPull setPinState

// Set Direction (1 for Output, 0 for Input)
static inline void setPinDir(pingroup_t pins, int output) {
    if (output) {
        switch (pins.group) {
            case PORT1:
                P1DIR |= pins.pins;
                break;
            case PORT2:
                P2DIR |= pins.pins;
                break;
            case PORT3:
                P3DIR |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case PORT1:
                P1DIR &= ~pins.pins;
                break;
            case PORT2:
                P2DIR &= ~pins.pins;
                break;
            case PORT3:
                P3DIR &= ~pins.pins;
                break;
        }
    }
}

// Get Input State
static inline unsigned int getPinState(pingroup_t pins) {
    unsigned int val = 0;
    switch (pins.group) {
        case PORT1:
            val = (P1IN & pins.pins);
            break;
        case PORT2:
            val = (P2IN & pins.pins);
            break;
        case PORT3:
            val = (P3IN & pins.pins);
            break;
    }
    return val;
}

// MSP430 Specific: Internal Resistor Control
static inline void setPinPullEn(pingroup_t pins, int enable) {
    if (enable) {
        switch (pins.group) {
            case PORT1:
                P1REN |= pins.pins;
                break;
            case PORT2:
                P2REN |= pins.pins;
                break;
            case PORT3:
                P3REN |= pins.pins;
                break;
        }
    } else {
        switch (pins.group) {
            case PORT1:
                P1REN &= ~pins.pins;
                break;
            case PORT2:
                P2REN &= ~pins.pins;
                break;
            case PORT3:
                P3REN &= ~pins.pins;
                break;
        }
    }
}
#endif
