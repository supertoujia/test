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

/*************************���ʱ�䳣��****************************/
#define TIMEOUT_20_MS 1
#define TIMEOUT_50_MS 10
/*************************�¶���ز���****************************/
#define MOTOR_FASTEST_PPS_TIMER	3
#define MOTOR_SLOWEST_PPS_TIMER	103 // 103
#define MAX_TEMPT	         460         //����趨�¶�
#define MIN_TEMPT	         150         //��С�趨�¶�
#define MAX_E_TEMPT			 100         //����¶�ƫ��
#define MIN_STOP_E_TEMPT 	 10          //�趨ֹͣ�����¶�
#define MIN_E_TEMPT			 30          //��С�¶�ƫ��
#define DEFAULT_TEMPT        380         //Ĭ���¶�
#define HIGH_ACCURACY        5           //�߾���Ϊ0.5��
#define MID_ACCURACY         10          //�߾���Ϊ1��
#define LOW_ACCURACY         20          //�߾���Ϊ2��
#define T1	                  0          //�¶ȴ�����1                
#define T2	                  1          //�¶ȴ�����2
/*************************ˮ·״̬*******************************/
#define NORMAL                 0      //��ˮ����
#define HOT_WATER_ERROR        1      //��ˮ����
#define COLD_WATER_ERROR       2      //��ˮ���� 

/*************************�¿���״̬****************************/
#define TEMPT_CLOSE            0      //��ˮ״̬
#define TEMPT_P_ADJUST         1      //�¶���������
#define TEMPT_N_ADJUST         2      //�¶ȱ�������
#define WAIT_FOR_NORMAL        3      //�ȴ�ˮ·����
/*************************�¿�����******************************/
#define WATER_OUT      0x31      //��ˮ
#define WATER_CLOSE    0x32      //��ˮ
#define TEMPT_ADJUST   0x33      //�¶ȵ���
#define FLOW_ADJUST    0x34		 //��������	
#define VALVE_CTRL     0x35      //���ſ���

static uint8    tempt_crtl_state;                //Ĭ���¶ȿ��ƴ��ڹ�ˮ״̬
static uint8  	water_state = NORMAL;            //ˮ·״̬
static uint8  	e_symbol = 0;                    //ƫ���־��1����0
static uint8  	pre_e_symbol = 0;                //ƫ���־��1����0

static uint16   tempt_accuracy = 10;          //Ĭ���¶ȵ�������Ϊ1��
static uint16   reference_tempt = 0;          //ʧЧ�жϲο��¶�ֵ
static uint16 	e_tempt = 0;                //ƫ��
static uint16 	pre_e_tempt = 0;            //��һ��ƫ��
static uint16 	pree_e_tempt = 0;            //ǰһ��ƫ��
static int16    delta_e_temp = 0;           //����ƫ��֮��
static int16    delta_temp = 0;             //�����¶Ȳ���ֻ��
static int16    pre_delta_e_temp = 0;       //ǰ����ƫ��֮��
static int16    e_tempt_product = 0;        //����ƫ��֮��
static int16    pre_e_tempt_product = 0;    //ǰ����ƫ��֮��
static int16 	tempt_motor_pps_100us = 0;  //PPS
static int16 	pre_tempt_motor_pps_100us = 0; //��һ��PPS
	
static uint16 	cur_adc_tempt = 0;            //��ǰ�¶�
static uint16 	cur_tempt = 0;
static uint16 	pre_cur_tempt = 0;
static uint16 	pree_cur_tempt = 0;
static uint16 	preee_cur_tempt = 0;
static uint8    poz_index;                   //Ŀ��λ������
static uint16   set_tempt;                   //�趨�¶�ֵ
static uint16   tempt_ctrl_timer;            //����ʱ��

		


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
	// 1���¶ȴ���һ��
	cur_tempt =  (adc_temp + pre_cur_tempt + pree_cur_tempt + preee_cur_tempt)>>2;

	//�����¶Ȳ���֮��
    if(cur_tempt > pre_cur_tempt)
	{
       delta_temp =cur_tempt -pre_cur_tempt;
	}
	else
	{
	   delta_temp = pre_cur_tempt -cur_tempt;
	}
	// 2��Ŀ���¶��뵱ǰ�¶ȱȽϣ�pid����������ת��
	// Ŀǰ�����Դ���
	pre_e_symbol = e_symbol;
	if(set_tempt >= cur_tempt)              //Ŀ���¶ȴ��ڵ�ǰ�¶�e_symbol =0
	{
		e_symbol = 0;
		e_tempt = set_tempt - cur_tempt;
	}
	else
	{
		e_symbol = 1;                                      //Ŀ���¶�С�ڵ�ǰ�¶�e_symbol =1
		e_tempt = cur_tempt - set_tempt;
	}	
	delta_e_temp = e_tempt - pre_e_tempt;	   //�����¶�ƫ��֮��
	preee_cur_tempt = pree_cur_tempt;
	pree_cur_tempt = pre_cur_tempt;
	pre_cur_tempt = adc_temp;
	pree_e_tempt = pre_e_tempt; 		   //����ǰһ��ƫ��
	pre_e_tempt = e_tempt;				   //������һ��ƫ��
	return  cur_tempt;                     //���ص�ǰ�¶�ֵ    
}

static void temp_stability_judge(void)
{
    static uint16   adjust_time = 0;            //����ʱ��
    if(e_tempt >= 100)      //�¶�ͻ�䣬����Ҫ���е���
	{
		adjust_time = 0;
		tempt_crtl_state = TEMPT_P_ADJUST;    //�ﵽĿ���¶ȷ�Χ�����뱻������ģʽ
	}
	else if(e_tempt <= 10)
    {
        
		adjust_time = 0;
    }		    
	else if(adjust_time ++>30)
	{
		adjust_time = 0;	
		tempt_crtl_state = TEMPT_P_ADJUST;    //�ﵽĿ���¶ȷ�Χ�����뱻������ģʽ
	}	
}
static uint8  pps_time_calculate(void)
{
    static uint16   stop_time = 0;          //ֹͣ��������ʱ��
    if(e_tempt >= MAX_E_TEMPT)              // ����²�ܴ���ô���PPS�����
	{
	 	tempt_motor_pps_100us = 6;          //MOTOR_FASTEST_PPS_TIMER;
		stop_time = 0;
	} 
    else if(e_tempt <= MIN_E_TEMPT)
	{  
		if(e_tempt <= tempt_accuracy)//MIN_STOP_E_TEMPT) 
		{      
		        // ����²��С����ôֹͣ
		     	// �²��С����ôֹͣ����
			if((is_motor_ok(TEMP_STEP_MOTOR) == 0)&&(stop_time>15))
			{
				set_step_motor_stop(TEMP_STEP_MOTOR);
				tempt_crtl_state = TEMPT_N_ADJUST;        //�ﵽĿ���¶ȷ�Χ�����뱻������ģʽ
                poz_index =(uint8)(cur_tempt/10.0)-15;    //����Ŀ��λ������
	            clear_motor_pos(TEMP_STEP_MOTOR);         //���ò������ǰλ��Ϊ0
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
		    //�¶�С����С����ֵ������С��ֹͣ����ֵ
		}
	} 	
	else
	{		
	    stop_time = 0;			
		tempt_motor_pps_100us = 30;
	}
	// ����Ϊ�޷����	
	if(tempt_motor_pps_100us < MOTOR_FASTEST_PPS_TIMER)
		tempt_motor_pps_100us = MOTOR_FASTEST_PPS_TIMER;
	else if(tempt_motor_pps_100us > MOTOR_SLOWEST_PPS_TIMER)
		tempt_motor_pps_100us = MOTOR_SLOWEST_PPS_TIMER;
	pre_tempt_motor_pps_100us = tempt_motor_pps_100us;   //������һ��PPS

	return 0;
}
static tempt_adjust_process(void)
{
    static uint8 err_cnt;
    if(e_symbol == 1)      //Ŀ���¶�С�ڵ�ǰ�¶�e_symbol =1
    {			
        if((is_motor_ok(TEMP_STEP_MOTOR) == 1)     //���ȶ�״̬�л�������״̬
            ||(pre_e_symbol ==0))                   //��һ�ε����¶ȹ���
        {

		    reference_tempt = cur_tempt;           //�����ȡ����ˮʧЧ�ο��¶�
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
        if((is_motor_ok(TEMP_STEP_MOTOR) == 1)     //���ȶ�״̬�л�������״̬
           ||(pre_e_symbol ==1))                   //��һ�ε����¶ȹ���
        {
             reference_tempt = cur_tempt;           //�����ȡ����ˮʧЧ�ο��¶�
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
	/*********************����Ϊ�¶�ʧЧ�жϴ���**********************/	
/*	 if(err_cnt ++>20)             //ת��3s���ж�����ˮ�Ƿ�ʧЧ
 	 {
 	      if(e_symbol ==1)
 	      {
 	          if(cur_tempt >= reference_tempt -10) //��ˮʧЧ
 	          	{
 	          	    err_cnt = 0;
 	          	    set_step_motor_stop(TEMP_STEP_MOTOR);
			        water_state = COLD_WATER_ERROR	;
					tempt_crtl_state = WAIT_FOR_NORMAL;
 	          	}
 	      }
		  else if(e_symbol ==0)
 	      {
 	          if(cur_tempt <= reference_tempt +10) //��ˮʧЧ
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
	// 10ms �ж�һ��
	if(tempt_ctrl_timer > 0)
		return;
	tempt_ctrl_timer = TIMEOUT_50_MS;
    (void)read_temp();                   //��ȡˮ�� 
    switch(tempt_crtl_state)
	{
	    case TEMPT_CLOSE :            	//��ˮ״̬�����ر��¶ȵ���
		{   	
			break;     
	    }
		case TEMPT_P_ADJUST:           //�¶���������   
		{
			if(pps_time_calculate() ==1)  break; //ppsʱ�����	
			if(adjust_cnt ++ >150)
			{
			   tempt_accuracy = LOW_ACCURACY;   //����ʱ�䳬��15s�����͵�������
			}
			tempt_adjust_process();			
			break;                  
	    }
		case TEMPT_N_ADJUST:           //�¶ȱ�������
		{
		    temp_stability_judge();
			break;
	    }
		case WAIT_FOR_NORMAL:          //�ȴ�ˮ·�ָ�����״̬
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
	// ����Ĭ��38��
	set_tempt = DEFAULT_TEMPT;
	poz_index = 23;                    //Ĭ�ϵ����38�ȵ�λ��
    clear_motor_pos(TEMP_STEP_MOTOR);  //���ò������ǰλ��Ϊ0
    tempt_accuracy =MID_ACCURACY;      //Ĭ�ϵ�������Ϊ1��
}
// ����Ŀ���¶�
void set_water_tempt(uint16 tempt)
{
	if(tempt > MAX_TEMPT)
		set_tempt = MAX_TEMPT;
	else if(tempt < MIN_TEMPT)
		set_tempt = MIN_TEMPT;
	else
		set_tempt = tempt; 
	set_water_leds (tempt, 460, 150);   //�ƹ�ָʾ�¶�
}

void set_tempt_cmd(uint8 cmd)
{ 

    //cmd =1,�¶����ӣ�
    //cmd =0 �¶Ƚ���
    uint16 data = 0;
    if(0x01 == cmd)
    {
       data = set_tempt +10;  //�˴���ťÿ��һ������0.5�ȣ��ֶȿɵ�
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
	set_water_leds (set_tempt, MAX_TEMPT,MIN_TEMPT);          //�ƹ�ָʾ�¶�
	tempt_crtl_state = TEMPT_P_ADJUST;                        //�е��£��������ģʽ

}
void tempt_crtl_process(uint8 func,uint16 value)
{ 
    switch(func)
	{
	    case WATER_OUT:            	      //��ˮ
		{   
			tempt_crtl_state = TEMPT_P_ADJUST;
			break;     
	    }
		case WATER_CLOSE:                 //��ˮ    
		{
			set_step_motor_stop(TEMP_STEP_MOTOR);   //�������ֹͣ����
            tempt_crtl_state = TEMPT_CLOSE;
			break;                  
	    }
		case TEMPT_ADJUST:               //�¶ȵ���
		{		
			set_water_tempt(value);     //�趨Ŀ���¶�  
			tempt_crtl_state = TEMPT_P_ADJUST;
			break;     
	    }
		case FLOW_ADJUST:              //��������
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
