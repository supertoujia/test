********************************************************************************
* tempt_ctrl.c
*
*********************************************************************************/
#include "com_config.h"

#include "step_motor.h"
#include"temperature.h"
#include "flow_ctrl.h"
#include "uart2_com.h"
#include "uart.h"
#include "led.h"

/*************************相关时间常数****************************/
#define TIMEOUT_20_MS 1
#define TIMEOUT_50_MS 10
/*************************温度相关参数****************************/
#define MOTOR_FASTEST_PPS_TIMER	3
#define MOTOR_SLOWEST_PPS_TIMER	103 // 103
#define MAX_TEMPT	         460         //最大设定温度
#define MIN_TEMPT	         150         //最小设定温度
#define MAX_E_TEMPT			 100         //最大温度偏差
#define MIN_STOP_E_TEMPT 	 10          //设定停止调整温度
#define MIN_E_TEMPT			 30          //最小温度偏差
#define DEFAULT_TEMPT        380         //默认温度
#define HIGH_ACCURACY        5           //高精度为0.5度
#define MID_ACCURACY         10          //高精度为1度
#define LOW_ACCURACY         20          //高精度为2度
#define T1	                  0          //温度传感器1                
#define T2	                  1          //温度传感器2
/*************************水路状态*******************************/
#define NORMAL                 0      //混水正常
#define HOT_WATER_ERROR        1      //热水故障
#define COLD_WATER_ERROR       2      //冷水故障 

/*************************温控主状态****************************/
#define TEMPT_CLOSE            0      //关水状态
#define TEMPT_P_ADJUST         1      //温度主动调节
#define TEMPT_N_ADJUST         2      //温度被动调节
#define WAIT_FOR_NORMAL        3      //等待水路正常
/*************************温控命令******************************/
#define WATER_OUT      0x31      //开水
#define WATER_CLOSE    0x32      //关水
#define TEMPT_ADJUST   0x33      //温度调整
#define FLOW_ADJUST    0x34		 //流量控制	
#define VALVE_CTRL     0x35      //阀门控制

static uint8    tempt_crtl_state;                //默认温度控制处于关水状态
static uint8  	water_state = NORMAL;            //水路状态
static uint8  	e_symbol = 0;                    //偏差标志，1或者0
static uint8  	pre_e_symbol = 0;                //偏差标志，1或者0

static uint16   tempt_accuracy = 10;          //默认温度调整精度为1度
static uint16   reference_tempt = 0;          //失效判断参考温度值
static uint16 	e_tempt = 0;                //偏差
static uint16 	pre_e_tempt = 0;            //上一次偏差
static uint16 	pree_e_tempt = 0;            //前一次偏差
static int16    delta_e_temp = 0;           //两次偏差之差
static int16    delta_temp = 0;             //两次温度采样只差
static int16    pre_delta_e_temp = 0;       //前两次偏差之差
static int16    e_tempt_product = 0;        //两次偏差之积
static int16    pre_e_tempt_product = 0;    //前两次偏差之差
static int16 	tempt_motor_pps_100us = 0;  //PPS
static int16 	pre_tempt_motor_pps_100us = 0; //上一次PPS
	
static uint16 	cur_adc_tempt = 0;            //当前温度
static uint16 	cur_tempt = 0;
static uint16 	pre_cur_tempt = 0;
static uint16 	pree_cur_tempt = 0;
static uint16 	preee_cur_tempt = 0;
static uint8    poz_index;                   //目标位置索引
static uint16   set_tempt;                   //设定温度值
static uint16   tempt_ctrl_timer;            //控制时间

		


int16 const high_tempt_dir[36] = { -6038,-5994,-5927,-5816,-5772,-5639,-5572,-5439,-5350,-5217,-5106,
	                          -4951,-4817,-4706,-4595,-4440,-4351,-4174,-4040,-3818,-3663,-3485,
	                          -3263,-2930,-2620,-2309,-2042,-1665,-1376,-999,-777,-555,-377,-266,-133,0 };
	                          
int16 const low_tempt_dir[36] = {0,44,111,222,266,400,466,599,688,821,932,1088,1221,
	                            1332,1443,1598,1687,1865,1998,2220,2375,2553,2775,3108,
	                            3419,3730,3996,4373,4662,5039,5261,5483,5661,5772,5905,6038};

uint16 read_temp(void)
{
    uint16 adc_temp;
	adc_temp = calculate_temperature(29,T1);	
	// 1、温度处理一次
	cur_tempt =  (adc_temp + pre_cur_tempt + pree_cur_tempt + preee_cur_tempt)>>2;

	//两次温度采样之差
    if(cur_tempt > pre_cur_tempt)
	{
       delta_temp =cur_tempt -pre_cur_tempt;
	}
	else
	{
	   delta_temp = pre_cur_tempt -cur_tempt;
	}
	// 2、目标温度与当前温度比较，pid计算输出电机转速
	// 目前简单线性处理
	pre_e_symbol = e_symbol;
	if(set_tempt >= cur_tempt)              //目标温度大于当前温度e_symbol =0
	{
		e_symbol = 0;
		e_tempt = set_tempt - cur_tempt;
	}
	else
	{
		e_symbol = 1;                                      //目标温度小于当前温度e_symbol =1
		e_tempt = cur_tempt - set_tempt;
	}	
	delta_e_temp = e_tempt - pre_e_tempt;	   //两次温度偏差之差
	preee_cur_tempt = pree_cur_tempt;
	pree_cur_tempt = pre_cur_tempt;
	pre_cur_tempt = adc_temp;
	pree_e_tempt = pre_e_tempt; 		   //保存前一次偏差
	pre_e_tempt = e_tempt;				   //保存上一次偏差
	return  cur_tempt;                     //返回当前温度值    
}

static void temp_stability_judge(void)
{
    static uint16   adjust_time = 0;            //调整时间
    if(e_tempt >= 100)      //温度突变，马上要进行调整
	{
		adjust_time = 0;
		tempt_crtl_state = TEMPT_P_ADJUST;    //达到目标温度范围，进入被动调整模式
	}
	else if(e_tempt <= 10)
    {
        
		adjust_time = 0;
    }		    
	else if(adjust_time ++>30)
	{
		adjust_time = 0;	
		tempt_crtl_state = TEMPT_P_ADJUST;    //达到目标温度范围，进入被动调整模式
	}	
}
static uint8  pps_time_calculate(void)
{
    static uint16   stop_time = 0;          //停止调整波动时间
    if(e_tempt >= MAX_E_TEMPT)              // 如果温差很大，那么电机PPS到最大
	{
	 	tempt_motor_pps_100us = 6;          //MOTOR_FASTEST_PPS_TIMER;
		stop_time = 0;
	} 
    else if(e_tempt <= MIN_E_TEMPT)
	{  
		if(e_tempt <= tempt_accuracy)//MIN_STOP_E_TEMPT) 
		{      
		        // 如果温差很小，那么停止
		     	// 温差很小，那么停止调整
			if((is_motor_ok(TEMP_STEP_MOTOR) == 0)&&(stop_time>15))
			{
				set_step_motor_stop(TEMP_STEP_MOTOR);
				tempt_crtl_state = TEMPT_N_ADJUST;        //达到目标温度范围，进入被动调整模式
                poz_index =(uint8)(cur_tempt/10.0)-15;    //设置目的位置索引
	            clear_motor_pos(TEMP_STEP_MOTOR);         //设置步电机当前位置为0
				stop_time = 0;
				return 1;
			}
			else
			{
			    stop_time++;
			    tempt_motor_pps_100us = 80;
			}
		}	
		else
		{ 
		    tempt_motor_pps_100us = 60;
		    stop_time = 0;	
		    //温度小于最小调整值，但不小于停止调整值
		}
	} 	
	else
	{		
	    stop_time = 0;			
		tempt_motor_pps_100us = 30;
	}
	// 以下为限幅输出	
	if(tempt_motor_pps_100us < MOTOR_FASTEST_PPS_TIMER)
		tempt_motor_pps_100us = MOTOR_FASTEST_PPS_TIMER;
	else if(tempt_motor_pps_100us > MOTOR_SLOWEST_PPS_TIMER)
		tempt_motor_pps_100us = MOTOR_SLOWEST_PPS_TIMER;
	pre_tempt_motor_pps_100us = tempt_motor_pps_100us;   //保存上一次PPS

	return 0;
}
static tempt_adjust_process(void)
{
    static uint8 err_cnt;
    if(e_symbol == 1)      //目标温度小于当前温度e_symbol =1
    {			
        if((is_motor_ok(TEMP_STEP_MOTOR) == 1)     //从稳定状态切换到调整状态
            ||(pre_e_symbol ==0))                   //上一次调整温度过冲
        {

		    reference_tempt = cur_tempt;           //换向获取冷热水失效参考温度
		    err_cnt = 0;
            if(get_motor_current_pos(TEMP_STEP_MOTOR) != low_tempt_dir[poz_index])
                set_step_motor_running(TEMP_STEP_MOTOR, low_tempt_dir[poz_index], tempt_motor_pps_100us); 
        }
        else 
        {
            if(get_motor_current_pos(TEMP_STEP_MOTOR) != low_tempt_dir[poz_index])
                set_step_motor_pps_time(TEMP_STEP_MOTOR, low_tempt_dir[poz_index], tempt_motor_pps_100us);
        }	
    } 
    else 
    {
        if((is_motor_ok(TEMP_STEP_MOTOR) == 1)     //从稳定状态切换到调整状态
           ||(pre_e_symbol ==1))                   //上一次调整温度过冲
        {
             reference_tempt = cur_tempt;           //换向获取冷热水失效参考温度
             err_cnt = 0;
            if(get_motor_current_pos(TEMP_STEP_MOTOR) != high_tempt_dir[poz_index])
                set_step_motor_running(TEMP_STEP_MOTOR, high_tempt_dir[poz_index], tempt_motor_pps_100us);
        }
        else
        {
           if(get_motor_current_pos(TEMP_STEP_MOTOR) != high_tempt_dir[poz_index])
           set_step_motor_pps_time(TEMP_STEP_MOTOR, high_tempt_dir[poz_index], tempt_motor_pps_100us);
        }	
    }
	/*********************以下为温度失效判断处理**********************/	
/*	 if(err_cnt ++>20)             //转动3s后判断冷热水是否失效
 	 {
 	      if(e_symbol ==1)
 	      {
 	          if(cur_tempt >= reference_tempt -10) //冷水失效
 	          	{
 	          	    err_cnt = 0;
 	          	    set_step_motor_stop(TEMP_STEP_MOTOR);
			        water_state = COLD_WATER_ERROR	;
					tempt_crtl_state = WAIT_FOR_NORMAL;
 	          	}
 	      }
		  else if(e_symbol ==0)
 	      {
 	          if(cur_tempt <= reference_tempt +10) //热水失效
 	          	{
 	          	    err_cnt = 0;
 	          	    set_step_motor_stop(TEMP_STEP_MOTOR);
			  	    water_state = HOT_WATER_ERROR;
					tempt_crtl_state = WAIT_FOR_NORMAL;
 	          	}
 	      }
 	 }*/
}
void tempt_ctrl_module_main(void)
{ 
    static uint8 adjust_cnt = 0;
	// 10ms 判断一次
	if(tempt_ctrl_timer > 0)
		return;
	tempt_ctrl_timer = TIMEOUT_50_MS;
    (void)read_temp();                   //读取水温 
    switch(tempt_crtl_state)
	{
	    case TEMPT_CLOSE :            	//关水状态，即关闭温度调整
		{   	
			break;     
	    }
		case TEMPT_P_ADJUST:           //温度主动调节   
		{
			if(pps_time_calculate() ==1)  break; //pps时间计算	
			if(adjust_cnt ++ >150)
			{
			   tempt_accuracy = LOW_ACCURACY;   //调整时间超过15s，降低调整精度
			}
			tempt_adjust_process();			
			break;                  
	    }
		case TEMPT_N_ADJUST:           //温度被动调节
		{
		    temp_stability_judge();
			break;
	    }
		case WAIT_FOR_NORMAL:          //等待水路恢复正常状态
		{
		//	get_right_judge();
			break;
	    }
		default:
		{
			break;     
	    }
	}
}
void tempt_ctrl_module_intr(void)
{
	if(tempt_ctrl_timer > 0)
    {
		tempt_ctrl_timer--;
	}
}
void tempt_ctrl_module_init(void)
{
	// 开机默认38度
	set_tempt = DEFAULT_TEMPT;
	poz_index = 23;                    //默认电机在38度的位置
    clear_motor_pos(TEMP_STEP_MOTOR);  //设置步电机当前位置为0
    tempt_accuracy =MID_ACCURACY;      //默认调整精度为1度
}
// 设置目标温度
void set_water_tempt(uint16 tempt)
{
	if(tempt > MAX_TEMPT)
		set_tempt = MAX_TEMPT;
	else if(tempt < MIN_TEMPT)
		set_tempt = MIN_TEMPT;
	else
		set_tempt = tempt; 
	set_water_leds (tempt, 460, 150);   //灯光指示温度
}

void set_tempt_cmd(uint8 cmd)
{ 

    //cmd =1,温度增加，
    //cmd =0 温度降低
    uint16 data = 0;
    if(0x01 == cmd)
    {
       data = set_tempt +10;  //此处旋钮每旋一格增加0.5度，分度可调
    }
	else if(0x00 == cmd)
	{
       data = set_tempt -10;
	}
	if(data > MAX_TEMPT)
		set_tempt = MAX_TEMPT;
	else if(data < MIN_TEMPT)
		set_tempt = MIN_TEMPT;
	else
		set_tempt = data; 
	set_water_leds (set_tempt, MAX_TEMPT,MIN_TEMPT);          //灯光指示温度
	tempt_crtl_state = TEMPT_P_ADJUST;                        //有调温，进入调整模式

}
void tempt_crtl_process(uint8 func,uint16 value)
{ 
    switch(func)
	{
	    case WATER_OUT:            	      //开水
		{   
			tempt_crtl_state = TEMPT_P_ADJUST;
			break;     
	    }
		case WATER_CLOSE:                 //关水    
		{
			set_step_motor_stop(TEMP_STEP_MOTOR);   //步进电机停止动作
            tempt_crtl_state = TEMPT_CLOSE;
			break;                  
	    }
		case TEMPT_ADJUST:               //温度调整
		{		
			set_water_tempt(value);     //设定目标温度  
			tempt_crtl_state = TEMPT_P_ADJUST;
			break;     
	    }
		case FLOW_ADJUST:              //流量调整
		{
			break;                
	    }
		case VALVE_CTRL:            
		{
			break;     
	    }
		default:
		{
			break;     
	    }
	}
}
uint16 read_set_water_tempt(void)
{

   return set_tempt;
}
/********************************************************************************/
