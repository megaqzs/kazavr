#ifdef ENERGIA_MSP_EXP430G2453
#include <msp430.h>
#include <config.h>
#include <stdint.h>
#include <stdio.h>
#include <hal/uart.h>

#define BUF_SIZE_BITS 5
typedef struct {
    char buff[1 << BUF_SIZE_BITS];
    volatile uint8_t i;
    volatile uint8_t j;
} rbuff_t;

static rbuff_t tx = {{0}, 0, 0};

// Tie putchar() to printf
int putchar(int c)
{
    uint8_t newj = (tx.j+1) % sizeof(tx.buff);
    while (newj == tx.i)
        __no_operation();
    tx.buff[tx.j] = c;
    tx.j = newj;
    IE2 |= UCA0TXIE;                       // Enable USCI_A0 TX interrupt since we added a character to the buffer
    return c;
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
    // Write the character to the TX buffer
    UCA0TXBUF = tx.buff[tx.i];
    tx.i = (tx.i+1) % sizeof(tx.buff);
    if (tx.i == tx.j)              // TX over?
        IE2 &= ~UCA0TXIE;          // Disable USCI_A0 TX interrupt
}

// UART init – example for SMCLK @ 1 MHz, 9600 baud
void uart_init(void)
{
    WDTCTL = WDTPW + WDTHOLD;   // Stop WDT
    P1SEL  |= BIT1 | BIT2;      // Set P1.1 RX and P1.2 TX
    P1SEL2 |= BIT1 | BIT2;

    UCA0CTL1 |= UCSWRST;        // Hold USCI in reset
    UCA0CTL1 |= UCSSEL_2;       // SMCLK

    // 9600 baud at 1 MHz
    UCA0BR0 = 104;              // 1,000,000 / 9600 ≈ 104.17
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0;          // Modulation

    UCA0CTL1 &= ~UCSWRST;       // Release reset
    IE2 &= ~UCA0TXIE;     // ensure TX interrupt disabled initially
    //__bis_SR_register(LPM0_bits + GIE);       // Enter int until Bytes TXed
}
#endif
