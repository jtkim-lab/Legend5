/* -----------------------------------------------------------------------
 * uart.c
 * baudrate 9600
 * adapted to ATtiny2313
 */

#include <inttypes.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "uart.h"


/*
 * init_uart
 */
void init_uart(void) {
	unsigned char x=0;

	rx_head=x;
	rx_tail=x;
	tx_head=x;
	tx_tail=x;

	// set baud rate
	UBRRH = (char)(MYUBBR >> 8); 
	UBRRL = (char)(MYUBBR);

	// enable receive and transmit
	UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
	// set frame format
	UCSRC = (1 << USBS) | (3 << UCSZ0);	// asynchron 8n1
}

/*
 * uart_getc
 * Gets a single char from the receive buffer.
 * return	uint16_r	the received char or UART_NO_DATA 
 */
char uart_getc(void) {
	char c = 0;
	char tmp_tail = 0;
	if (rx_head == rx_tail) {
		return UART_NO_DATA;
	}
	tmp_tail = (rx_tail + 1) % BUFFER_SIZE;
	c = rx_buffer[rx_tail];
	rx_tail = tmp_tail;
	return c;
}



/*
 * uart_getc_wait
 * Blocking call to getc. Will not return until a char is received.
 */
char uart_getc_wait(void) {
	uint16_t c;
	while ((c = uart_getc()) == UART_NO_DATA) {}
	return c;
}



/*
 * uart_putc
 * Puts a single char. Will block until there is enough space in the
 * send buffer.
 */
void uart_putc(char c) {
	char tmp_head = (tx_head + 1) % BUFFER_SIZE;
	// wait for space in buffer
	while (tmp_head == tx_tail) {
		;
	}
	tx_buffer[tx_head] = c;
	tx_head = tmp_head;

	// enable uart data interrupt (send data)
	UCSRB |= (1<<UDRIE);

	// Wait until TX complete
	while(!TEST_BIT(UCSRA,TXC));
	UCSRA |= (1<<TXC);
}



/*
 * uart_putc_f
 * Puts a single char. Used by printf functions.
 */
int uart_putc_f(char c, FILE *stream) {
	uart_putc(c);
	return 0;
}



/*
 * ISR User Data Regiser Empty
 * Send a char out of buffer via UART. If sending is complete, the 
 * interrupt gets disabled.
 */
ISR(USART_UDRE_vect) {
	char tmp_tail = 0;
	if (tx_head != tx_tail) {
		tmp_tail = (tx_tail + 1) % BUFFER_SIZE;
		UDR = tx_buffer[tx_tail];
		tx_tail = tmp_tail;
	}
	else {
		// disable this interrupt if nothing more to send
		UCSRB &= ~(1 << UDRIE);
	}
}


/*
 * ISR RX complete
 * Receives a char from UART and stores it in ring buffer.
 */
ISR(USART_RX_vect) {
	char tmp_head = 0;
	tmp_head = (rx_head + 1) % BUFFER_SIZE;
	if (tmp_head == rx_tail) {
		// buffer overflow error!
	}
	else {
		rx_buffer[rx_head] = UDR;
		rx_head = tmp_head;
	}
}

