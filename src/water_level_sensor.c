#include "com_config.h"
#include "system.h"


#define  H_LEVLE_SENSOR_INPUT()    {ANSA2 = 0;TRISA2 = 1;LATA2 = 0;}
#define  L_LEVLE_SENSOR_INPUT()    {ANSA3 = 0;TRISA3 = 1;LATA3 = 0;}

#define H_WATER_LEVEL           RA2                  //高液位传感器
#define L_WATER_LEVEL           RA3                   //低液位传感器


#define ON_TIME                  50                  // 消抖时间
#define OFF_TIME                 30                 //


volatile unsigned  char h_level_sign;                    //高液位信号
volatile unsigned  char l_level_sign;                    //低液位信号

void water_level_judge(void)
{
    static unsigned char l_level_on_cnt = 0,h_level_on_cnt = 0,
		                 l_level_off_cnt= 0,h_level_off_cnt = 0;
		                 
    if(H_WATER_LEVEL == 0)
	{
		h_level_on_cnt ++;
		h_level_off_cnt = 0;
		if(h_level_on_cnt >ON_TIME)       //消抖
		{
			h_level_on_cnt = 0;
			h_level_sign = ON;
		}
	}
	else
	{
    	h_level_off_cnt++; 
		h_level_on_cnt = 0;
		if(h_level_off_cnt >OFF_TIME)     //消抖
		{
			h_level_off_cnt = 0;
			h_level_sign = OFF;
		}
	}
	if(L_WATER_LEVEL == 0)
	{
		l_level_on_cnt++;
		l_level_off_cnt = 0;
		if(l_level_on_cnt >ON_TIME)    //消抖
		{
			l_level_on_cnt = 0;
			l_level_sign = ON;
		}
	}
	else
	{
    	l_level_off_cnt++;
		l_level_on_cnt = 0;
		if(l_level_off_cnt >OFF_TIME)  //消抖
		{
			l_level_off_cnt = 0;
			l_level_sign = OFF; 		
		}
	}			
}


//水位传感器初始化

void water_level_in_int(void)
{

   H_LEVLE_SENSOR_INPUT();               
   L_LEVLE_SENSOR_INPUT();
   h_level_sign = OFF; 
   l_level_sign = OFF; 

}

uint8 read_water_level(uint8 num)
{
   if(1 == num)
       return h_level_sign;
   else
       return l_level_sign;
}








