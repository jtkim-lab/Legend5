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

char ID_arrayc[32] = {1,8,9,10,6,}; 	//samle ID array "I love a pretty girl"
int block_cnt=5;

int en_BT = RPI_GPIO_P1_07 ;		//pin 7 send to BT
const int en_AT = RPI_GPIO_P1_15 ;	//pin 11 send to ATTINY


int main (char argc, char * argv[])
{
	int testflag=0;

	// R-Pi Serial
	int fd ;
	char last_check = '!' ;
	char ack = '@' ;
	char start_bit = '^' ;
	char stop_bit = '*' ;
	char temp ;
	int result=0;
	int i = 0 ;
	int count = 0 ;// id_array length
	unsigned int nextTime ;


	char a = 'a' ;

	if(!bcm2835_init())
		return 1 ;

	bcm2835_gpio_fsel(en_BT, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(en_AT, BCM2835_GPIO_FSEL_OUTP);
	
	bcm2835_gpio_write(en_AT,HIGH); 
	bcm2835_gpio_write(en_BT,HIGH); 
	sleep(1);


	bcm2835_gpio_write(en_AT,LOW); 
	bcm2835_gpio_write(en_BT,LOW); 
	sleep(1);

//	bcm2835_close() ;
	

	printf("1111\n") ;
//	pinMode(en_BT, OUTPUT) ;
	printf("2222\n");
//	pinMode(en_AT, OUTPUT) ;
 

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

	// INITIALIZE
	load_lg();//Load link grammer

	init_lcd(); //Initialize LCD
	image_display(0); //Display waiting mode LCD

	// RUNNING
	while(1){
		result = 0;
		count = 0 ;

	//Touch Sensing
		read_res=Read_Continue();

		if(!(touch_dot.x==0&&touch_dot.y==0)){
			printf("Touch event activated!\n") ;
			
			if(!bcm2835_init())
				return 1 ;
			bcm2835_gpio_fsel(en_BT, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_fsel(en_AT, BCM2835_GPIO_FSEL_OUTP);
	

//			//BUFFER CONTROL en_AT = LOW, en_BT = HIGH
			bcm2835_gpio_write(en_AT,LOW); 
			bcm2835_gpio_write(en_BT,HIGH); 
			
//			bcm2835_close() ;
	
			//SEND LAST_CHECK_BIT
			serialPutchar ( fd, last_check ) ;
			printf("I sent last check bit\n");

			//WAIT UNTIL R-PI GOT ACK_BIT
			while( serialGetchar (fd) != ack)
			{
				printf("Here is ACK waiting loop \n") ;
				fflush (stdout) ;
			}
			printf("********I got ack bit***********\n") ;

			//WAIT UNTIL R-PI GOT START_BIT
			temp = serialGetchar (fd) ;
			while( temp != start_bit )
			{
				temp = serialGetchar (fd) ;
			}
			printf("********I got start bit***********\n") ;

			//WAIT UNTIL R-PI GOT STOP BIT
			while(serialDataAvail(fd) && 
			      ((ID_arrayc[count]=serialGetchar(fd)) != stop_bit) ){
				count++ ;
			}
			printf("********I got stop bit***********\n") ;


			printf("total count: %d\n",count);
		
			printf("******abddfasdfasdfasdfsdafsdafsdafas^^^^^^^^^^^^^^^^\n");
			
			
			for( i = 0 ; i < count ; i++ )
			{
				printf("id arr %d is %c\n", i, ID_arrayc[i]) ; 
			}
			
		//	result=sentence_processing(ID_arrayc, count);
  			printf("***************************\n");	
//			//BUFFER CONTROL en_AT = HIGH, en_BT = LOW
			if(!bcm2835_init())
				return 1 ;

			bcm2835_gpio_fsel(en_BT, BCM2835_GPIO_FSEL_OUTP);
			bcm2835_gpio_fsel(en_AT, BCM2835_GPIO_FSEL_OUTP);
			printf("------------------------------\n") ;	
			bcm2835_gpio_write(en_BT,LOW); 
			bcm2835_gpio_write(en_AT,HIGH); 
//			bcm2835_close() ;
	
			//if(result==1){
				//If it's correct RESULT to BT
				printf("It`s time to send id_arr to android by BT\n") ;
				for( i = 0 ; i < count ; i++)
				{
					fflush(stdout) ;
				//	serialPutchar (fd, ID_arrayc[i]);
					serialPutchar (fd, a) ;
					printf("%c \n",ID_arrayc[i]);
				}
				printf("BT sending is done\n") ;
			//}

		}
		touch_dot.x=0;
		touch_dot.y=0;
	}

	// TERMINATING
	unload_lg();//Unload link grammer
	bcm2835_close() ;
	return 0 ;
}
