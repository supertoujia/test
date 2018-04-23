#include "lamp.h"

#define RED_LAM       LATD4                 //RD4控制红灯
#define GREEN_LAM     LATD6                 //RD2控制绿灯
#define BLUE_LAM      LATD5                 //RD3控制蓝灯


/****************************相关变量定义***********************************/

volatile bit lamp_on_sign = 0;              //开灯标志
volatile bit lamp_switch_sign = 0;          
volatile static unsigned char red_pwm = 0;          //红灯脉宽
volatile static unsigned char green_pwm = 0;        //绿灯脉宽
volatile static unsigned char blue_pwm = 0;         //蓝灯脉宽

volatile static unsigned char set_red_pwm = 0;          //红灯脉宽
volatile static unsigned char set_green_pwm = 0;        //绿灯脉宽
volatile static unsigned char set_blue_pwm = 0;         //蓝灯脉宽
volatile static uint16 lamp_pwm_cnt = 0;
volatile static  int rgb[3] = {0,0,0};        //数组三个元素分别为RGB
volatile static  int set_rgb[3] = {0,0,0};

volatile static unsigned char color_state = 0;      //灯的颜色状态
volatile bit color_change_sign = 0;                 //七彩灯色变标志位


/****************************相关函数定义***********************************/
void lamp_init(void);                      //灯初始化
void lamp_on(void);                        //开灯
void lamp_off(void);                       //关灯
void lamp_on_process(void);                //灯pwm控制
void lamp_color_switch(void);              //灯颜色变换
void color_switch_on(void);                //开启灯循环闪烁
void color_switch_off(void);               //关闭灯循环闪烁
void lamp_pwm_on(void);                    //开启灯的PWM输出
void lamp_1ms_process(void);               //灯1ms处理 
void lamp_ctrl(unsigned char lamp_color);  //灯的控制


void lamp_init(void)
{
	TRISD4 = 0;
	TRISD6 = 0;
	TRISD5 = 0;
	RED_LAM = OFF;
	GREEN_LAM = OFF;                        //上电初始化，绿灯亮
	BLUE_LAM = OFF;
	
//以下是TIMER0初始化
	
    TMR0CS = 0;					//内部时钟
	TMR0SE = 0;
	PSA = 0;					//预分??
	PS2 = 0;
	PS1 = 0;
	PS0 = 0;
	TMR0 = 156;          //定时器赋初值
	TMR0IE = 1;                 //允许timer0中断
	
}
void lamp_on(void)
{
   lamp_on_sign = 1;	
}
void lamp_off(void)
{
   lamp_on_sign = 0;
   RED_LAM = 0;
   GREEN_LAM = 0;
   BLUE_LAM = 0;
   color_switch_off();      //关闭循环	
}
void lamp_pwm_on(void)
{	
    lamp_pwm_cnt++;
	if(lamp_pwm_cnt>=255)
	lamp_pwm_cnt = 0;
}
void lamp_on_process(void)
{
     if(lamp_pwm_cnt >=rgb[0])
 	 {
 	    RED_LAM = 0;
 	 }
	 else
 	 {
        RED_LAM = 1;
 	 } 
	 if(lamp_pwm_cnt >=rgb[1])
 	 {
 	    GREEN_LAM = 0;
 	 }
	 else
 	 {
        GREEN_LAM = 1;
 	 } 
	 if(lamp_pwm_cnt >=rgb[2])
 	 {
 	    BLUE_LAM = 0;
 	 }
	 else
 	 {
        BLUE_LAM = 1;
 	 } 
}
void lamp_color_change(void)
{
    if(lamp_switch_sign == ON)
	{

	   color_change_sign = 1;    //需要改变颜色
       color_state ++;
	   if(color_state > 7)
	   color_state = 1;
      // lamp_ctrl( color_state );
	}
}
void lamp_color_switch(void)
{
    switch(color_state)
	{
	    case 1:                      //红色
		{   
			set_rgb[0] = 255;
			set_rgb[1] = 0;
			set_rgb[2] = 0;
			break;
    	}
		case 2:                      //绿色
		{
			set_rgb[0] = 0;
			set_rgb[1] = 255;
			set_rgb[2] = 0;
			break;
    	} 
		case 3:                     //蓝色
		{
			set_rgb[0] = 0;
			set_rgb[1] = 0;
			set_rgb[2] = 255;
			break;
    	}
		case 4:                    //黄色
		{
			set_rgb[0] = 255;
			set_rgb[1] = 255;
			set_rgb[2] = 0;
			break;
    	}
		case 5:                   //青色
		{
			set_rgb[0] = 0;
			set_rgb[1] = 255;
			set_rgb[2] = 255;
			break;
		
    	} 
		case 6:                  //品红  
		{
			set_rgb[0] = 255;
			set_rgb[1] = 0;
			set_rgb[2] = 255;
			break;
			
    	}
		case 7:                   //白色
		{
			set_rgb[0] = 255;
			set_rgb[1] = 255;
			set_rgb[2] = 255;
			break;
		
    	}
		
		default:
		{
			set_rgb[0] = 0;
			set_rgb[1] = 0;
			set_rgb[2] = 0;
			break;
    	}

	}
}
void color_switch_on(void)
{
   lamp_on();
   lamp_switch_sign = ON;
}

void color_switch_off(void)
{
   lamp_switch_sign = OFF;
}
void lamp_ctrl(unsigned char lamp_state)
{
    switch (lamp_state)
    {
        case 0:
		{
			color_switch_off();
			RED_LAM = OFF;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = OFF;
            color_state = 0;
			color_change_sign = 1;
	        break;
        }
		case 1:
		{
			color_switch_off();
			RED_LAM = ON;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = OFF;
			color_state = 1;
			color_change_sign = 1;
	        break;
        }
		case 2:
		{   
			color_switch_off();
			RED_LAM = OFF;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = OFF;
			color_state = 2;
			color_change_sign = 1;
	        break;
        }
		case 3:
		{
			color_switch_off();
			RED_LAM = OFF;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = ON;
			color_state = 3;
			color_change_sign = 1;
	        break;
        }
		case 4:
		{
			color_switch_off();
			RED_LAM = ON;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = OFF;
			color_state = 4;
			color_change_sign = 1;
	        break;
        }
		case 5:
		{
			color_switch_off();
			RED_LAM = OFF;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = ON;
			color_state = 5;
			color_change_sign = 1;
	        break;
        }
		case 6:
		{
			color_switch_off();
			RED_LAM = ON;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = ON;
			color_state = 6;
			color_change_sign = 1;
	        break;
        }
		case 7:
		{
			color_switch_off();
			RED_LAM = ON;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = ON;
			color_state = 7;
			color_change_sign = 1;
	        break;
        }
		case 8:
		{
			color_switch_on();
	        break;
        }
		default :
		{
			color_switch_off();
            RED_LAM = OFF;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = OFF;
			break;
		}
    } 
}

void lamp_color_process(void)
{

if(color_change_sign == 0)
	return ;

    switch (color_state)
    {
        case 0:
		{
    		RED_LAM = OFF;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = OFF;
	        break;
        }
		case 1:
		{
			RED_LAM = ON;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = OFF;
	        break;
        }
		case 2:
		{   
			RED_LAM = OFF;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = OFF;
	        break;
        }
		case 3:
		{
			RED_LAM = OFF;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = ON;
	        break;
        }
		case 4:
		{
			RED_LAM = ON;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = OFF;
	        break;
        }
		case 5:
		{
			RED_LAM = OFF;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = ON;
	        break;
        }
		case 6:
		{
			RED_LAM = ON;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = ON;
	        break;
        }
		case 7:
		{
			RED_LAM = ON;
	        GREEN_LAM = ON;                       
	        BLUE_LAM = ON;
	        break;
        }
		default :
		{
            RED_LAM = OFF;
	        GREEN_LAM = OFF;                       
	        BLUE_LAM = OFF;
			break;
		}
    } 

	color_change_sign = 0;
}

/*void lamp_ctrl(unsigned char lamp_state)
{
   if((lamp_state>0) && (lamp_state<8))
   {
       color_state = lamp_state;
	   lamp_on();
	  // lamp_color_switch();
	   color_switch_off();
   }
   else if(lamp_state == 8)
   {
       color_state = lamp_state;
	   color_switch_on();
   }
   else
   {
        lamp_off();
	    color_switch_off();
   }
   
   lamp_color_switch();
}*/

/*void lamp_1ms_process(void)
{
    if(lamp_on_sign == ON)
	{   
	    lamp_pwm_on();
        lamp_on_process();
	}
	else
	{
        lamp_off();
	}
}*/
void lamp_out_process(void)
{
	if(lamp_switch_sign == ON)
	{	
		lamp_pwm_on();
		lamp_on_process();
	}
	/*else
	{
		lamp_off();
	}*/
}

void lamp_process(void)
{
    static  unsigned int change_time;
//	unsigned char i = 0;	

	//if(lamp_switch_sign == OFF )    //如果没有进行循环，返回
	//	return;

	lamp_color_process();

	if((change_time++)%400==0)
	{
      
        lamp_color_change();    
	}	
	//if(((change_time++)%1)==0)
	/*{
	    for(i=0;i<3;i++)
	    {
	       if(rgb[i]<set_rgb[i])
	       {
	          rgb[i] = rgb[i] + 5;
			  if(rgb[i]>=set_rgb[i])
			      rgb[i] = set_rgb[i]; 
	       }
		   else if(rgb[i]>set_rgb[i])
	       {
	          rgb[i] = rgb[i] - 5;
			  if(rgb[i]<=set_rgb[i])
			      rgb[i] = set_rgb[i]; 
	       }
	    }
	}*/

}


