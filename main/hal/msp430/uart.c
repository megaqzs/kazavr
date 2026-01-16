#ifdef ENERGIA_MSP_EXP430G2453
#include <msp430.h>
#include <config.h>
#include <stdint.h>
#include <stdio.h>
#include <hal/uart.h>
#include "config.h"

#define BUF_SIZE_BITS 6
typedef struct {
    char buff[1 << BUF_SIZE_BITS];
    volatile uint8_t i;
    volatile uint8_t j;
} rbuff_t;

static rbuff_t tx = {{0}, 0, 0};

// Tie putchar() to printf
int putchar(int c) {
    uint8_t newj = (tx.j+1) % sizeof(tx.buff);
    while (newj == tx.i);
    tx.buff[tx.j] = c;
    tx.j = newj;
    IE2 |= UCA0TXIE;                       // Enable USCI_A0 TX interrupt since we added a character to the buffer
    return c;
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void) {
    // Write the character to the TX buffer
    UCA0TXBUF = tx.buff[tx.i];
    tx.i = (tx.i+1) % sizeof(tx.buff);
    if (tx.i == tx.j)              // TX over?
        IE2 &= ~UCA0TXIE;          // Disable USCI_A0 TX interrupt
}

#define FREQ F_CPU
#define BMULT (FREQ/BAUD)
#define BMOD ((16*FREQ+BAUD)/BAUD/2-8*BMULT)

bool tx_busy(void) {
    return IE2 & UCA0TXIE;
}

void uart_init(void) {
    P1SEL  |= BIT1 | BIT2;      // Set P1.1 RX and P1.2 TX
    P1SEL2 |= BIT1 | BIT2;

    UCA0CTL1 |= UCSWRST;        // Hold USCI in reset
    UCA0CTL1 |= UCSSEL_2;       // SMCLK

    UCA0BR0 = BMULT&0xFF;       // baud rate multiplier
    UCA0BR1 = BMULT>>8;

    UCA0MCTL = !!(BMOD&1)*0b00000001 + !!(BMOD&2)*0b01000100 + !!(BMOD&4)*0b10101010;          // Modulation

    UCA0CTL1 &= ~UCSWRST;       // Release reset
    IE2 &= ~UCA0TXIE;     // ensure TX interrupt disabled initially
    //__bis_SR_register(LPM0_bits + GIE);       // Enter int until Bytes TXed
}
#endif
