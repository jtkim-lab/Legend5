/*
 *========================================================================================================
 *
 * File                : lcd_22.C
 * Hardware Environment:
 * By                  : jack ben
 *
 *                                  (c) Copyright 2011-2016, WaveShare
 *                                       http://www.waveShare.net
 *                                          All Rights Reserved
 *
 *========================================================================================================
 */

#include "lcd_22.h"

#include <bcm2835.h>
#include <stdio.h>
#include "asciihex8X16.h"

//If you want to use new Image file, make header file by using "pyxel" program. then add it.
#include "trueimg.h"
#include "falseimg.h"
#include "waitimg.h"

#define READ_TIMES 10 //
#define LOST_VAL 4        //
const u16 colorfol[]={0xf800,0x07e0,0x001f,0xffe0,0x0000,0xffff,0x07ff,0xf81f};

void init_lcd_spi(void)
{    
	bcm2835_init();

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default

	bcm2835_gpio_fsel(RPI_GPIO_P1_16, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(RPI_GPIO_P1_22, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(RPI_GPIO_P1_24, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(RPI_GPIO_P1_26, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(PENIRQ, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(PENIRQ, BCM2835_GPIO_PUD_UP);///////////////////////

	printf("\n spi-lcd22 test :\n Start .......\n  ");

}

void init_touch_spi(void)
{
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE3);                   // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default

}

#if defined(LCD22_SPI)
void post_data(u16 data)
{
	en_lcd();
	bcm2835_spi_transfer((u8)(data>>8));
	//delayMicroseconds(12);
	bcm2835_spi_transfer((u8)(data));
	//delayMicroseconds(12);
}


#endif

void lcd_rst(void) 
{
	reset_clr;
	delayMicroseconds(100);
	reset_set;
	delayMicroseconds(100);
}

void post_cmd(u16 index, u16 cmd)
{
	en_lcd_index();
	post_data(index);
	en_lcd_data();
	post_data(cmd);
}

void init_lcd(void)
{
	u16 x, y;/*
		    touch_counter = 0;
		    touch_wr_index = 0;
		    touch_rd_index = 0;*/

	init_lcd_spi();
	en_lcd();
	lcd_rst();

	post_cmd( 0x000, 0x0001 ); /* oschilliation start */
	bcm2835_delay( 10 );
	/* Power settings */  	
	post_cmd( 0x100, 0x0000 ); /*power supply setup*/	
	post_cmd( 0x101, 0x0000 ); 
	post_cmd( 0x102, 0x3110 ); 
	post_cmd( 0x103, 0xe200 ); 
	post_cmd( 0x110, 0x009d ); 
	post_cmd( 0x111, 0x0022 ); 
	post_cmd( 0x100, 0x0120 ); 
	bcm2835_delay( 20 );

	post_cmd( 0x100, 0x3120 );
	bcm2835_delay( 80 );
	/* Display control */   
	post_cmd( 0x001, 0x0100 );
	post_cmd( 0x002, 0x0000 );
	post_cmd( 0x003, 0x1230 );
	post_cmd( 0x006, 0x0000 );
	post_cmd( 0x007, 0x0101 );
	post_cmd( 0x008, 0x0808 );
	post_cmd( 0x009, 0x0000 );
	post_cmd( 0x00b, 0x0000 );
	post_cmd( 0x00c, 0x0000 );
	post_cmd( 0x00d, 0x0018 );
	/* LTPS control settings */   
	post_cmd( 0x012, 0x0000 );
	post_cmd( 0x013, 0x0000 );
	post_cmd( 0x018, 0x0000 );
	post_cmd( 0x019, 0x0000 );

	post_cmd( 0x203, 0x0000 );
	post_cmd( 0x204, 0x0000 );

	post_cmd( 0x210, 0x0000 );
	post_cmd( 0x211, 0x00ef );
	post_cmd( 0x212, 0x0000 );
	post_cmd( 0x213, 0x013f );
	post_cmd( 0x214, 0x0000 );
	post_cmd( 0x215, 0x0000 );
	post_cmd( 0x216, 0x0000 );
	post_cmd( 0x217, 0x0000 );

	// Gray scale settings
	post_cmd( 0x300, 0x5343);
	post_cmd( 0x301, 0x1021);
	post_cmd( 0x302, 0x0003);
	post_cmd( 0x303, 0x0011);
	post_cmd( 0x304, 0x050a);
	post_cmd( 0x305, 0x4342);
	post_cmd( 0x306, 0x1100);
	post_cmd( 0x307, 0x0003);
	post_cmd( 0x308, 0x1201);
	post_cmd( 0x309, 0x050a);

	/* RAM access settings */ 
	post_cmd( 0x400, 0x4027 );
	post_cmd( 0x401, 0x0000 );
	post_cmd( 0x402, 0x0000 );	/* First screen drive position (1) */   	
	post_cmd( 0x403, 0x013f );	/* First screen drive position (2) */   	
	post_cmd( 0x404, 0x0000 );

	post_cmd( 0x200, 0x0000 );
	post_cmd( 0x201, 0x0000 );

	post_cmd( 0x100, 0x7120 );
	post_cmd( 0x007, 0x0103 );
	bcm2835_delay( 10 );
	post_cmd( 0x007, 0x0113 );

	dis_lcd();
}

void image_display(int imgcase){
	u16  temp,num;
	u8 n,i;

	en_lcd();	

	post_cmd(0x210,0x00);
	post_cmd(0x212,0x0000);
	post_cmd(0x211,0xEF);
	post_cmd(0x213,0x013F);

	post_cmd(0x200,0x0000);
	post_cmd(0x201,0x0000);

	en_lcd_index();
	post_data(0x202);
	en_lcd_data();

	post_cmd(0x210,0x00);
	post_cmd(0x212,0x0000);
	post_cmd(0x211,0xEF);
	post_cmd(0x213,0x013F);

	post_cmd(0x200,0x0000);
	post_cmd(0x201,0x0000);

	en_lcd_index();
	post_data(0x202);
	en_lcd_data();

	printf("Image is going to change\n ");

	switch(imgcase){
		case 1:
			for(i=0;i<240;i++)
			{
				for(num=0;num<320;num++)
				{
					post_data(trueimg[num+i*320]);
				}
			}
			break;
		case 2:
			for(i=0;i<240;i++)
			{
				for(num=0;num<320;num++)
				{
					post_data(falseimg[num+i*320]);
				}
			}
			break;

		default:
			for(i=0;i<240;i++)
			{
				for(num=0;num<320;num++)
				{
					post_data(waitimg[num+i*320]);
				}
			}
			break;

			break;
	}


	dis_lcd();
	printf("Change is done!\n ");
}

void LCD_test(void)
{
	u16  temp,num;
	u8 n,i;

	en_lcd();	

	post_cmd(0x210,0x00);
	post_cmd(0x212,0x0000);
	post_cmd(0x211,0xEF);
	post_cmd(0x213,0x013F);

	post_cmd(0x200,0x0000);
	post_cmd(0x201,0x0000);

	en_lcd_index();
	post_data(0x202);
	en_lcd_data();
	for(n=0;n<8;n++)
	{
		temp=colorfol[n];
		for(num=40*240;num>0;num--)
		{
			post_data(temp);
		}
	}
	bcm2835_delay(500);
	for(n=0;n<1;n++)
	{
		post_cmd(0x210,0x00);
		post_cmd(0x212,0x0000);
		post_cmd(0x211,0xEF);
		post_cmd(0x213,0x013F);

		post_cmd(0x200,0x0000);
		post_cmd(0x201,0x0000);

		en_lcd_index();
		post_data(0x202);
		en_lcd_data();
		temp=colorfol[n];
		for(i=0;i<240;i++)
		{
			for(num=0;num<320;num++)
			{
				post_data(temp);
			}
		}
		//	bcm2835_delay(50);
	}
	dis_lcd();
}


void DisplayChar(u8 casc,u8 postion_x,u8 postion_y)
{
	u8 i,j,b;
	const u8 *p;


	en_lcd();
	post_cmd(0x210,postion_x*8); 	//x start point
	post_cmd(0x212,postion_y*16); 	//y start point
	post_cmd(0x211,postion_x*8+7);	//x end point
	post_cmd(0x213,postion_y*16+15);	//y end point
	post_cmd(0x200,postion_x*8);	
	post_cmd(0x201,postion_y*16);

	en_lcd_index();
	post_data(0x202);
	en_lcd_data();
	p=ascii;
	p+=casc*16;
	for(j=0;j<16;j++)
	{
		b=*(p+j);
		for(i=0;i<8;i++)
		{
			if(b&0x80)
			{
				post_data(COLOR_WHITE);
			}
			else
			{
				post_data(COLOR_BLACK);
			}
			b=b<<1;

		}	
	}
	dis_lcd();
}

void DisplayChar_Reverse(u8 casc,u8 postion_x,u8 postion_y)
{
	u8 i,j,b;
	const u8 *p;

	en_lcd();
	post_cmd(0x210,postion_x*8); 	//x start point
	post_cmd(0x212,postion_y*16); 	//y start point
	post_cmd(0x211,postion_x*8+7);	//x end point
	post_cmd(0x213,postion_y*16+15);	//y end point

	post_cmd(0x200,postion_x*8);	
	post_cmd(0x201,postion_y*16);

	en_lcd_index();
	post_data(0x202);
	en_lcd_data();
	p=ascii;
	p+=casc*16;
	for(j=16;j>0;j--)
	{
		b=*(p+j-1);
		for(i=0;i<8;i++)
		{
			if(b&0x01)
			{
				post_data(COLOR_BLACK);
			}
			else
			{
				post_data(COLOR_YELLOW);
			}
			b=b>>1;

		}	
	}
	dis_lcd();
}

u8* swap(u8 *s,u8 sz)
{
	u8 i=0;
	static u8 b[10]={0};
	s+=sz-2;
	for(i=0;i<sz-1;i++)
	{
		b[i]=*s;
		s--;
	}
	s=b;
	return s;
}

void DisplayString(u8 *s,u8 x,u8 y,u8 Reverse)
{
	u8 a[10],i;
	if(Reverse)
	{
		i=0;
		while(*s){a[i]=*s;s++;i++;}
		s=swap(a,sizeof(a));
	}
	while (*s) 
	{ 
		if(Reverse)
		{DisplayChar_Reverse(*s,x,y);}
		else
		{DisplayChar(*s,x,y);}
		if(++x>=30)
		{
			x=0;
			if(++y>=20)
			{
				y=0;
			}
		}
		s++;
	}
}

unsigned long mypow(unsigned char m,unsigned char n)
{
	unsigned long result=1;
	while(n--)result*=m;
	return result;
}

void lcd_display_number(unsigned int x,
		unsigned int y,
		unsigned long num,
		unsigned char num_len )
{
	unsigned char t,temp;
	unsigned char enshow=0;          // ´Ë±äÁ¿ÓÃÀ´È¥µô×î¸ßÎ»µÄ0

	for(t=0;t<num_len;t++)
	{
		temp=(num/mypow(10,num_len-t-1))%10;
		if(enshow==0&&t<(num_len-1))
		{
			if(temp==0)
			{
				DisplayChar(' ',x+t,y);
				continue;
			}else enshow=1;

		}
		DisplayChar(temp+'0',x+t,y);
	}
}

u16 get_touch_data(u8 cmd)
{
	/*	u8 tmp1,tmp2;
		u16  tmp;
		bcm2835_spi_transfer(cmd);

		tmp1=bcm2835_spi_transfer(0x00);
		printf("tmp1: %d ",tmp1);       
		tmp2=bcm2835_spi_transfer(0x00);	
		printf("tmp2: %d ",tmp2);
		tmp=((u16)(tmp1)<<5 | (u16)(tmp2)>>3 );
		printf("tmp: %d ",tmp);	
		delayMicroseconds(50);

		return (tmp);*/

	unsigned int NUMH , NUML;
	unsigned int Num;
	dis_lcd();//LCD_CS_H()
	en_touch();//TOUCH_nCS_L();

	delayMicroseconds(2);
	bcm2835_spi_transfer(cmd);
	delayMicroseconds(2);              // ÑÓÊ±µÈ´ý×ª»»Íê³É
	NUMH=bcm2835_spi_transfer(0x00);//printf("tmp1: %d ",NUMH);
	NUML=bcm2835_spi_transfer(0x00);//printf("tmp2: %d ",NUML);
	Num=((NUMH)<<8)+NUML;
	Num>>=4;                // Ö»ÓÐ¸ß12Î»ÓÐÐ§.
	dis_touch();//TOUCH_nCS_H();
	//printf("tmp2: %d ",Num);
	return(Num);

}

xy_t get_touch_xy(void)
{
	xy_t tmp_xy;
	if(1){//if(!bcm2835_gpio_lev(PENIRQ)){
		tmp_xy.x = get_touch_data(TOUCH_CMD_X);
		tmp_xy.y = get_touch_data(TOUCH_CMD_Y);
		printf("1 ");

	} else{
		tmp_xy.x = 0xFFFF;
		tmp_xy.y = 0xFFFF;
		//	    printf("0 ");
	}

	return tmp_xy;
	}

	u8 get_point_xy(void)
	{
		u8 n,m,tmp;
		xy_t tmp_xy_buf[SAMP_COUNT], tmp_xy;
		u32 tmp_x = ((u32)tmp_xy_buf[SAMP_COUNT/2].x + (u32)tmp_xy_buf[SAMP_COUNT/2-1].x)/2;
		u32 tmp_y = ((u32)tmp_xy_buf[SAMP_COUNT/2].y + (u32)tmp_xy_buf[SAMP_COUNT/2-1].y)/2;
		/*	
			if(touch_counter>=(TOUCH_MAX_CACHE-1)){
			return 0;
			}
		 *///mask by jack ben
		init_touch_spi();
		en_touch();
		for(n=0; n<SAMP_COUNT; n++){
			tmp_xy_buf[n] = get_touch_xy();
		}
		dis_touch();
		for(n=0; n<(SAMP_COUNT-1); n++){
			for(m=0; m<(SAMP_COUNT-n-1); m++){
				tmp = m+1;
				if((tmp_xy_buf[m].x + tmp_xy_buf[m].y) > (tmp_xy_buf[tmp].x + tmp_xy_buf[tmp].y)){
					tmp_xy = tmp_xy_buf[tmp];
					tmp_xy_buf[tmp] = tmp_xy_buf[m];
					tmp_xy_buf[m] = tmp_xy;
				}
			}
		}
		if((tmp_xy_buf[SAMP_COUNT/2].x - tmp_xy_buf[SAMP_COUNT/2-1].x > SAMP_THRESHOLD) 
				|| (tmp_xy_buf[SAMP_COUNT/2].y - tmp_xy_buf[SAMP_COUNT/2-1].y > SAMP_THRESHOLD)){
			return 0;
		}

		if(tmp_x >= 0xFFF || tmp_y >= 0xFFF){
			return 0;
		}
		touch_xy_buffer[touch_wr_index].x = ((tmp_x * 240)>>12);
		touch_xy_buffer[touch_wr_index].y = ((tmp_y * 320)>>12);
		if(touch_wr_index < (TOUCH_MAX_CACHE-1)){
			touch_wr_index++;
		}else{
			touch_wr_index = 0;
		}
		touch_counter++;
		return 1;
	}

	u8 draw_lcd(void)
	{
		u8 n;
		init_lcd_spi();
		en_lcd();
		if(touch_counter==0){
			return 0;
		}

		touch_counter--;

		post_cmd(0x210,touch_xy_buffer[touch_rd_index].x);
		post_cmd(0x212,touch_xy_buffer[touch_rd_index].y);
		post_cmd(0x211,touch_xy_buffer[touch_rd_index].x+(DOT_WIDTH-1));
		post_cmd(0x213,touch_xy_buffer[touch_rd_index].y+(DOT_WIDTH-1));
		if(touch_rd_index < (TOUCH_MAX_CACHE-1)){
			touch_rd_index++;
		}else{
			touch_rd_index = 0;
		}
		//post_cmd(0x0005,0x0010);

		en_lcd_index();
		post_data(0x202);
		en_lcd_data();
		for(n=0; n< (DOT_WIDTH*DOT_WIDTH); n++)
		{
			post_data(COLOR_BLACK);
		}
	dis_lcd();
	return 1;
}


//From start_lcd.c
unsigned int GetTouchADC (unsigned char data)
{
        u8 tmp1,tmp2;
        u16  tmp;
        dis_lcd();
        en_touch();
	delayMicroseconds(2);
        bcm2835_spi_transfer(data);
	delayMicroseconds(2);
        tmp1=bcm2835_spi_transfer(0x00);
        tmp2=bcm2835_spi_transfer(0x00);
        tmp=((u16)(tmp1)<<5 | (u16)(tmp2)>>3 );
	dis_touch();
        return (tmp);
/*
        unsigned int NUMH , NUML;
        unsigned int Num;
        dis_lcd();//LCD_CS_H()
        en_touch();//TOUCH_nCS_L();

        delayMicroseconds(2);
        bcm2835_spi_transfer(data);
        delayMicroseconds(2);              // ÑÓÊ±µÈ´ý×ª»»Íê³É
        NUMH=bcm2835_spi_transfer(0x00);
        NUML=bcm2835_spi_transfer(0x00);
        Num=((NUMH)<<8)+NUML;
        Num>>=4;                // Ö»ÓÐ¸ß12Î»ÓÐÐ§.
        dis_touch();//TOUCH_nCS_H();

        return(Num);
*/
}

unsigned int GetTouchADCEx(unsigned char cmd_code)
{
        unsigned int i, j;
        unsigned int buf[READ_TIMES];
        unsigned int sum=0;
        unsigned int temp;

        for(i=0;i<READ_TIMES;i++)
        {
                buf[i]=GetTouchADC(cmd_code);
        }
        for(i=0;i<READ_TIMES-1; i++)//ÅÅÐò
        {
                for(j=i+1;j<READ_TIMES;j++)
                {
                        if(buf[i]>buf[j])//ÉýÐòÅÅÁÐ
                        {
                                temp=buf[i];
                                buf[i]=buf[j];
                                buf[j]=temp;
                        }
                }
        }
        sum=0;
        for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
        temp=sum/(READ_TIMES-2*LOST_VAL);
        return temp;
}

unsigned char Read_ADS(unsigned int *x_ad,unsigned int *y_ad)
{
        unsigned int xtemp,ytemp;
        xtemp=GetTouchADCEx(TOUCH_CMD_X);    //ÓÐÉ¸Ñ¡µÄ¶ÁÈ¡XÖáAD×ª»»½á¹û
        ytemp=GetTouchADCEx(TOUCH_CMD_Y);            //ÓÐÉ¸Ñ¡µÄ¶ÁÈ¡YÖáAD×ª»»½á¹û           
        if(xtemp<100||ytemp<100)
        return 1;   //¶ÁÊýÊ§°Ü
        *x_ad=xtemp;
        *y_ad=ytemp;
        return 0;//¶ÁÊý³É¹¦
}

#define ERR_RANGE 50 //Îó²î·¶Î§

unsigned char Read_ADS2(unsigned long *x_ad,unsigned long *y_ad)
{
        unsigned int x1,y1;
        unsigned int x2,y2;
        unsigned char res;

    res=Read_ADS(&x1,&y1);  // µÚÒ»´Î¶ÁÈ¡ADCÖµ
    if(res==1)  return(1);      // Èç¹û¶ÁÊýÊ§°Ü£¬·µ»Ø1
    res=Read_ADS(&x2,&y2);      // µÚ¶þ´Î¶ÁÈ¡ADCÖµ
    if(res==1)  return(1);      // Èç¹û¶ÁÊýÊ§°Ü£¬·µ»Ø1
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//Ç°ºóÁ½´Î²ÉÑùÔÚ+-50ÄÚ
        &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x_ad=(x1+x2)/2;
        *y_ad=(y1+y2)/2;
        return 0;        // ÕýÈ·¶ÁÈ¡£¬·µ»Ø0
    }
    else return 1;       // Ç°ºó²»ÔÚ+-50ÄÚ£¬¶ÁÊý´íÎó
}

void convert_ad_to_xy(void)
{
      touch_dot.x=((touch_dot.x_ad_val-121)/13.030); // °Ñ¶Áµ½µÄX_ADCÖµ×ª»»³ÉTFT X×ø±êÖµ
      touch_dot.y=((touch_dot.y_ad_val-101)/10.500); // °Ñ¶Áµ½µÄY_ADCÖµ×ª»»³ÉTFT Y×ø±êÖµ
}

unsigned char Read_Once(void)
{
//      touch_dot.pen_status=Pen_Up;
        if(Read_ADS2(&touch_dot.x_ad_val,&touch_dot.y_ad_val)==0)       // Èç¹û¶ÁÈ¡Êý¾Ý³É¹¦
        {
                while(((bcm2835_gpio_lev))==0);   // ¼ì²â±ÊÊÇ²»ÊÇ»¹ÔÚÆÁÉÏ:IRQÎªµÍµçÆ½(bit7Îª0)ËµÃ÷±ÊÔÚÆÁÉÏ
                convert_ad_to_xy();   // °Ñ¶Áµ½µÄADCÖµ×ª±ä³ÉTFT×ø±êÖµ
                return 0;       // ·µ»Ø0£¬±íÊ¾³É¹¦
        }
        else return 1;  // Èç¹û¶ÁÈ¡Êý¾ÝÊ§°Ü£¬·µ»Ø1±íÊ¾Ê§°Ü
}

unsigned char Read_Continue(void)
{
//      touch_dot.pen_status=Pen_Up;
        if(Read_ADS2( &touch_dot.x_ad_val,&touch_dot.y_ad_val )==0)      // Èç¹û¶ÁÈ¡Êý¾Ý³É¹¦
        {
                convert_ad_to_xy();   // °Ñ¶Áµ½µÄADCÖµ×ª±ä³ÉTFT×ø±êÖµ
                return 0;          // ·µ»Ø0£¬±íÊ¾³É¹¦
        }
        else return 1;     // Èç¹û¶ÁÈ¡Êý¾ÝÊ§°Ü£¬·µ»Ø1±íÊ¾Ê§°Ü
}
void LCD_WRITE_COMMAND(unsigned int index,unsigned int data)
{
        //select command register
        en_lcd_index();//LCD_RS_L();
        en_lcd();//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(index>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(index));
        dis_lcd();//LCD_CS_H();
        //send data
        en_lcd_data();//LCD_RS_H();

        en_lcd()//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(data>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(data));
        dis_lcd();//LCD_CS_H();
}
void LCD_WRITE_REG(unsigned int index)
{
        unsigned int value_index;
        en_lcd_index();//LCD_RS_L();
        en_lcd();//LCD_CS_L();
        value_index=index;
        value_index =value_index>>8;
    bcm2835_spi_transfer((unsigned char)(value_index));    //00000000 000000000

        value_index=index;
        value_index &=0x00ff;
    bcm2835_spi_transfer((unsigned char)(index));
        dis_lcd();//LCD_CS_H();
        en_lcd_data();//LCD_RS_H();
}
void LCD_WRITE_DATA(unsigned int data)
{
    bcm2835_spi_transfer((unsigned char)(data>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(data));
}
void LCD_SEND_COMMAND(unsigned int index,unsigned int data)
{
        //select command register
        en_lcd_index();//LCD_RS_L();
        en_lcd();//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(index>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(index));
        dis_lcd();//LCD_CS_H();
        //send data
        en_lcd_data();//LCD_RS_H();
        en_lcd();//LCD_CS_L();
    bcm2835_spi_transfer((unsigned char)(data>>8));    //00000000 000000000
    bcm2835_spi_transfer((unsigned char)(data));
        dis_lcd();//LCD_CS_H();
}

void lcd_draw_dot(unsigned int color_front,
                  unsigned char x,
                  unsigned int y)
{

        LCD_SEND_COMMAND(0x210,x);
        LCD_SEND_COMMAND(0x212,y);
        LCD_SEND_COMMAND(0x211,x+1);
        LCD_SEND_COMMAND(0x213,y+1);


        en_lcd_index();//LCD_RS_L();
        LCD_WRITE_REG(0x202);   //RAM Write index
        en_lcd();//LCD_CS_L();
        LCD_WRITE_DATA(color_front);


}
void lcd_draw_bigdot(unsigned int   color_front,
                     unsigned int    x,
                     unsigned int    y )
{
    lcd_draw_dot(color_front,x,y);
    lcd_draw_dot(color_front,x,y+1);
    lcd_draw_dot(color_front,x,y-1);

    lcd_draw_dot(color_front,x+1,y);
    lcd_draw_dot(color_front,x+1,y+1);
    lcd_draw_dot(color_front,x+1,y-1);

    lcd_draw_dot(color_front,x-1,y);
    lcd_draw_dot(color_front,x-1,y+1);
    lcd_draw_dot(color_front,x-1,y-1);

}

/*
void display_touch_debug()
{        //ADC results show
        DisplayString("READ SUCCESS:",0,0xffff,1,3);
        lcd_display_number(14,3,read_res,6);

        DisplayString("X AD Val:",0,0xffff,1,4);
        lcd_display_number(10,4,touch_dot.x_ad_val,6);

        DisplayString("Y AD Val:",0,0xffff,1,5);
        lcd_display_number(10,5,touch_dot.y_ad_val,6);

        //Display coordinates
        DisplayString("X:",0,0xffff,1,6);
        lcd_display_number(10,6,touch_dot.x,5);
        DisplayString("Y:",0,0xffff,1,7);
        lcd_display_number(10,7,touch_dot.y,5);

}*/
