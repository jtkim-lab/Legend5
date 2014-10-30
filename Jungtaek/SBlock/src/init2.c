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

char ID_arrayc[32] = {1,8,9,10,6,}; //samle ID array "I love a pretty girl"
int block_cnt=5;

int main (char argc, char * argv[])
{
	int testflag=0;

	// R-Pi Serial
	int fd ;
	int count ;
	unsigned int nextTime ;

	if ((fd = serialOpen ("/dev/ttyAMA0", 115200)) < 0)
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
		//1. Serial Communication
		serialPutchar(fd, '!'); //send 'last check bit' to AVR
		
		delay(3);

		//need to wait until the array would +arrive.
		printf("%d\n",serialDataAvail(fd));
		while(1){
			if(serialGetchar(fd)=='@') break;
			while(serialDataAvail(fd)){
				printf("%c\n",serialGetchar(fd));
			}
			fflush(stdout);
		}

		// Send Signal by Using UART

		//2. Touch Sensing
		read_res=Read_Continue();
		if(!(touch_dot.x==0&&touch_dot.y==0)){
			//touch event activated!

			//<---------JUST FOR THE TEST. NEED TO BE CHANGED-------->
			if(testflag==0){
				block_cnt=5;

				testflag=1;
			}else{
				block_cnt=4;

				testflag=0;
			}
			//<---------JUST FOR THE TEST. NEED TO BE CHANGED-------->

			//1. make sentence by using ID array

			//2. sentence processing
			sentence_processing(ID_arrayc,block_cnt);
		}

		touch_dot.x=0;
		touch_dot.y=0;
	}

	// TERMINATING
	unload_lg();//Unload link grammer

	return 0 ;
}
