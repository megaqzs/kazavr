#include <avr/io.h>
#include <stdio.h>
#include <config.h>
#include <uart.h>
static int uart_putchar(char c, FILE *stream)
{
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));

    // Put data into buffer, sends the character
    UDR0 = c;
    return 0;
}

// Create a FILE stream for stdout
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

// Initialize UART
void uart_init(void)
{
	// setup the serial interface
#	include <util/setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#	if USE_2X
		UCSR0A |= (1 << U2X0);
#	else
		UCSR0A &= ~(1 << U2X0);
#	endif

    // Enable transmitter
    UCSR0B = (1 << TXEN0);

    // Set frame format: 8 data bits, 1 stop bit
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    // Redirect stdout to UART
    stdout = &uart_output;
}

