#include "stdio.h"
#include "stdlib.h"
#include "reg52.h"

typedef unsigned int u16;
typedef unsigned char u8;

#ifndef _key_H
#define _key_H

// Pressable Keys
sbit KEY1=P3^1;
sbit KEY2=P3^0;
sbit KEY3=P3^2;
sbit KEY4=P3^3;

#endif

// Piano Notes value
#define L1  1
#define L1_ 2
#define L2  3
#define L2_ 4
#define L3  5
#define L4  6
#define L4_ 7
#define L5  8
#define L5_ 9
#define L6  10
#define L6_ 11
#define L7  12
#define M1  13
#define M1_ 14
#define M2  15
#define M2_ 16
#define M3  17
#define M4  18
#define M4_ 19
#define M5  20
#define M5_ 21
#define M6  22
#define M6_ 23
#define M7  24
#define H1  25
#define H1_ 26
#define H2  27
#define H2_ 28
#define H3  29
#define H4  30
#define H4_ 31
#define H5  32
#define H5_ 33
#define H6  34
#define H6_ 35
#define H7  36

// pins
sbit SRCLK=P3^6;	// register
sbit TRCLK=P3^5;	// clock input
sbit SER=P3^4; 		// serial input

// LED display setting
#define LEDDZ_COL_PORT	P0	// control pin
u8 gled_row[8]={0x38,0x7C,0x7E,0x3F,0x3F,0x7E,0x7C,0x38}; // heart image - row
u8 gled_col[8]={0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe}; // heart image - col

// buzzer setting
#define ClockSpeed 12000000 // clock freq (Hz)
#define SongSpeed 150       // each note span (ms)
sbit beepIO = P2^5;         // buzzer IO

unsigned char freq_select;
unsigned char PLAY_FLAG;

// Notes frequency table
const unsigned int code freq_table[] = {0,61714 ,61928 ,62131 ,62322 ,62502 ,62673 ,62833 ,62985 ,63128 ,63263 ,63391 ,63511 //µÕ“Ù
	                         ,63628 ,63731 ,63835 ,63928 ,64021 ,64103 ,64185 ,64260 ,64331 ,64400 ,64463 ,64524 //÷–“Ù
                                 ,64580 ,64633 ,64684 ,64732 ,64777 ,64820 ,64860 ,64898 ,64934 ,64968 ,65000 ,65030 //∏ﬂ“Ù
	                        };															 
// Wedding song
// music sheet
unsigned char code melody[] = {0, M5, H1, H1, H1, 0, M5, H2, M7, H1, 0,
						M5, H1, H4, H4,	H3, H2, H1, H2, H1, M7, H1, H2, 0,
						M5, H1, H1, H1, 0, M5, H2, M7, H1, 0,
						M5, H1, H3, H5,	H3, H1, M6, H2, H3, H1, 0,
						40};
// span of note
unsigned char code span[]={4, 4,2,1,4,2,4,2,1,4,2,
						4,2,1,4,2,1,2,1,1,2,1,4,2,
						4,2,1,4,2,4,2,1,4,2,
						4,2,1,4,2,1,4,2,1,4,4,1,1,1,1,1};


void timer0_initial()
{
	IT1 	= 1; // descending edge
	EX1 	= 1; // INT1 interrupt allowance

	beepIO 	= 0;
	TH0   	= 0xFD;	
	TL0   	= 0x09;
	TMOD  	= 0x01;  // Timer 0 working flow 1

	ET0   	= 1;     // enable timer 0
	TF0   	= 0;     // overflow flag
	TR0   	= 1;     // timer 0 start

	EA		= 1; // main interrupt
}

void delay_ms(unsigned int x)
{
	unsigned char t;
	while(x--){
		for(t = 0; t < 120; t++);
	}
}

void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}

void buzzerInterrupt() interrupt 1	// buzzer interrupt
{
	beepIO = !beepIO;   // buzzer IO switch
	TH0 = freq_table[freq_select]/256 ;
	TL0 = freq_table[freq_select]%256 ;
}

void keyInterrupt() interrupt 2 // key interrupt
{
	delay_10us(100); // jitter
	PLAY_FLAG = !PLAY_FLAG; // reverse
	if (!PLAY_FLAG) {
	 	beepIO = 0;
	}					
}

void exti1_init(void)
{
	IT1 		= 1; // descending edge
	EX1			= 1; // INT1
	EA			= 1; // main interrupt
	PLAY_FLAG 	= 0; // flag
}

void hc595_write_data(u8 dat)
{
	u8 i = 0;
	
	for(i = 0; i < 8; i++)
	{
		SER = dat >> 7; //higher bit first
		dat <<= 1; // low -> high
		SRCLK = 0;
		delay_10us(1);
		SRCLK = 1;
		delay_10us(1); // clock ascending edge	
	}
	TRCLK = 1;
	delay_10us(1);
	TRCLK = 0; // clock ascending edge		
}

void main()
{
  unsigned int select;
  unsigned char key = 0;
  unsigned char time_flag = 0;
  u8 i = 0;
  timer0_initial();
  exti1_init();
	while (1)
	{
		while(PLAY_FLAG && melody[select] != 40) // if music ends
		{
				freq_select = melody[select];		
				if(freq_select) // pause
				{
					delay_ms(span[select] * SongSpeed);
					TR0 = 0;
					delay_ms(10);
					TR0 = 1;
					for(i = 0; i < 8; i++)
					{
						LEDDZ_COL_PORT = gled_col[i]; // col
						hc595_write_data(gled_row[i]); // row
						delay_10us(200);
						hc595_write_data(0x00);	
					}
				}else{			
					TR0 = 0;
					delay_ms(span[select] * SongSpeed);
					TR0 = 1;
				}
				select++;
	
		}
		TR0 = 0;
		beepIO = 0;
		PLAY_FLAG = 0;
		select = 0;		
	}
	
}
