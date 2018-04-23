/**************************************************************************************************
* interrupt.c
*
*
**************************************************************************************************/
#include "com_config.h"
#include "timer.h"
#include "step_motor.h"
//#include "main.h"
#include "adc.h"
#include "system.h"
#include "uart.h"
#include "massage.h"
#include "host_com.h"

/**************************************************************************************************/
// 定时器4处理函数，用于步进电机运动
void  timer_6_intr_func(void)
{
	// 步进电机中断处理函数
	step_motors_intr_func();
}
// 定时器1处理函数，用于时间
void timer_1_intr_func(void)
{
	set_timer_1_period();
	set_os_10ms_main_count();
	adc_go_on();
}
void timer_0_intr_func(void)
{  
   static uint8 pluse_state = 0;
   static uint8 pluse_cnt = 0;
   
}
void interrupt ISR (void)
{
	/*******************************/
	if((IOCIE ==1)&&(IOCBF0 == 1))
	{
	    //massage_ctrl();              //过零中断控制SCR
        IOCBF0 = 0;		
	}
	if(TMR0IF & TMR0IE)
	{
	     TMR0IF = 0;
	     TMR0 = 180;  
	}	
	/* 串口1接收数据 */	
	if(PIR1bits.RCIF ) 
	{
		static uint8 rec_h = 0;
		if((RC1STAbits.FERR == 1) || ( RC1STAbits.OERR == 1)) 
		{
			static uint8 error_rc = 0;
			error_rc = RCREG;
			NOP();
			error_rc = RCREG;			
			RC1STAbits.CREN = 0;
			NOP();
			RC1STAbits.CREN = 1;
		}	
		rec_h = RCREG;
		PIR1bits.RCIF = 0;
		if(PIE1bits.RCIE)
		{
		   rec_from_tempt_box_int();       //串口1接收来自主控板的数据
		}
	}
	if(TXIE & TXIF)
	{ 
	    TXIF = 0;
		usart1_send_interrupt();
	}
	/* 串口2接收数据 */	
	if(PIR4bits.RC2IF )
	{
		static uint8 rec_h = 0;
		if ((RC2STAbits.FERR == 1) || (RC2STAbits.OERR == 1))
		{
			static uint8 error_rc = 0;
			error_rc = RC2REG;
			NOP();
			error_rc = RC2REG;		
			
			RC2STAbits.CREN = 0;
			NOP();
			RC2STAbits.CREN = 1;
		}	
		rec_h = RC2REG;
		PIR4bits.RC2IF = 0;
		if(PIE4bits.RC2IE)
		{				
			rec_debug_int();         //串口2接收来自WIFI模块的数据
		}
	}
	if(TX2IE & TX2IF)
	{ 
	    TX2IF = 0;
		usart2_send_interrupt();
	}
	if(ADIE & ADIF)
	{   ADIF=0;
		read_adc();		
	}
	// 步进电机
	if (TMR6IF) 
	{
	    TMR6IF = 0;
	    if(TMR6IE)
		{
		    timer_6_intr_func();	
	    }
	}
	/*******************************/
	// 系统时钟/滴答
	if (TMR1IF == 1)
	{
		TMR1IF = 0;	
		if (TMR1IE == 1) 
		{
			timer_1_intr_func();		
		}
	}	
}
