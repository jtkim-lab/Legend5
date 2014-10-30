/*
   These are Block Project Codes
 ***********************************************************************
		 Raspberry Pi - AVR#1 - AVR#2 - ... - AVR#n
 ***********************************************************************
 - init.c
 : This file includes main() function.

 - lcd_22.h, lcd_22.c
 : LCD API

 - parse.c
 : link grammer
 ***********************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <wiringPi.h>
#include <wiringSerial.h>

#include "lcd_22.h"

#define NUMOFBLKS 32

unsigned char ID_arrayc[NUMOFBLKS] = {0, }; 	//samle ID array "I love a pretty girl"
unsigned char ID_cmp_array[NUMOFBLKS] = {0,} ;

int block_cnt=5;

int en_BT = RPI_GPIO_P1_16 ;		//pin 16 send to BT
int en_AT = RPI_GPIO_P1_18 ;		//pin 18 send to ATTINY

// 100: Ready (rpi <- app)
// 101: Ack (rpi <-> app)
// 102: Start (rpi -> app)
// 103: End (rpi -> app)

int main (char argc, char * argv[])
{
	// R-Pi Serial
	int fd ;
	unsigned char last_check = '!' ;
	unsigned char ack = '@' ;
	unsigned char start_bit = '^' ;
	unsigned char stop_bit = '*' ;

	unsigned char temp ;
	int result=0;
	int i = 0 ;
	int count = 0 ;// id_array length
	unsigned int nextTime ;

	int flag_refresh = 0 ;
	int error_cnt = 0;

	int temp_cnt = 0 ;
	int loop_cnt = 0 ;
	int pairing_flag = 0;

	if(!bcm2835_init()) return 1 ;

	//SET GPIO OUTPUT PIN
	bcm2835_gpio_fsel(en_BT, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(en_AT, BCM2835_GPIO_FSEL_OUTP);

	if ((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0)
	{
		fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
		return 1 ;
	}

	if (wiringPiSetup () == -1)
	{
		fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
		return 1 ;
	}

	// BUFFER TEST 
	for(i=0;i<10;i++){
		bcm2835_gpio_write(en_AT,LOW); 
		bcm2835_gpio_write(en_BT,HIGH); 
		delay(10);

		bcm2835_gpio_write(en_AT,HIGH); 
		bcm2835_gpio_write(en_BT,LOW); 
		delay(10);
	}

	// INITIALIZE
	load_lg();//Load link grammer

	init_lcd(); //Initialize LCD
	image_display(0); //Display waiting mode LCD

	memset(ID_arrayc, NUMOFBLKS, '*');

	// RUNNING
	while(1){
		loop_cnt++ ;
		
		//BUFFER CONTROL en_AT = LOW, en_BT = HIGH
		bcm2835_gpio_write(en_AT,LOW); 
		bcm2835_gpio_write(en_BT,HIGH); 


		//SEND LAST_CHECK_BIT
		serialPutchar ( fd, last_check ) ;
		printf("I sent last_check: '!' \n");

		//WAIT UNTIL R-PI GOT ACK_BIT
		while( serialGetchar (fd) != ack){
			serialPutchar ( fd, '!' ) ;
			printf("---Re send the last_check\n");
		}
		printf("********I got ack bit***********\n") ;


		//WAIT UNTIL R-PI GOT START_BIT
		temp_cnt=0;
		while( serialGetchar(fd) != start_bit && temp_cnt < 3 ){
			printf("%d Waiting for start bit\n",temp_cnt) ;
			temp_cnt++ ;
		}
		printf("********I got start bit***********\n") ;


		//WAIT UNTIL R-PI GOT STOP BIT
		count = 0;
		while((ID_arrayc[count]=serialGetchar(fd)) != stop_bit && count < 32 ){ //32
			if(ID_arrayc[count] == 255) break; 
			printf(":::%d\n",ID_arrayc[count]); //print ID array
			count++ ;
		}
		printf("********I got stop bit***********\n") ;
		printf("total count: %d\n",count);


		//COMPARE ORIGIN ARRAY WITH PREV_ARRAY
		flag_refresh = 0 ;
		for( i = 0 ; i < count ; i++ )
		{
			if( ID_arrayc[i] != ID_cmp_array[i] ){
				flag_refresh = 1 ;
			}
			ID_cmp_array[i] = ID_arrayc[i] ;
		}

		if(count != 5)
			error_cnt++;

		printf("total loop : %d, error count : %d \n",loop_cnt,error_cnt) ;


		result = 0 ;
		if(count < 32 && flag_refresh == 1 ){
			printf("----------check grammar start----------\n") ;
			result=sentence_processing(ID_arrayc, count);
			printf("----------check grammar end----------\n") ;
		}


		bcm2835_gpio_write(en_BT,LOW); 
		bcm2835_gpio_write(en_AT,HIGH); 

		pairing_flag=0;
		temp_cnt=0;
		while(1){
			printf("BT waiting: %d\n",temp_cnt);
			temp = serialGetchar(fd); //1 sec if there is no result
			if(temp == 100){
				pairing_flag=1;
				break;
			}else if(temp_cnt==1){//need to be change (timer counter)
				break;
			}
			temp_cnt++;
		}

		//Pairing Mode!!
		// 100, d: Ready (rpi <- app)
		// 101, e: Ack (rpi <-> app)
		// 102, f: Start (rpi -> app)
		// 103, g: End (rpi -> app)
		if(pairing_flag==1){
			printf("BT start..!\n");

			//SEND ACK TO APP
			serialPutchar(fd, 101);
			printf("Ack is sent..!\n");

			//WAIT UNTIL START BYTE 
			while((temp = serialGetchar(fd)) != 102); //timer counter
			printf("Start is recieved..!\n");

			//IF START BYTE IS ARRIVED, SEND DATA
			
			// (0) Total Count
			printf("total count:%d is sent\n",count);
			serialPutchar(fd, count);
			serialPutchar(fd, '/');

			// (1) DATE
			serialPutchar(fd, 2); //NEED TO BE REPLACED WITH Sqlite
			serialPutchar(fd, 0);
			serialPutchar(fd, 1);
			serialPutchar(fd, 4);
			serialPutchar(fd, 1);
			serialPutchar(fd, 0);
			serialPutchar(fd, 2);
			serialPutchar(fd, 9);
			serialPutchar(fd, '/');
			printf("DATE is sent");

			// (2) T/F
			serialPutchar(fd, result);
			serialPutchar(fd, '/');
			printf("T/F is sent");

			// (3) ARRAY
			for( i = 0 ; i < count ; i++)
			{
				printf("ID_arrayc[%d]: %d is sent \n",i, ID_arrayc[i]);
				serialPutchar (fd, ID_arrayc[i]);
			}
			serialPutchar(fd, '/');

			printf("BT done..!\n");
		}
	}

	// TERMINATING
	unload_lg();//Unload link grammer
	bcm2835_close() ;
	return 0 ;
}
