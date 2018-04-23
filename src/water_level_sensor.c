#include "com_config.h"
#include "system.h"


#define  H_LEVLE_SENSOR_INPUT()    {ANSA2 = 0;TRISA2 = 1;LATA2 = 0;}
#define  L_LEVLE_SENSOR_INPUT()    {ANSA3 = 0;TRISA3 = 1;LATA3 = 0;}

#define H_WATER_LEVEL           RA2                  //��Һλ������
#define L_WATER_LEVEL           RA3                   //��Һλ������


#define ON_TIME                  50                  // ����ʱ��
#define OFF_TIME                 30                 //


volatile unsigned  char h_level_sign;                    //��Һλ�ź�
volatile unsigned  char l_level_sign;                    //��Һλ�ź�

void water_level_judge(void)
{
    static unsigned char l_level_on_cnt = 0,h_level_on_cnt = 0,
		                 l_level_off_cnt= 0,h_level_off_cnt = 0;
		                 
    if(H_WATER_LEVEL == 0)
	{
		h_level_on_cnt ++;
		h_level_off_cnt = 0;
		if(h_level_on_cnt >ON_TIME)       //����
		{
			h_level_on_cnt = 0;
			h_level_sign = ON;
		}
	}
	else
	{
    	h_level_off_cnt++; 
		h_level_on_cnt = 0;
		if(h_level_off_cnt >OFF_TIME)     //����
		{
			h_level_off_cnt = 0;
			h_level_sign = OFF;
		}
	}
	if(L_WATER_LEVEL == 0)
	{
		l_level_on_cnt++;
		l_level_off_cnt = 0;
		if(l_level_on_cnt >ON_TIME)    //����
		{
			l_level_on_cnt = 0;
			l_level_sign = ON;
		}
	}
	else
	{
    	l_level_off_cnt++;
		l_level_on_cnt = 0;
		if(l_level_off_cnt >OFF_TIME)  //����
		{
			l_level_off_cnt = 0;
			l_level_sign = OFF; 		
		}
	}			
}


//ˮλ��������ʼ��

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








