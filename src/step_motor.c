/**********************************************************************
* step_motor.c
* 步进电机执行模块
*  模块功能：按照调用执行相应步进电机的相应步数
*   				 保存各个步进电机当前的状态（当前所走步数、是否达到目标位置）
*********************************************************************/
#include "com_config.h"
#include "system.h"
#include "timer.h"
#include "uart.h"
#include "io_setting.h"
#include "uart.h"


//#include <flow_ctrl.h>
/**********需要修改的部分  start******************************************************/
/**********需要修改的部分  end******************************************************/
#define MOTOR_INTERVAL_US  100
/*******************************************************************************************/
#define FLOW_STEP_MOTOR 0
#define TEMP_STEP_MOTOR 1
#define STEP_MOTOR_NUM 2

#define DIREC_P 0
#define DIREC_N 1

static void motor_ios_init(uint8 num);
static void motor_ios_deinit(uint8 num);
static void set_a3984_dir(uint8 num, uint8 dir);
static void set_a3984_step(uint8 num, uint8 value);
static void step_motor_sleep(uint8 num);
static void step_motor_active(uint8 num);
// 实时记录每个步进电机的步数和拍数
//让外部检测步进电机是否已经达到目标位置
// static struct _motors_running_params 
struct _motors_running_params 
{
	int16 current_pos;
	int16 step_count;
	//uint8 move_dir;
	int16 target_pos;
	int16 init_cnt;
	uint16 pps_time;
	uint16 timer_counter;
	unsigned pluse:  3;
	unsigned is_motor_ok: 1;	
}step_motor[2];

/****************步进电机IO初始化***********************/
static void motor_ios_init(uint8 num)
{
	if(num == FLOW_STEP_MOTOR) 
	{
		A3984_FLOW_IOS_OUTPUT;
		
		SET_A3984_RST_FLOW(1);
		SET_A3984_DIR_FLOW(1);
		SET_A3984_STEP_FLOW(1);
		SET_A3984_EN_FLOW(0);
		SET_A3984_MS1_FLOW(1);
		SET_A3984_MS2_FLOW(1);
	}
	else if(num == TEMP_STEP_MOTOR) 	
	{
		A3984_TEMP_IOS_OUTPUT;
		SET_A3984_RST_TEMP(1);
		SET_A3984_DIR_TEMP(1);
		SET_A3984_STEP_TEMP(1);
		SET_A3984_EN_TEMP(0);
		SET_A3984_MS1_TEMP(1);
		SET_A3984_MS2_TEMP(0);
	}
}

/****************设置步进电机方向***********************/
static void set_a3984_dir(uint8 num, uint8 dir)
{
	if(num == FLOW_STEP_MOTOR) 
	{
		if(dir == DIREC_P)
		{
			SET_A3984_DIR_FLOW(1);
		
		}
		else 
		{
			SET_A3984_DIR_FLOW(0);
		}		
	} 
	else if(num == TEMP_STEP_MOTOR)
	{
		if(dir == DIREC_P) 
		{
			SET_A3984_DIR_TEMP(0);
		} 
		else 
		{
			SET_A3984_DIR_TEMP(1);
		}	
	}
}

/****************步进电机脉冲设定***********************/
static void set_a3984_step(uint8 num, uint8 value)
{
	if(num == FLOW_STEP_MOTOR)
	{
		SET_A3984_STEP_FLOW(value);			
	} 
	else if(num == TEMP_STEP_MOTOR) 
	{
		SET_A3984_STEP_TEMP(value);	
	}
}
/****************步进电机休眠***********************/
static void step_motor_sleep(uint8 num)
{
    if(num == FLOW_STEP_MOTOR)
	{
		SET_A3984_RST_FLOW(0) ;		
	} 
	else if(num == TEMP_STEP_MOTOR) 
	{
		SET_A3984_RST_TEMP(0) ;	
	}
}
/****************步进电机动作***********************/
static void step_motor_active(uint8 num)
{
    if(num == FLOW_STEP_MOTOR)
	{
		SET_A3984_RST_FLOW(1) ;		
	} 
	else if(num == TEMP_STEP_MOTOR) 
	{
		SET_A3984_RST_TEMP(1) ;	
	}
}

void set_step_motor_stop(uint8 num)
{
	step_motor[num].is_motor_ok 	= 1;
	step_motor[num].step_count 		= 0;
	step_motor_sleep(num);
}

/****************获取步进电机到位标志***********************/
uint8 is_motor_ok(uint8 num)
{
	return step_motor[num].is_motor_ok;
}
/****************获取目标位置***********************/
int16 get_motor_target_pos(uint8 num)
{
	return step_motor[num].target_pos;
}
/****************获取当前位置***********************/
int16 get_motor_current_pos(uint8 num)
{
	return step_motor[num].current_pos;
}


//此函数用于步进电机校正
void set_motor_current_pos(uint8 num ,int16 adj_pos)
{
     step_motor[num].current_pos = step_motor[num].current_pos+adj_pos;
}
void clear_motor_pos(uint8 num)
{
	 step_motor[num].current_pos = 0;
	 usart2_send_byte('o');
     usart2_send_byte('\n');
}
/****************设定步进电机基准位置***********************/

/****************设定步进电机PPS时间***********************/
void set_step_motor_pps_time(uint8 num, int16 target, uint16 motor_pps_100us)
{
	step_motor[num].target_pos = target;			
	step_motor[num].pps_time = motor_pps_100us ;
}

/****************设定步进电机目标位置和PPS时间***********************/
void set_step_motor_running(uint8 num, int16 target, uint16 motor_pps_100us)
{	
	interrupt_disable();
	//motor_ios_init(num);
	//if(num ==TEMP_STEP_MOTOR)
	//	step_motor[num].current_pos = 0;
	step_motor[num].target_pos = target;
	step_motor[num].pps_time = motor_pps_100us;
	step_motor[num].is_motor_ok = 0;
	step_motor_active(num);
	interrupt_enable();
}
/****************步进电机初始化***********************/
void step_motors_init(void)
{

	motor_ios_init(TEMP_STEP_MOTOR);        //温控步进电机IO初始化
	interrupt_disable();
	step_motor[TEMP_STEP_MOTOR].current_pos = -300;
	set_step_motor_running(TEMP_STEP_MOTOR, 0, 10);	
	interrupt_enable();
	while(1)
	{
		feed_wdt();
		if(step_motor[TEMP_STEP_MOTOR].is_motor_ok ==1)
		{  
		 break;
		}	
	}	
}
/**************************************************************************************/
// 步进电机中断函数
void step_motors_intr_func(void)
{
	static uint8 index  =  0;
	for(index = 0; index < STEP_MOTOR_NUM; index++)
	{
		if(step_motor[index].is_motor_ok == 0) 
		{
			if(step_motor[index].timer_counter > 0)
			{
				step_motor[index].timer_counter--;
			}			
			if(step_motor[index].timer_counter > 0)
				continue;
			
			step_motor[index].timer_counter = step_motor[index].pps_time;			

			if(step_motor[index].pluse != 0) 
			{
				set_a3984_step(index, 0);			
				step_motor[index].pluse = 0;
			} 
			else
			{
				if(step_motor[index].current_pos < step_motor[index].target_pos) 
				{		
					set_a3984_dir(index, DIREC_P);
					
					step_motor[index].current_pos++;
				
					set_a3984_step(index, 1);
					
					step_motor[index].pluse = 1;

				} 
				else if(step_motor[index].current_pos > step_motor[index].target_pos) 
				{ // 反向
				
					set_a3984_dir(index, DIREC_N);
									
					step_motor[index].current_pos--;
					
					set_a3984_step(index, 1);
					
					step_motor[index].pluse = 1;
					
				}
				else 
			    {
					step_motor[index].is_motor_ok 	= 1;
					step_motor_sleep(index); 				
					//DEBUG 监控电机是否转到目标位置
					if(index==FLOW_STEP_MOTOR)
					{
						usart1_send_byte('c');
		                usart1_send_byte('\n');
					}
					if(index==TEMP_STEP_MOTOR)
					{
						 usart1_send_byte('f');
		                 usart1_send_byte('\n');
					}
			    } 
			}	
		}		
	}
}
// 设置电机的pps的时间
/**************************************************************************************/
