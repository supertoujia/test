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
#define  MAX_TASKS    30                                        /* ���������*/
#define  MS  *8  


/**********************************��غ�������****************************************/
void mcu_init(void);                    //MCU��ʼ��
void interrupt_enable(void);            //ʹ���ж�
void interrupt_disable(void);           //��ֹ�ж�

void feed_wdt(void);                    //ι��
void wdt_disable(void);                 //��ֹ���Ź�
void wdt_enable(void);                  //ʹ�ܿ��Ź�
void debug(void);                        //ϵͳ����



/**********************************��ر�������****************************************/

//���������������ṹ
typedef struct 
{
    void (*func_process)(void);          //��������ָ��
    uint16 period;                       //�����Ӧ��ʱ��Ƭ���ڣ�֧�����65535*ϵͳʱ���׼
    uint16 time ;                       //ÿ��ִ�������Ӧ��ʱ��
} _task_;

static _task_ tasks[] ={
    {host_com_in_main,100,0},           //���ڽ��ܴ���
    {massage_ctrl_in_mian,100,0},       //��Ħ������ƣ�100msִ��һ��
    {keep_warm_in_mian,100,0},          //ѭ�����£�100msִ��һ��
    {water_level_judge,100,0},          //Һλ�жϣ�100msִ��һ��
    {adc_go_on,10,0},                   //AD������10ms����һ�� 
 //   {lamp_1ms_process,1,0},
    {lamp_process,10,0},
    { debug,100,0},
 //   {fan_heat_process,10,0},

#if MASSAGE_CTRL_MODE  >0               //�н��й��ʿ���     
   // {massage_power_process,10,0},

#endif   
};
     
static _task_ *sys_task;        //����һ����������ָ��
static uint16 system_tick = 0;  //ϵͳ�δ�ʱ��



 


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
	WDTCONbits.SWDTEN = 1; // ����λ����: WDTPS
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
 �� �� ��  : mcu_init
 ��������  : mcu��ʼ������ʼ��ʱ�ӣ������
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :  
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��9��17:21:26
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void mcu_init(void)
{   
    OSCSTAT = 0;	
	#if(SYSCLK_FREQ_32MHz == 1)
		OSCCON = 0b11110010;		//ʹ���ڲ�8M����,ʹ��PLL
	#elif(SYSCLK_FREQ_16MHz == 1)
		OSCCON = 0b01111010;		//ʹ���ڲ�16M����
	#elif(SYSCLK_FREQ_8MHz == 1)	
		OSCCON = 0b01110010;		//ʹ���ڲ�8M����
	#elif(SYSCLK_FREQ_4MHz == 1)
		OSCCON = 0b01101010;		//ʹ���ڲ�4M����
    #elif( SYSCLK_FREQ_2MHz  == 1)	
		OSCCON = 0b01100010; 		//ʹ���ڲ�2M����
    #else
		OSCCON = 0b01011010;		//ʹ���ڲ�1M����
	#endif	
 	while(!OSCSTATbits.HFIOFR);		//�ȴ������ȶ�

    
	ANSELA = 0x00;            //��Aȫ��Ϊ���
	TRISA = 0x00;
	PORTA = 0x00;
	

	TRISB = 0x00;             //�˿�Bȫ��Ϊ���
	PORTB = 0x00;
	WPUB = 0x00;

	TRISC = 0x00;             //�˿�Cȫ��Ϊ���
	PORTC = 0x00;

	TRISD = 0x00;             //�˿�Dȫ��Ϊ���
	PORTD = 0x00;

    ANSELE = 0x00;
	TRISE = 0x00;             //�˿�Eȫ��Ϊ���
	PORTD = 0x00;


    ANSELF = 0x00;
	TRISF = 0x00;			  //�˿�Fȫ��Ϊ���
	PORTF = 0x00;
	
    ANSELG = 0x00;
	TRISG = 0x00;
	PORTG = 0x00;
	
	WPUG = 0x00;

	IOCBP = 0x00;
	IOCBN = 0x00;
	IOCBF = 0x00;

	FVREN = 0;				//��ֹ�̶��ο���

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
 �� �� ��  : system_init
 ��������  : ϵͳ���Ӳ����ģ���ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  : �������ϵͳӲ����ģ���ʼ������
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��9��17:21:26
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void system_init(void)
{
    interrupt_init();	           //�жϳ�ʼ��
	mcu_init();                    //MCU��ʼ��
	water_level_in_int();          //Һλ��������ʼ��
	traic_init();                  //�ɿع���Ƴ�ʼ��
	adc_init();                    //adc��ʼ��
	open_timer_1(1,1,0,1);         // ʱ��1ms 
	open_timer_6(5, 1, 0, 1);    // ʱ��100us 
	usart1_init(9600);  
	usart2_init(9600);             //����2��ʼ�� 
	
	lamp_init();                   //ˮ�׵Ƴ�ʼ�����ϵ����̵�
	
	interrupt_enable();            //ʹ���ж�
	//drain_valve_init();            //�׵���ˮ����ʼ��
    pid_init();
//�н��й��ʿ���
#if ( MASSAGE_CTRL_MODE > 0)     
    zero_corss_init();             //�����ʼ��
    
 #endif
}

/*****************************************************************************
 �� �� ��  : task_run
 ��������  : ִ�����ϵͳ����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��9��17:21:26
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/
void task_run(void)
{
    uint8 len = sizeof(tasks)/sizeof(_task_) ;    //��������ĸ���   
   // system_tick = 0; 
    while(1)
    {
        for(sys_task = tasks; sys_task < tasks+len; sys_task++)        //��ѯ�Ƿ���������Ҫִ��
        {
            if((system_tick - sys_task->time) >= ( sys_task->period )) //��ѯ���������Ƿ񵽴�
            {
                sys_task->time = system_tick;   //ʱ��Ƭ���¸�ֵ 
                sys_task->func_process();       //��������
            }
        }
        feed_wdt();          //ι��
    }  
}
/*****************************************************************************
 �� �� ��  : interrupt ISR
 ��������  : ִ������ж�����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��9��17:21:26
 ��    ��  : SJY
 �޸�����  : �����ɺ���

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
#if MASSAGE_CTRL_MODE  >0        //�н��й��ʿ���       
        //massage_ctrl();	
#endif

	}
	if(TMR0IF & TMR0IE)
	{
	     TMR0IF = 0;
	     TMR0 = 156;          //��ʱ������ֵ
	   //  LATD7 = !LATD7;
	 //    lamp_out_process();
	}	
	/* ����1�������� */	
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
		   //rec_from_wifi_int();       //����WIFI����
		}
	}
	if(TXIE & TXIF)
	{ 
	    TXIF = 0;
		usart1_send_interrupt();
	}
	/* ����2�������� */	
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
			 rec_temp_box_int();         //����2�����¿غ�����
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
	// �������
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
	// ϵͳʱ�ӵδ�
	if (TMR1IF & TMR1IE)
	{
		TMR1IF = 0;			 
	    set_timer_1_period();  //�趨��ʱ����ʼֵ
        system_tick ++;  	   //ϵͳʱ������
	}	
}

