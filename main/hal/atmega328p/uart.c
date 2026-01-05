#ifdef __AVR_ATmega328P__
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <pins.h>
#include <config.h>
#include <hal/uart.h>
#define TX_BUFFER_SIZE 32

static volatile char tx_buffer[TX_BUFFER_SIZE];
static volatile uint8_t tx_head = 0;
static volatile uint8_t tx_tail = 0;

// 1. Data Register Empty ISR: Called whenever the UART is ready for a new byte
ISR(USART_UDRE_vect) {
    if (tx_head != tx_tail) {
        UDR0 = tx_buffer[tx_tail];
        tx_tail = (tx_tail + 1) % TX_BUFFER_SIZE;
    } else {
        // Buffer empty, disable this interrupt
        UCSR0B &= ~(1 << UDRIE0);
    }
}

static int uart_putchar(char c, FILE *stream)
{
    uint8_t next_head = (tx_head + 1) % TX_BUFFER_SIZE;

    // Wait if buffer is full (simple flow control)
    //while (next_head == tx_tail);

    tx_buffer[tx_head] = c;
    tx_head = next_head;

    // Enable the "Data Register Empty" interrupt to start sending
    UCSR0B |= (1 << UDRIE0);
    return c;
}

// Create a FILE stream for stdout
FILE uart_output = FDEV_SETUP_STREAM(uart_putchar, NULL, _FDEV_SETUP_WRITE);

bool tx_busy(void) {
    return UCSR0B & UDRIE0;
}

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
#endif
