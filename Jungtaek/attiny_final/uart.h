#ifndef UART_H_
#define UART_H_

#include <stdio.h>

#define UART_NO_DATA 0x0100
#define TEST_BIT(ADDRESS,BIT) (((ADDRESS & (1<<BIT))==0)?0:1)
#ifndef TRUE
#define TRUE 1
#define FALSE 0 
#endif

#define CPU_CLOCK   16000000
//#define BAUD 19200
#define BAUD 9600
// 4.096MHz
//  4800: 52.3333333
//  9600: 25.6666667
// 14400: 16.7777778
// 19600: 12.06
// 28800: 7.8889
// 38400: 5.6667

#define MYUBBR ((CPU_CLOCK / (BAUD * 16L)) - 1)
#define BUFFER_SIZE 16

volatile static char rx_buffer[BUFFER_SIZE] = "****************";
volatile static char tx_buffer[BUFFER_SIZE] = "****************";
volatile static char rx_head = 0;
volatile static char rx_tail = 0;
volatile static char tx_head = 0;
volatile static char tx_tail = 0;
volatile static char sent = TRUE;


// Initialize UART to 9600 baud with 8N1. 
void init_uart(void);

// Send and receive functions, that run without ISRs
char receive_uart();
void send_uart(char c);

// Receive a single char or UART_NO_DATA, if nothing received
char uart_getc(void);
// Blocking call to receive a char
char uart_getc_wait(void);
int uart_getc_f(FILE *stream);

// Send a single char
void uart_putc(char c);
int uart_putc_f(char c, FILE *stream);

// Send a string
void uart_puts(const char *s);
// Send a PROGMEM string
void uart_puts_P(const char *s);

#endif /*UART_H_*/
