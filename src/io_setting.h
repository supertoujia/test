#ifndef __IO_SETTING_H__
#define __IO_SETTING_H__
#include "com_config.h"
/*********************************************************************/
// A3984 ICøÿ÷∆io
#define A3984_RST_FLOW_OUTPUT 	{ANSA0 = 0; TRISA0 = 0; LATA0 = 0;} //RA0/AN0
#define A3984_DIR_FLOW_OUTPUT 	{ANSA2 = 0; TRISA2 = 0; LATA2 = 0;} //RA2/AN2
#define A3984_STEP_FLOW_OUTPUT 	{ANSA1 = 0; TRISA1 = 0; LATA1 = 0;} //RA1/AN1
#define A3984_EN_FLOW_OUTPUT 	{TRISC1 = 0; LATC1 = 0;}            //RC4
#define A3984_MS1_FLOW_OUTPUT 	{TRISA4 = 0; LATA4 = 0;}            //RA4
#define A3984_MS2_FLOW_OUTPUT 	{ANSA5 = 0; TRISA5 = 0; LATA5 = 0;} //RA5/AN4


#define A3984_RST_FLOW 	LATA0
#define A3984_DIR_FLOW 	LATA2
#define A3984_STEP_FLOW LATA1
#define A3984_EN_FLOW 	LATC1
#define A3984_MS1_FLOW 	LATA4
#define A3984_MS2_FLOW  LATA5


#define A3984_RST_TEMP_OUTPUT 	{ANSG4 = 0;  TRISG4 = 0; LATG4 = 0;}  //RG4
#define A3984_DIR_TEMP_OUTPUT 	{ANSF1 = 0;  TRISF1 = 0; LATF1 = 0;}  //RF1
#define A3984_STEP_TEMP_OUTPUT 	{ANSF5 = 0;  TRISF5 = 0; LATF5 = 0;}  //RF5
#define A3984_EN_TEMP_OUTPUT 	{ANSF2 = 0;  TRISF2 = 0; LATF2 = 0;}  //RF2
#define A3984_MS1_TEMP_OUTPUT 	{ANSF3 = 0;  TRISF3 = 0; LATF3 = 0;}  //RF3
#define A3984_MS2_TEMP_OUTPUT 	{ANSF3 = 0;  TRISF4 = 0; LATF4 = 0;}  //RF4


#define A3984_RST_TEMP 	LATG4
#define A3984_DIR_TEMP 	LATF1
#define A3984_STEP_TEMP LATF5
#define A3984_EN_TEMP 	LATF2
#define A3984_MS1_TEMP 	LATF3
#define A3984_MS2_TEMP  LATF4


#define A3984_FLOW_IOS_OUTPUT {A3984_RST_FLOW_OUTPUT; A3984_DIR_FLOW_OUTPUT; A3984_STEP_FLOW_OUTPUT;A3984_EN_FLOW_OUTPUT; A3984_MS1_FLOW_OUTPUT; A3984_MS2_FLOW_OUTPUT;}

#define A3984_TEMP_IOS_OUTPUT {A3984_RST_TEMP_OUTPUT; A3984_DIR_TEMP_OUTPUT; A3984_STEP_TEMP_OUTPUT;A3984_EN_TEMP_OUTPUT; A3984_MS1_TEMP_OUTPUT; A3984_MS2_TEMP_OUTPUT;}
                                

#define SET_A3984_RST_TEMP(x)  	{ A3984_RST_TEMP = x;}
#define SET_A3984_DIR_TEMP(x)  	{ A3984_DIR_TEMP = x;}
#define SET_A3984_STEP_TEMP(x) 	{ A3984_STEP_TEMP = x;}
#define SET_A3984_EN_TEMP(x)  	{ A3984_EN_TEMP = x;}
#define SET_A3984_MS1_TEMP(x)  	{ A3984_MS1_TEMP = x;}
#define SET_A3984_MS2_TEMP(x) 	{ A3984_MS2_TEMP = x;}


#define SET_A3984_RST_FLOW(x) 	{ A3984_RST_FLOW = x;}
#define SET_A3984_DIR_FLOW(x) 	{ A3984_DIR_FLOW = x;}
#define SET_A3984_STEP_FLOW(x) 	{ A3984_STEP_FLOW = x;}
#define SET_A3984_EN_FLOW(x) 	{ A3984_EN_FLOW = x;}
#define SET_A3984_MS1_FLOW(x) 	{ A3984_MS1_FLOW = x;}
#define SET_A3984_MS2_FLOW(x) 	{ A3984_MS2_FLOW = x;}

/*********************************************************************/
/*********************************************************************/
/* ADC */
#define AN_0  0
#define AN_1  1
#define AN_2  2
#define AN_3  3
#define AN_4  4
#define AN_5  5
#define AN_6  6
#define AN_7  7
#define AN_8  8
#define AN_9  9
#define AN_10 10
#define AN_11 11
#define AN_12 12
#define AN_13 13
#define AN_14 14
#define AN_15 15
#define AN_16 16
#define AN_17 17
#define AN_18 18

//#define TEMP_IN	 AN_8
//#define TEMP_MID   AN_7
//#define VREF_ADC   AN_3


#define ON         1
#define OFF        0
#define OPEN       1
#define CLOSE      0

 
/**********************–˝≈•œ‡πÿ∂®“Â*******************************/
#define INT1_IO_INPUT		{ TRISBbits.TRISB0 = 1; }
#define INT1_UP_INT	        { IOBP0 = 1;IOBN0 = 0;}     //…œ…˝—ÿ÷–∂œ
#define INT1_DOWN_INT       { IOBP0 = 0;IOBN0 = 1;}     //œ¬Ωµ—ÿ÷–∂œ
#define INT1_INTR_DISALBE   { IOBP0 = 0;IOBN0 = 0;}     //Ω˚÷π±ﬂ—ÿºÏ≤‚
#define INT1_IO              RB0

#define INT2_IO_INPUT		{ TRISBbits.TRISB1 = 1; }
#define INT2_UP_INT	        { IOBP1 = 1;IOBN1 = 0;}     //…œ…˝—ÿ÷–∂œ
#define INT2_DOWN_INT       { IOBP1 = 0;IOBN1 = 1;}     //œ¬Ωµ—ÿ÷–∂œ
#define INT2_INTR_DISALBE   { IOBP1 = 0;IOBN1 = 0;}     //Ω˚÷π±ﬂ—ÿºÏ≤‚
#define INT2_IO              RB1


#define INT3_IO_INPUT		{ TRISBbits.TRISB2 = 1; }
#define INT3_UP_INT	        { IOBP2 = 1;IOBN2 = 0;}     //…œ…˝—ÿ÷–∂œ
#define INT3_DOWN_INT       { IOBP2 = 0;IOBN2 = 1;}     //œ¬Ωµ—ÿ÷–∂œ
#define INT3_INTR_DISALBE   { IOBP2 = 0;IOBN2 = 0;}     //Ω˚÷π±ﬂ—ÿºÏ≤‚
#define INT3_IO              RB2

#define KEY_A_IO_INPUT 		{INT1_IO_INPUT}
#define KEY_A_UP_INT 		{INT1_UP_INT}
#define KEY_A_DOWN_INT 		{INT1_DOWN_INT}
#define KEY_A_INTR_DISALBE 	{INT1_INTR_DISALBE}
#define KEY_A 				 INT1_IO

#define KEY_B_IO_INPUT 		{INT2_IO_INPUT}
#define KEY_B_UP_INT 		{INT2_UP_INT}
#define KEY_B_DOWN_INT 		{INT2_DOWN_INT}
#define KEY_B_INTR_DISALBE 	{INT2_INTR_DISALBE}
#define KEY_B 				 INT2_IO				

#define KEY_C_IO_INPUT 		{INT3_IO_INPUT}
#define KEY_C_UP_INT 		{INT3_UP_INT}
#define KEY_C_DOWN_INT 		{INT3_DOWN_INT}
#define KEY_C_INTR_DISALBE 	{INT3_INTR_DISALBE}
#define KEY_C 				 INT3_IO


#define GPIO_B				PORTB


// INTCON2 RBPU
#define KEY_IOS_INIT {RBPU = 0; CTMUEN = 0; KEY_A_IO_INPUT;KEY_B_IO_INPUT;KEY_C_IO_INPUT;}







#endif
/*********************************************************************/
