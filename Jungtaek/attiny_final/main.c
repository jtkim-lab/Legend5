/* -----------------------------------------------------------------------
 * Title:    Smart Blocks
 * Date:     2014.10.
 * Hardware: ATtiny2313
 * Software: AVRdude
 */

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include "uart.h"

#define LED_BIT PD5
#define LED_BIT2 PD4
#define BUFFER_R 0xFE
#define BUFFER_L 0xFD
#define NUMBER_OF_BLKS 33

#define LAST_CHECK_BYTE '!'
#define ACK_BYTE '@'
#define DATA_START_BYTE '^'
#define DATA_END_BYTE '*'

#define TICKS_PER_SEC 1000

unsigned int timer_cnt;

ISR(TIMER0_OVF_vect)
{
	timer_cnt++;
	TCNT0 = 256 - (CPU_CLOCK / TICKS_PER_SEC / 64);
}

void init_tc(void)
{
	TCCR0A = (0 << WGM01) | (0 << WGM00);
	TCCR0B = (0 << CS02) | (1 << CS01) | (1 << CS00);
	TCNT0 = 256 - (CPU_CLOCK / TICKS_PER_SEC / 64);
	TIMSK = (0 << OCIE0B) | (0 << OCIE0A) | (1 << TOIE0);
	TIFR = (0 << OCF0B) | (0 << OCF0A) | (1 << TOV0);
}

/*
1 I
2 YOU
3 HE

4 AM
5 ARE
6 IS

7 LOVE(S)
8 MEET(S)
9 MAKE(S)
10 DO(ES)

11 A
12 AN
13 THE

14 PRETTY
15 SMALL

16 PRINCESS
17 DWARFS
18 MIRROR
19 APPLE
20 CAKE
*/

int main(void) {

	DDRD=0xff; //output D port
	PORTD=0x00; //off LEDs

	unsigned char MY_BLK_ID=3; 
	unsigned char i = 0;
	unsigned char c=0;
	unsigned char cnt=0;
	unsigned static char ID_array[NUMBER_OF_BLKS];//Static?
	unsigned static char error_flag = 1;

	//ID_array init
	for( i = 0; i < NUMBER_OF_BLKS; i++ ){
		ID_array[i] = '*' ;
	}

	//START (for LED debug)
	for (i = 0; i < 10; i++) {
		PORTD |= (1 << LED_BIT);
		_delay_ms(50);
		PORTD &= ~(1 << LED_BIT);
		_delay_ms(50);
	}

	//INITIALIZE
	init_uart();
	init_tc();

	//SET INTERRUPT
	sei();

	//buffer setting, right
	DDRB = 0x03;
	PORTB = BUFFER_R;

	while(1){
		cnt = 0;

		while((c = uart_getc())!=LAST_CHECK_BYTE);
		_delay_ms(100);

		//I send ack_bit to left
		uart_putc(ACK_BYTE);

		//buffer change
		PORTB = BUFFER_L;

		//send last_check_bit to right
		timer_cnt = 0 ;

		_delay_ms(100);
		uart_putc(LAST_CHECK_BYTE) ;

		unsigned char last_one=1;
		timer_cnt = 0 ;
		while(timer_cnt < 100){

			if( (c=uart_getc()) == ACK_BYTE){ //0.1
				//I'm not the last one
				last_one=0;
				break;
			}

		}

		if(last_one==1){
			//buffer change
			PORTB = BUFFER_R;
			_delay_ms(100);

			//I'm the last one, send array to the left
			uart_putc(DATA_START_BYTE);
			uart_putc(MY_BLK_ID);
			uart_putc(DATA_END_BYTE);
		}else{
			//I'm not the last. wait for array start bit
			timer_cnt = 0 ;
			error_flag = 1;
			while(timer_cnt < 1000){
				if((c = uart_getc()) == DATA_START_BYTE){ //0.1
					//START BYTE is arrived
					error_flag = 0;
					break;
				}
			}

			if(error_flag==1){
				PORTB = BUFFER_R;
				continue;
			}

			while(1){
				c=uart_getc(); //for debug
				if(c==0) continue; // Dummy data

				ID_array[cnt] = c;
				if(c == DATA_END_BYTE || cnt == 32) break;
				cnt++;
			}

			//buffer change
			PORTB = BUFFER_R;

			//send my ID array to left
			_delay_ms(100);

			uart_putc(DATA_START_BYTE);
			for(i=0; i<cnt; i++){
				uart_putc(ID_array[i]);
			}
			uart_putc(MY_BLK_ID);
			uart_putc(DATA_END_BYTE);
		}

	}
	return 0;
}

