#ifdef ENERGIA_MSP_EXP430G2453
#include <quirks.h>
#include <matrix.h>
#include <delay.h>
#include <hal/uart.h>
#include <msp430.h>
#include "config.h"
#include <pins.h>

volatile unsigned int vlo_period;

void calibrate_vlo(void) {
    // 1. Setup DCO to 1MHz (using factory calibration constants)
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // 3. Setup Timer_A to capture ACLK
    // TASSEL_2: SMCLK (1MHz) as clock source
    // MC_2: Continuous mode
    TACTL = TASSEL_2 + MC_2;

    // CCIS_1: Capture input from ACLK
    // CAP: Capture mode
    // CM_1: Capture on rising edge
    TACCTL0 = CAP + CM_1 + CCIS_1;

    // 4. Perform the measurement
    while (!(TACCTL0 & CCIFG));     // Wait for first capture
    unsigned int start = TACCR0;    // Save start time
    TACCTL0 &= ~CCIFG;              // Clear flag

    while (!(TACCTL0 & CCIFG));     // Wait for second capture
    vlo_period = TACCR0 - start;    // Difference = DCO counts per VLO cycle

    // vlo_period now contains how many microseconds (at 1MHz) one VLO cycle lasts.
    // If vlo_period = 83, then freq = 1,000,000 / 83 = ~12.04 kHz.

    TACTL = MC_0;                   // Stop timer
    TACCTL0 = 0;                    // Reset CCTL
}

#pragma vector=PORT1_VECTOR
__interrupt void port1_passive_isr(void)
{
    P1IFG = 0;        // Clear interrupt flag
    P2IFG = 0;        // Clear interrupt flag
    P2IE = 0; // disable isr
    P1IE = 0; // disable isr

    matrix_passive_scan(0);
    __bic_SR_register_on_exit(LPM4_bits); // change the run mode from low power mode 4 to 3
    __bis_SR_register_on_exit(LPM3_bits); // on exit in order to start aclk, and run periodically
    TACTL |= TAIFG; // run the loop isr after this in order to scan the matrix
    TA0CTL = TASSEL_1 + MC_1;
}
#pragma vector=PORT2_VECTOR
__interrupt void port2_passive_isr(void)
{
    P1IFG = 0;        // Clear interrupt flag
    P2IFG = 0;        // Clear interrupt flag
    P2IE = 0; // disable isr
    P1IE = 0; // disable isr

    matrix_passive_scan(0);
    __bic_SR_register_on_exit(LPM4_bits); // change the run mode from low power mode 4 to 3
    __bis_SR_register_on_exit(LPM3_bits); // on exit in order to start aclk, and run periodically
    TACTL |= TAIFG; // run the loop isr after this in order to scan the matrix
    TA0CTL = TASSEL_1 + MC_1;
}

// Timer A Interrupt Service Routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timer_loop_isr (void) {
    const pingroup_t *sgrps;
    unsigned int sgrps_size;
    bool scan_value;

    TA0CTL = MC_0; // stop timer in interrupt
    __bis_SR_register(GIE); // allow interrupts in order for uart to function
    bool active = matrix_scan(&sgrps, &sgrps_size, &scan_value); // actively scan the keys in the matrix
    if (active) {
        // Timer_A Configuration
        // TASSEL_1: ACLK 
        // MC_1: Up mode (counts from 0 to TACCR0)
        TA0CTL = TASSEL_1 + MC_1;
    } else { // unlikely
        __bic_SR_register(GIE); // allow interrupts in order for uart to function
        for (int i = 0; i < sgrps_size; i++) {
            if (sgrps[i].group == PORT1) {
                P1IES = sgrps[i].pins*!scan_value;
                P1IE = sgrps[i].pins;
                P1IFG = 0;
            }
            if (sgrps[i].group == PORT2) {
                P2IES = sgrps[i].pins*!scan_value;
                P2IE = sgrps[i].pins;
                P2IFG = 0;
            } // skip if it is not a supported port, any event on such a port won't wake up the cpu

        }
        matrix_passive_scan(1); // set up the pins for passive scan after the interrupts are set up to avoid race conditions.
        __bic_SR_register_on_exit(LPM3_bits); // change the run mode from low power mode 3 to 4
        __bis_SR_register_on_exit(LPM4_bits); // on exit in order to stop aclk, and only interrupt if we get an event
    }
    // After this ISR ends, the CPU automatically returns to the sleep state 
    // SR is reset, so OSCOFF and GIE are set to the value outside the isr
    // unless they were modified by the isr
    // if data is still being sent then aclk will continue to run until no data is left in the buffer
}

int main(void)
{
    // Stop Watchdog
    WDTCTL = WDTPW | WDTHOLD;

    P1DIR = -1; // set as output in order to not leave floating inputs
    P2DIR = -1;

    P1IE = 0;
    P2IE = 0;

    P1IFG = 0;
    P2IFG = 0;

    P1SEL = 0;
    P1SEL2 = 0;
    P2SEL = 0;
    P2SEL2 = 0;

    ADC10AE0 = 0;
    CAPD = 0;
    // Clock Setup
    BCSCTL3 = LFXT1S_2; // Set ACLK to use internal VLO (4kHz-20kHz, ~12kHz)
    calibrate_vlo();
    // Set DCO to 8MHz
    BCSCTL1 = CALBC1_8MHZ;
    DCOCTL = CALDCO_8MHZ;

    // Initialize Peripherals
    matrix_init();
    uart_init();

#   ifdef LED
    // Blink LED once
    setPinDir(((pingroup_t)LED), 1);
    setPinState(((pingroup_t)LED), HIGH);
    mdelayc(500);
    setPinState(((pingroup_t)LED), LOW);
#   endif

    TA0CCR0 = KSCAN_SCAN_PERIOD*1000/vlo_period; // set clock interval
    // Timer_A Configuration
    // TASSEL_1: ACLK 
    // MC_1: Up mode (counts from 0 to TACCR0)
    TA0CTL = TASSEL_1 + MC_1;
    TA0CCTL0 = CCIE;                            // Enable interrupt for CCR0

    while(1) __bis_SR_register(LPM3_bits + GIE);     // Enter Low Power Mode 3 with interrupts in a loop in case of accidental exit
                                                     // Execution STOPS here until an ISR triggers
}
#endif
