#include "com_config.h"
#include "system.h"
#include "step_motor.h"

#define   DRAIN_VALVE_OPEN_POZ     -300            //排水阀开启的位置
#define   DRAIN_VALVE_CLOSE_POZ     0X000          //排水阀关闭的位置
#define   DRAIN_MOTOR               1
#define   MOTOR_PPS                 40
  

volatile static uint8 drain_state = 0; 
volatile static uint8 last_drain_state = 0; 


void drain_valve_init(void);
void drain_water_ctrl(uint8 cmd,uint8 value);
void open_valve(void);
void close_valve(void);


void drain_valve_init(void)
{
    step_motors_init();	           //步进电机模块初始化 
    last_drain_state = 0;
}
void drain_water_ctrl(uint8 cmd,uint8 value)
{
     volatile uint8 cmd_temp = cmd;
	 if(0x80 == value)
	 { 
	     drain_state = 1;  
		 open_valve();
	 }
	 else if(0x00 == value)
	 {
	     drain_state = 2;
		 close_valve();
	 }
}
void open_valve(void)
{
    if((is_motor_ok(DRAIN_MOTOR) == 1)
		&&(drain_state == 1)
		&&(last_drain_state == 0))
    {
        set_step_motor_running(DRAIN_MOTOR,DRAIN_VALVE_OPEN_POZ, MOTOR_PPS );            
        drain_state = 0; 
		last_drain_state = 1;
    }
}
void close_valve(void)
{
    if((is_motor_ok(DRAIN_MOTOR) == 1)
		&&(drain_state == 2)
		&&(last_drain_state == 1))
    {
        set_step_motor_running(DRAIN_MOTOR,DRAIN_VALVE_CLOSE_POZ, MOTOR_PPS );            
        drain_state = 0; 
		last_drain_state = 0;
    }
}



















