#include "com_config.h"
#include "system.h"
#include "traic.h"
#include"temperature.h"
#include "water_level_sensor.h"

#define MAX_TEMPT      42
#define MIN_TEMPT      15
#define TIME_1S        100
#define TEMPT_TIME     100
volatile static uint16 keep_warm_ctrl_time = 0;     //保温控制时间
volatile static uint8 keep_warm_sym = 1;            //保温控制标志位
volatile static uint8 set_keep_tempt = 0;
volatile static uint8 tempt_compare_resault = 0;

void set_keep_warm_cmd(uint8 cmd)
{
     if(cmd == 1)
     {
         keep_warm_sym = ON;
     }
	 else
	 {
         keep_warm_sym = OFF;
	 }
}
void set_keep_warm_tempt(uint8 tempt)
{
      if(tempt > MAX_TEMPT)
	      set_keep_tempt = MAX_TEMPT;
	  else if(tempt < MIN_TEMPT)
	  	  set_keep_tempt = MIN_TEMPT;
	  else 
	  	  set_keep_tempt = tempt;
	  keep_warm_sym = ON;                     //设定保温温度时，开启保温功能
}
uint8 read_keep_warm_tempt(void)   //读取保温温度
{
     return set_keep_tempt;
}
	 uint8  current_tempt = 0;
static tempt_compare(void)
{
     static uint16 tempt_cnt1 = 0,tempt_cnt2 = 0;
	 //uint8  current_tempt = 0;
	 current_tempt = (uint8)(calculate_temperature(29,0));
	 if(current_tempt < set_keep_tempt-1)
	 {
	     tempt_cnt2 = 0;
         tempt_cnt1++;
		 if(tempt_cnt1 >TEMPT_TIME)
		 {
            tempt_cnt1 = 0;
            tempt_compare_resault = 1;
			//此处增加相关代码
		 }
	 }
	 else if(current_tempt > set_keep_tempt +1)
	 {
         tempt_cnt1 = 0;
         tempt_cnt2++;
		 if(tempt_cnt2 >TEMPT_TIME)
		 {
            tempt_cnt2 = 0;
            tempt_compare_resault = 0;
			//此处增加相关代码
		 }
	 }   
	 else
	 {
         tempt_cnt1 = 0;
		 tempt_cnt2 = 0;
	 }
}
void keep_warm_in_mian(void)
{
	 tempt_compare();                   //读取当前水温与目标温度比较	 	 
	 if((read_water_level(L_WATER_SENSOR) == ON)    //保温功能开起来，且水位漫过低液位
	 	 &&(tempt_compare_resault == 1))
	 	// &&(tempt_compare_resault == 1))       //当前水温低于保温温度
	 {
          traic_ctrl(HEAT_TUB ,ON);            //开加热管
		  traic_ctrl(CIR_PUMP,ON);             //开紫外杀菌和循环泵
		
	 }
	 else
	 {
          traic_ctrl(HEAT_TUB,OFF);
		  traic_ctrl(CIR_PUMP ,OFF);
		  
	 }	 
}


