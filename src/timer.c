/***************************************************************************************
* timer.c
*
*
*****************************************************************************************/
#include "com_config.h"
/**************************************************************************/
/* ��ʱ��0 */
#define _H_ 0xEC
#define _L_ 0x78

volatile static uint8 timer_0_v = _H_;


void open_timer_0(uint16 ms, uint8 intr_en, uint8 intr_pr,uint8 tmr_on)
{
	uint16 timer_period = 0;
	
	INTCONbits.TMR0IE  = 0;    
	INTCONbits.TMR0IF  = 0;
	
	// ��ʱ�����ڼ���������
	OPTION_REGbits.TMR0CS 	 = 0;	//	ѡ��ʱ��ԴFosc/4
	OPTION_REGbits.PSA  	 = 0;	//	TMR0ʱ�������ź���Դ�ڷ�Ƶ��
	
	// FOSC = 16M ; FOSC/4 = 4MHz
	OPTION_REGbits.PS = 0b010;	// 256Ԥ��Ƶ // 64us 
		
    TMR0 = timer_0_v;
    
	// �ж�ʹ��
	if(intr_en)
		INTCONbits.TMR0IE  = 1; 
}

void set_timer_0_period(void)
{      
    TMR0 = timer_0_v;
}

void close_timer_0(void)
{
//    T0CONbits.TMR0ON = 0;    
	INTCONbits.TMR0IE  = 0;    
//	INTCON2bits.TMR0IP = 0;	
	INTCONbits.TMR0IF  = 0;	
}

/**************************************************************************/
static uint8 timer_0_h = _H_;
static uint8 timer_0_l = _H_;
/* ��ʱ��1 */
void open_timer_1(uint16 ms, uint8 intr_en, uint8 intr_pr, uint8 tmr_on)
{
	uint16 ms_t = ms; 
	TMR1ON = 0;
	TMR1IE  = 0;    
	TMR1IF  = 0;
	TMR1GE = 0;
	
/*	T2CONbits.T2CKPS = 0b10; // 16Ԥ��Ƶ�� fosc/4 /16 = 4Mhz /16 = 4us
	PR2 = (_4us >> 2) -1;
*/
	T1CONbits.TMR1CS = 0b00; // fosc/4 = 4MHZ
	T1CONbits.T1CKPS = 0b11; // 1:8 Prescale value  = 0.5MHZ = 2us
	T1OSCEN = 0;
	
	// 500 * 2us = 1ms
	ms_t = ms_t * 500;
	
	ms_t = 0x10000 - ms_t;
	
	timer_0_h = (uint8)((ms_t >> 8) & 0xff);
	timer_0_l = (uint8)((ms_t >> 0) & 0xff);
	
	TMR1H = timer_0_h;
	TMR1L = timer_0_l;
	
	if(intr_en)
		TMR1IE = 1;
	if(tmr_on) // ʹ�ܶ�ʱ��
		TMR1ON = 1; 
	return;
}

void set_timer_1_period(void)
{      
 	TMR1H = timer_0_h;
	TMR1L = timer_0_l;
}


void close_timer_1(void)
{	
	TMR1ON = 0;
	TMR1IE  = 0;    
	TMR1IF  = 0;
	return ;
}
/**************************************************************************/
/* ��ʱ��2 */
void open_timer_2(uint16 _4us, uint8 intr_en, uint8 intr_pr, uint8 tmr_on)
{

	return;
}

void close_timer_2(void)
{

	return ;
}
/**************************************************************************/
/**************************************************************************/
/* ��ʱ��4 */
//void open_timer_4(uint16 _64us, uint8 intr_en, uint8 intr_pr,uint8 tmr_on)
void open_timer_4(uint16 _16us, uint8 intr_en, uint8 intr_pr,uint8 tmr_on)
{
	return ;
}

void close_timer_4(void)
{
	return ;
}
/**************************************************************************/
/**************************************************************************/
/* ��ʱ��6 */
void open_timer_6(uint16 _1us, uint8 intr_en, uint8 intr_pr,uint8 tmr_on)
{
	
	TMR6ON = 0;
	TMR6IE = 0;
	TMR6IF = 0;
	
	// FOSC/4 = 16/4 = 4MHZ = 0.25us
	T6CONbits.T6CKPS0 = 0b1;   // 64 Ԥ��Ƶ
	T6CONbits.T6CKPS1 = 0b1;   // 64Ԥ��Ƶ
	
	T6CONbits.T6OUTPS0 = 0b1; // 16 ���Ƶ
	T6CONbits.T6OUTPS1 = 0b1; // 16 ���Ƶ
	T6CONbits.T6OUTPS2 = 0b1; // 16���Ƶ
	T6CONbits.T6OUTPS3 = 0b1; // 16���Ƶ
	// 4mhz/4/1 = 1us
	
	if(_1us >= 255)
		PR6 = 255;
	else
		PR6 = _1us;
		
	if(intr_en)
		TMR6IE = 1;
	
	if(tmr_on) // ʹ�ܶ�ʱ��
		TMR6ON = 1; 
		
}



void close_timer_6(void)
{
	TMR6ON = 0;
	TMR6IE = 0;
	TMR6IF = 0;
	return ;
}
/**************************************************************************/
/**************************************************************************/
