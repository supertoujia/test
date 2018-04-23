#include "lamp.h"

#define RED_LAM       LATD4                 //RD4���ƺ��
#define GREEN_LAM     LATD6                 //RD2�����̵�
#define BLUE_LAM      LATD5                 //RD3��������


/****************************��ر�������***********************************/

volatile bit lamp_on_sign = 0;              //���Ʊ�־
volatile bit lamp_switch_sign = 0;          
volatile static unsigned char red_pwm = 0;          //�������
volatile static unsigned char green_pwm = 0;        //�̵�����
volatile static unsigned char blue_pwm = 0;         //��������

volatile static unsigned char set_red_pwm = 0;          //�������
volatile static unsigned char set_green_pwm = 0;        //�̵�����
volatile static unsigned char set_blue_pwm = 0;         //��������
volatile static uint16 lamp_pwm_cnt = 0;
volatile static  int rgb[3] = {0,0,0};        //��������Ԫ�طֱ�ΪRGB
volatile static  int set_rgb[3] = {0,0,0};

volatile static unsigned char color_state = 0;      //�Ƶ���ɫ״̬
volatile bit color_change_sign = 0;                 //�߲ʵ�ɫ���־λ


/****************************��غ�������***********************************/
void lamp_init(void);                      //�Ƴ�ʼ��
void lamp_on(void);                        //����
void lamp_off(void);                       //�ص�
void lamp_on_process(void);                //��pwm����
void lamp_color_switch(void);              //����ɫ�任
void color_switch_on(void);                //������ѭ����˸
void color_switch_off(void);               //�رյ�ѭ����˸
void lamp_pwm_on(void);                    //�����Ƶ�PWM���
void lamp_1ms_process(void);               //��1ms���� 
void lamp_ctrl(unsigned char lamp_color);  //�ƵĿ���


void lamp_init(void)
{
	TRISD4 = 0;
	TRISD6 = 0;
	TRISD5 = 0;
	RED_LAM = OFF;
	GREEN_LAM = OFF;                        //�ϵ��ʼ�����̵���
	BLUE_LAM = OFF;
	
//������TIMER0��ʼ��
	
    TMR0CS = 0;					//�ڲ�ʱ��
	TMR0SE = 0;
	PSA = 0;					//Ԥ��??
	PS2 = 0;
	PS1 = 0;
	PS0 = 0;
	TMR0 = 156;          //��ʱ������ֵ
	TMR0IE = 1;                 //����timer0�ж�
	
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
   color_switch_off();      //�ر�ѭ��	
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

	   color_change_sign = 1;    //��Ҫ�ı���ɫ
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
	    case 1:                      //��ɫ
		{   
			set_rgb[0] = 255;
			set_rgb[1] = 0;
			set_rgb[2] = 0;
			break;
    	}
		case 2:                      //��ɫ
		{
			set_rgb[0] = 0;
			set_rgb[1] = 255;
			set_rgb[2] = 0;
			break;
    	} 
		case 3:                     //��ɫ
		{
			set_rgb[0] = 0;
			set_rgb[1] = 0;
			set_rgb[2] = 255;
			break;
    	}
		case 4:                    //��ɫ
		{
			set_rgb[0] = 255;
			set_rgb[1] = 255;
			set_rgb[2] = 0;
			break;
    	}
		case 5:                   //��ɫ
		{
			set_rgb[0] = 0;
			set_rgb[1] = 255;
			set_rgb[2] = 255;
			break;
		
    	} 
		case 6:                  //Ʒ��  
		{
			set_rgb[0] = 255;
			set_rgb[1] = 0;
			set_rgb[2] = 255;
			break;
			
    	}
		case 7:                   //��ɫ
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

	//if(lamp_switch_sign == OFF )    //���û�н���ѭ��������
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


