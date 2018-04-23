

#include "com_config.h"
#include "fan_heat.h"
#include "temperature.h"

#define FAN_IO         LATD4
#define HEAT_IO        LATE7
volatile uint8 out_flag = 0;

typedef struct 
{
	int16 set_tempt;		//�趨�¶�
	int16 out_tempt;        //����¶�
	float Kp;		        //����ϵ��
	float Ki;               //����ϵ��
	float Kd;		        //΢��ϵ��
	int16 error;			//ƫ��
	int16 last_error;	    //��һ�ε�ƫ��
	int16 pre_error;       
    int16 out_pwm ;         //���PWM
}PID_PARAM;		//����һ��PID����

PID_PARAM pid;






void set_point(uint8 tempt)
{
    if(tempt == 0x00)
    {
       out_flag = 0;
       HEAT_IO = 0;
       FAN_IO = 0;
    }
    else
    {  
       FAN_IO = 1;
       out_flag = 1;  
       if(tempt <15) pid.set_tempt = 150;
       else if(tempt >60)pid.set_tempt = 600;
       else pid.set_tempt = tempt*10;
    }

}
void pid_init(void)
{   
    pid.set_tempt = 38;
    pid.error = 0;
    pid.last_error = 0;
    pid.pre_error = 0;
    pid.Kp = 2;
    pid.Ki= 0.03;
    pid.Kd = 0;
}


void pid_calc(void)
{
    pid.out_tempt = calculate_temperature(29,0);
    //��ǰ���
    pid.error= pid.set_tempt- pid.out_tempt;
    //��������
    pid.out_pwm =(int16) (pid.Kp* pid.error            //E[k]��
              - pid.Ki *pid.last_error                 //E[k��1]��
              + pid.Kd*pid.pre_error);                 //E[k��2]��
    //�洢�������´μ���
    pid.pre_error = pid.last_error;
    pid.last_error = pid.error;


    if(pid.out_pwm >63) pid.out_pwm = 63;
    if(pid.out_pwm <0) pid.out_pwm = 0;

}
const unsigned char output_power[]= {0b00000000,0b00010000,0b00010001,0b01010001,0b01010101,0b01110101,0b01110111,0b11110111,0b11111111};

void fan_heat_process(void)
{
    unsigned char output_code,output_index;
    static unsigned char output_time;
	//static uint16 pwm_cnt = 0;
    pid_calc();
	if(pid.out_tempt >700)
	{
		FAN_IO = 0;
        HEAT_IO = 0;
		out_flag = 0;  
//        pwm_cnt = 0;
		return;
	}
    if(out_flag == 0x00)return; 

    if(pid.out_pwm==0)
    {
        HEAT_IO = 0;
        return;
    }
    output_index=pid.out_pwm/8;
    if(pid.out_pwm %8>(output_time/8))output_index+=1;
    if(output_index>8)output_index=8;
    output_code=output_power[output_index];
    if((output_code>>(output_time%8))&0x01)
    {
         HEAT_IO = 1;;
    }
    else
    {
         HEAT_IO = 0;
    }
    output_time++;
    if(output_time>=63)
    {
        HEAT_IO = 0;
        output_time=0;
    }
   /* pwm_cnt ++;
    if(pwm_cnt <pid.out_pwm)
    {
        HEAT_IO = 1;
    }
    else
    {
        HEAT_IO = 0;
    }
    if(pwm_cnt>63)
    {
       pwm_cnt = 0;
       HEAT_IO = 0;*
    }*/
}








































