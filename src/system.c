/*************************************************************************************
* system.c
*
*
***************************************************************************************/
#include "stdio.h"
#include "com_config.h"
#include "temperature.h"
#include "host_com.h"
#include "keep_warm.h"
#include "water_level_sensor.h"
#include "com_config.h"
#include "timer.h"
#include "step_motor.h"
#include "adc.h"
#include "system.h"
#include "uart.h"
#include "massage.h"
#include "host_com.h"
#include "traic.h"
#include "host_com.h"
#include "drain_water.h"
#include "lamp.h"
#include "fan_heat.h"


#define  SYSCLK_FREQ_16MHz 1
#define  MAX_TASKS    30                                        /* 最大任务数*/
#define  MS  *8  


/**********************************相关函数声明****************************************/
void mcu_init(void);                    //MCU初始化
void interrupt_enable(void);            //使能中断
void interrupt_disable(void);           //禁止中断

void feed_wdt(void);                    //喂狗
void wdt_disable(void);                 //禁止看门狗
void wdt_enable(void);                  //使能看门狗
void debug(void);                        //系统调试



/**********************************相关变量声明****************************************/

//定义程序任务基本结构
typedef struct 
{
    void (*func_process)(void);          //程序任务指针
    uint16 period;                       //任务对应的时间片周期，支持最大65535*系统时间基准
    uint16 time ;                       //每次执行任务对应的时间
} _task_;

static _task_ tasks[] ={
    {host_com_in_main,100,0},           //串口接受处理
    {massage_ctrl_in_mian,100,0},       //按摩处理控制，100ms执行一次
    {keep_warm_in_mian,100,0},          //循环保温，100ms执行一次
    {water_level_judge,100,0},          //液位判断，100ms执行一次
    {adc_go_on,10,0},                   //AD采样，10ms采样一次 
 //   {lamp_1ms_process,1,0},
    {lamp_process,10,0},
    { debug,100,0},
 //   {fan_heat_process,10,0},

#if MASSAGE_CTRL_MODE  >0               //有进行功率控制     
   // {massage_power_process,10,0},

#endif   
};
     
static _task_ *sys_task;        //定义一个程序任务指针
static uint16 system_tick = 0;  //系统滴答时间



 


void interrupt_enable(void)
{	
	INTCONbits.GIE 	= 1;	
}
void interrupt_disable(void)
{
	INTCONbits.GIE 	= 0;
}

void wdt_enable(void)
{
	WDTCONbits.SWDTEN = 1; // 配置位设置: WDTPS
	CLRWDT();
}	
void wdt_disable(void)
{
	WDTCONbits.SWDTEN = 0; // WDTPS 
}	
void feed_wdt(void)
{
	CLRWDT();
}
void interrupt_init(void)
{ 
	INTCONbits.PEIE 	= 1;
	INTCONbits.GIE 		= 0;
}



void debug(void)
{
    static uint8 debug_cnt = 0;
	

    uint16 set_tempt = 390;
    uint16 cur_temp;
	if(debug_cnt ++ %2 == 0)
	{
	 set_tempt = calculate_temperature(29,1);  
	 cur_temp =  calculate_temperature(29,0);	
     usart2_send_byte(0x0d);
     usart2_send_byte(0x0a);
    
     usart2_send_byte(cur_temp>>8);
     usart2_send_byte(cur_temp);

     usart2_send_byte(set_tempt>>8);
     usart2_send_byte(set_tempt);
     usart2_send_byte(0xFF);
     usart2_send_byte(0x0F);
     usart2_send_byte(0x0F);
	}
}

/*****************************************************************************
 函 数 名  : mcu_init
 功能描述  : mcu初始化，初始化时钟，外设等
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :  
 被调函数  :

 修改历史      :
  1.日    期   : 2017年3月9日17:21:26
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void mcu_init(void)
{   
    OSCSTAT = 0;	
	#if(SYSCLK_FREQ_32MHz == 1)
		OSCCON = 0b11110010;		//使用内部8M晶振,使能PLL
	#elif(SYSCLK_FREQ_16MHz == 1)
		OSCCON = 0b01111010;		//使用内部16M晶振
	#elif(SYSCLK_FREQ_8MHz == 1)	
		OSCCON = 0b01110010;		//使用内部8M晶振
	#elif(SYSCLK_FREQ_4MHz == 1)
		OSCCON = 0b01101010;		//使用内部4M晶振
    #elif( SYSCLK_FREQ_2MHz  == 1)	
		OSCCON = 0b01100010; 		//使用内部2M晶振
    #else
		OSCCON = 0b01011010;		//使用内部1M晶振
	#endif	
 	while(!OSCSTATbits.HFIOFR);		//等待晶振稳定

    
	ANSELA = 0x00;            //端A全部为输出
	TRISA = 0x00;
	PORTA = 0x00;
	

	TRISB = 0x00;             //端口B全部为输出
	PORTB = 0x00;
	WPUB = 0x00;

	TRISC = 0x00;             //端口C全部为输出
	PORTC = 0x00;

	TRISD = 0x00;             //端口D全部为输出
	PORTD = 0x00;

    ANSELE = 0x00;
	TRISE = 0x00;             //端口E全部为输出
	PORTD = 0x00;


    ANSELF = 0x00;
	TRISF = 0x00;			  //端口F全部为输出
	PORTF = 0x00;
	
    ANSELG = 0x00;
	TRISG = 0x00;
	PORTG = 0x00;
	
	WPUG = 0x00;

	IOCBP = 0x00;
	IOCBN = 0x00;
	IOCBF = 0x00;

	FVREN = 0;				//禁止固定参考电

	DACEN = 0;
	C1ON = 0;
	C2ON = 0;
	C3ON = 0;
	SRLEN = 0;
	TMR1ON = 0;
	TMR2ON = 0;
	TMR4ON = 0;
	TMR6ON = 0;
	CCP3CON = 0x00;
	CCP4CON = 0x00;
	CCP5CON = 0x00;
	CPSON = 0;
	LCDEN = 0;
}

/*****************************************************************************
 函 数 名  : system_init
 功能描述  : 系统相关硬件和模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  : 调用相关系统硬件和模块初始化函数
 被调函数  :

 修改历史      :
  1.日    期   : 2017年3月9日17:21:26
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void system_init(void)
{
    interrupt_init();	           //中断初始化
	mcu_init();                    //MCU初始化
	water_level_in_int();          //液位传感器初始化
	traic_init();                  //可控硅控制初始化
	adc_init();                    //adc初始化
	open_timer_1(1,1,0,1);         // 时基1ms 
	open_timer_6(5, 1, 0, 1);    // 时基100us 
	usart1_init(9600);  
	usart2_init(9600);             //串口2初始化 
	
	lamp_init();                   //水底灯初始化，上电亮绿灯
	
	interrupt_enable();            //使能中断
	//drain_valve_init();            //缸底排水阀初始化
    pid_init();
//有进行功率控制
#if ( MASSAGE_CTRL_MODE > 0)     
    zero_corss_init();             //过零初始化
    
 #endif
}

/*****************************************************************************
 函 数 名  : task_run
 功能描述  : 执行相关系统任务
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年3月9日17:21:26
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/
void task_run(void)
{
    uint8 len = sizeof(tasks)/sizeof(_task_) ;    //计算任务的个数   
   // system_tick = 0; 
    while(1)
    {
        for(sys_task = tasks; sys_task < tasks+len; sys_task++)        //查询是否有任务需要执行
        {
            if((system_tick - sys_task->time) >= ( sys_task->period )) //查询任务周期是否到达
            {
                sys_task->time = system_tick;   //时间片重新赋值 
                sys_task->func_process();       //调用任务
            }
        }
        feed_wdt();          //喂狗
    }  
}
/*****************************************************************************
 函 数 名  : interrupt ISR
 功能描述  : 执行相关中断任务
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年3月9日17:21:26
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/ 


volatile uint8 zero_flag = 0;
volatile uint8 pr6_temp = 5;
void interrupt ISR (void)
{
	/*******************************/
	if((IOCIE ==1)&&(IOCBF0 == 1))
	{
	   // traic_ctrl(AIR_PUMP,OFF);
        IOCBF0 = 0;
        TMR6ON = 1;
        zero_flag = 1; 
#if MASSAGE_CTRL_MODE  >0        //有进行功率控制       
        //massage_ctrl();	
#endif

	}
	if(TMR0IF & TMR0IE)
	{
	     TMR0IF = 0;
	     TMR0 = 156;          //定时器赋初值
	   //  LATD7 = !LATD7;
	 //    lamp_out_process();
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
		   pr6_temp= rec_h;
		   // set_point(rec_h);
		   //rec_from_wifi_int();       //接收WIFI数据
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
		    //set_point(rec_h);
			 rec_temp_box_int();         //串口2接收温控盒数据
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
		    if(zero_flag ==1)
            {
               PR6 = 4;
               zero_flag = 0;
               traic_ctrl(AIR_PUMP,ON);
            }
	        else
            {
                TMR6ON = 0;
                PR6 = pr6_temp;
                traic_ctrl(AIR_PUMP,OFF);
            }
		   
		    //step_motors_intr_func();
	    }
	}
	// 系统时钟滴答
	if (TMR1IF & TMR1IE)
	{
		TMR1IF = 0;			 
	    set_timer_1_period();  //设定定时器初始值
        system_tick ++;  	   //系统时间增加
	}	
}

