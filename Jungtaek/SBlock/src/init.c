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
#include <stdlib.h>

#include <wiringPi.h>
#include <wiringSerial.h>

//JT
#include <sqlite3.h>

#include "lcd_22.h"

#define NUMOFBLKS 32

//JT
char dbID[100];
char dbTime[100];
char dbIDArray[100];
char dbCorrect[100];

//JT
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;

	for(i = 0; i < argc; i++)
	{   
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");

		if(!strcmp(azcolname[i], "ID"))
		{
			strcpy(dbID, argv[i]);
		}

		if(!strcmp(azcolname[i], "TIME"))
		{
			strcpy(dbTime, argv[i]);
		}

		if(!strcmp(azcolname[i], "CORRECT"))
		{
			strcpy(dbCorrect, argv[i]);
		}

		if(!strcmp(azcolname[i], "IDARRAY"))
		{
			strcpy(dbIDArray, argv[i]);
		}
	}   

	printf("\n");

	return 0;
}


/*
char WordTable[MAX_BLOCKS][10]={
	"",			//0
	"I",			//1
	"you",			//2
	"he",			//3

	"am",			//4
	"are",			//5
	"is",			//6

	"love",			//7 loves
	"meet",			//8 meets
	"make",			//9 makes	
	"do",			//10 do

	"a",			//11
	"an",			//12
	"the",			//13

	"pretty",		//14
	"small",		//15

	"princess",		//16
	"dwarfs",		//17
	"mirror",		//18
	"apple",		//19
	"cake",			//20

	//if there are any block between 7-10, check again with 21-24 (+14)
	"loves",		//21
	"meets",		//22
	"makes",		//23
	"does"			//24
};
*/

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
	int double_check_flag = 0;

	//JT
	sqlite3 *db;
	int ret;
	char* sql;
	char* err = 0;
	const char* data = "Callback function is called";
	
	ret = sqlite3_open("Legend5.db", &db);

	if(ret)
	{
		return 1;
	}

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
			serialPutchar ( fd, last_check ) ;
			printf("No ACK BYTE, Re-send the last_check\n");
		}
		printf("********I got ack bit***********\n") ;


		//WAIT UNTIL R-PI GOT START_BIT
		temp_cnt=0;
		while( serialGetchar(fd) != start_bit && temp_cnt < 2){
			printf("%d Waiting for start bit\n",temp_cnt) ;
			temp_cnt++ ;
		}
		printf("********I got start bit***********\n") ;


		//WAIT UNTIL R-PI GOT STOP BIT
		count = 0;
		double_check_flag = 0;
		while((ID_arrayc[count]=serialGetchar(fd)) != stop_bit && count < 32 ){ //32
			if(ID_arrayc[count] == 255) break; 

			if(ID_arrayc[count] == 7 ||
			   ID_arrayc[count] == 8 ||
			   ID_arrayc[count] == 9 ||
			   ID_arrayc[count] == 10){
				double_check_flag=1;
			}

			printf(":::%d\n",ID_arrayc[count]); //print ID array
			count++ ;
		}
		if(ID_arrayc[count] == 255){
			error_cnt++;
			continue; //error handling 
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

		if(count != 6)
			error_cnt++;

		printf("total loop : %d, error count : %d \n",loop_cnt,error_cnt) ;


		result = 0 ;
		if(count < 32 && flag_refresh == 1 ){
			if(double_check_flag==1){
				printf("----------double check grammar start----------\n") ;
				result=sentence_processing(ID_arrayc, count, 1);

				if(result==0){
					for(i=0;i<count;i++){
						if(ID_arrayc[i] == 7 ||
						   ID_arrayc[i] == 8 ||
						   ID_arrayc[i] == 9 ||
						   ID_arrayc[i] == 10){
							ID_arrayc[i]+=14;
						}
					}
					result=sentence_processing(ID_arrayc, count, 0);
				}
			}else{
				printf("----------only once check grammar start----------\n") ;
				result=sentence_processing(ID_arrayc, count, 0);
			}
			printf("----------check grammar end----------\n") ;

			//JT
			sql = "INSERT INTO SENTENCE (CORRECT, IDARRAY) VALUES (";
			if (result == 0)
				strcat(sql, "0, '");
			else
				strcat(sql, "1, '");

			strcat(sql, ID_arrayc);
			strcat(sql, "');");

			ret = sqlite3_exec(db, sql, callback, 0, &err);

			if(ret != SQLITE_OK)
			{
				printf("JT: %s\n", err);
				return 1;
			}
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
		// 102, f: Start (rpi <- app)
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
			
			//JT
			sql = "SELECT * from COMPANY";

			ret = sqlite3_exec(db, sql, callback, (void*) data, &err);

			if(ret != SQLITE_OK)
			{
				printf("JT: %s\n", err);
				return 1;
			}

			// (0) Total Count
			printf("total count:%d is sent\n",count);
			serialPutchar(fd, 1); //Number of datas
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
