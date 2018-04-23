#include "com_config.h"
#include "system.h"
#include "traic.h"
#include "water_level_sensor.h"
#include "massage.h"



/*****************��Ħ״̬**********************/
#define  STOP                    0     //ֹͣ
#define  ALL_SPEED               1     //ȫ��
#define  HALF_SPEED              2     //����
#define  TRIANGEL                3     //���ǲ�
#define  SQUARE                  4     //����


#define  TIME_2S                  20


void zero_corss_init(void);
static void air_pump_ctrl(void);
static void water_pump_ctrl(void);
void massage_init(void);
void massage_set(unsigned char work_mode,unsigned char grade);
void massage_process(void);
void massage_power_process(void);
static unsigned long int power_rate_set(unsigned char poz);


//static unsigned long int power_rate = 0;            //����
volatile static unsigned char air_pump_state = 0;            //������״̬
volatile static unsigned char water_pump_state = 0;          //ˮ��״̬
volatile static unsigned char water_power_rate = 0;          //ˮ�ù���
volatile static unsigned char air_power_rate= 0;             //�����ù���
volatile static bit air_pump_work_sign = 0;                  //�����ù���״̬
volatile static bit water_pump_work_sign = 0;                //ˮ�ñù���״̬
volatile static bit air_out_sign = 0;                        //����������ź�
volatile static bit water_out_sign = 0;                      //ˮ������ź�
volatile static bit zero_sign = 0; 

volatile static uint16 massage_ctrl_time = 0;                //��Ħ����ʱ��
volatile static uint8  massage_state = 0;                    

/*typedef struct
{
  unsigned :1   work_sign;
  unsigned :1   work_sign;
  uint8        pump_state;
  uint8        power_rate;

} */


/*****************************************************************************
 �� �� ��  : zero_corss_init
 ��������  : �������ʼ��
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :  
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��12��18:33:36
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void zero_corss_init(void)
{
	TRISB0 = 1;              //����B0λ����
	IOCBP0 = 0; IOCBN0 = 1;  //���������ؼ��
	IOCBF0 = 0;              //����жϱ�־λ
	IOCIE = 1;             
	PEIE = 1;
}
/*****************************************************************************
 �� �� ��  : massage_set
 ��������  : ��Ħ�趨,���ݲ����й��ʿ��ƣ������й��ʿ���
 �������  : ��Ħģʽmode����Ħǿ��grade
 �������  : ��
 �� �� ֵ  :
 ���ú���  :  
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��12��18:33:45
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void massage_set(unsigned char work_mode,unsigned char grade)
{
    if((read_water_level(0)==OFF)
       &&(water_pump_state == STOP)
	   &&(air_pump_state == STOP)) 
        return;


#if (MASSAGE_CTRL_MODE == POWER_CTRL_NONE)	

    switch(work_mode)
    {
        case 0:              //ֹͣȫ����Ħ
        {     
           traic_ctrl(AIR_PUMP,OFF);
           traic_ctrl(WATER_PUMP,OFF);
           water_pump_state = OFF;
           air_pump_state = OFF;
           break;
        }
        case 1:              //��
        {
           traic_ctrl(WATER_PUMP,OFF);                
           traic_ctrl(AIR_PUMP,ON);  
           water_pump_state = OFF;
           air_pump_state = ON;
           break;
        }
        case 2:              //ˮ
        {           
            traic_ctrl(AIR_PUMP,OFF);
            traic_ctrl(WATER_PUMP,ON);
            water_pump_state = ON;
            air_pump_state = OFF;
            break;
        }
        case 3:             //HUNHE
        {
            traic_ctrl(AIR_PUMP,ON);
            traic_ctrl(WATER_PUMP,ON);     //�ײ�ˮ��
            water_pump_state = ON;
            air_pump_state = ON;
            break;
        }
        default:
        {
             traic_ctrl(AIR_PUMP,OFF);
             traic_ctrl(WATER_PUMP,OFF);
             water_pump_state = OFF;
             air_pump_state = OFF;
             break;
        }
    }

#else

    static unsigned char massage_power = 0 ;
    if(grade == 0 )
       massage_power = 7 ;    //����%25
    else if(grade == 1 )
       massage_power = 15 ;    //����%50
    else if(grade == 2 )
       massage_power = 23 ;    //����%75
    else if(grade == 3 )
       massage_power = 31 ;    //����%100

    switch(work_mode)
	{
		case 0:              //ֹͣȫ����Ħ
		{
		
           traic_ctrl(AIR_PUMP,OFF);
           air_power_rate = 0;
           
#if(MASSAGE_CTRL_MODE == POWER_CTRL_FOR_AIR)  
           traic_ctrl(WATER_PUMP,OFF);
#else

//�˴����ӱ�Ƶ������ؿ���

#endif
		   water_pump_state = OFF;	
           air_pump_state = OFF;	

		   break;
		}
		case 1:              //��
		{
          traic_ctrl(AIR_PUMP,ON);
          air_power_rate = massage_power;
#if(MASSAGE_CTRL_MODE == POWER_CTRL_FOR_AIR)  

         traic_ctrl(WATER_PUMP,OFF);
         	
#else
//�˴����ӱ�Ƶ������ؿ���

#endif
           water_pump_state = OFF;	
           air_pump_state = ON;    
		   break;
		}
		case 2:              //ˮ
		{
           traic_ctrl(AIR_PUMP,OFF);
           air_power_rate = 0;
#if(MASSAGE_CTRL_MODE == POWER_CTRL_FOR_AIR)          
            traic_ctrl(WATER_PUMP,ON);
#else
//�˴����ӱ�Ƶ������ؿ���

#endif
           water_pump_state = ON;	
           air_pump_state = OFF;      
		   break;
		}
		case 3:             //HUNHE
		{
           traic_ctrl(AIR_PUMP,ON);
           air_power_rate = massage_power;   
#if(MASSAGE_CTRL_MODE == POWER_CTRL_FOR_AIR)         
           traic_ctrl(WATER_PUMP,ON);
                    
#else
            //�˴����ӱ�Ƶ������ؿ���
            
#endif
           water_pump_state = ON;	
           air_pump_state = ON;   
           break;

		}
		default:
		{
           traic_ctrl(AIR_PUMP,OFF);
           air_power_rate = 0;
                       
#if(MASSAGE_CTRL_MODE == POWER_CTRL_FOR_AIR)  
           traic_ctrl(WATER_PUMP,OFF);
#else
            
            //�˴����ӱ�Ƶ������ؿ���
            
#endif
           water_pump_state = OFF;	
           air_pump_state = OFF;               
           break;

		}
    }  
#endif
}
/*****************************************************************************
 �� �� ��  : low_water_protect
 ��������  : ��Ħ�趨
 �������  : ��Һλ��������ˮλ���ڵ�ˮλ������λ������
             �趨�رհ�Ħ����ֹˮ�����ҽ�
 �������  : ��
 �� �� ֵ  :
 ���ú���  :  
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��12��19:43:18
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

static low_water_protect(void)
{    
     static uint16 pro_cnt = 0;
	 if(read_water_level(0)==OFF)
	 {
        pro_cnt ++;
		if(pro_cnt >TIME_2S)
		{
           pro_cnt = 0;
		   massage_set(0,0);
		}
	 }
	 else
	 {
         pro_cnt = 0;
	 }
}



//�����ǽ��й��ʿ��Ƶ���غ�����ֻ�ж����˹��ʿ��Ʋű���

//#if( MASSAGE_CTRL_MODE == POWER_CTRL_FOR_AIR) 

#if MASSAGE_CTRL_MODE > 0	


/*static void air_pump_ctrl(void)
{
    static unsigned char ctrl_cnt = 0;
	static bit ctrl_sign = 0;
	switch(air_pump_state)
	{
		case STOP:
		{
			//AIR_PUMP   = OFF; 
			traic_ctrl(AIR_PUMP,OFF);
			
			air_power_rate = 0;  
			ctrl_cnt = 0;
        	break;
		}
		case ALL_SPEED:
		{   
			air_power_rate = 31; 
			ctrl_cnt = 0;
        	break;
		}
		case HALF_SPEED:
		{
			air_power_rate = 1; 
			ctrl_cnt = 0;
        	break;
		}
		case SQUARE:
		{   
			ctrl_cnt++;
			if(ctrl_cnt == 30)
			{
			    air_power_rate = 1; 
			}
			else if(ctrl_cnt >=60)
			{
				 air_power_rate = 16; 
				 ctrl_cnt = 0;
			}
        	break;
		}
		case TRIANGEL:
		{
			ctrl_cnt ++;
			if(ctrl_cnt >= 5)
			{
				ctrl_cnt = 0;
				if(ctrl_sign == 0)
				{
				    air_power_rate ++;
					if(air_power_rate >=16)
					{	
						ctrl_sign = 1;	
					}
				}
				else
				{
					air_power_rate --;
					if(air_power_rate <=1)
					{	
						ctrl_sign = 0;	
					}
				}
			}
        	break;
		}
		default:
		{
			//AIR_PUMP   = OFF;  
            traic_ctrl(AIR_PUMP,OFF);
			air_power_rate = 0; 
        	break;
		}
	}
}




static void water_pump_ctrl(void)
{

}*/





/*void water_pump_ctrl(void)
{
    static unsigned char ctrl_cnt = 0;
	static bit ctrl_sign = 0;
	switch(water_pump_state)
	{
		case STOP:
		{
			//WATER_PUMP   = OFF; 
            traic_ctrl(WATER_PUMP,OFF); 
			water_power_rate = 0;     
			ctrl_cnt = 0;
        	break;
		}
		case ALL_SPEED:
		{   
			water_power_rate = 31; 
			ctrl_cnt = 0;
        	break;
		}
		case HALF_SPEED:
		{
			water_power_rate = 1; 
			ctrl_cnt = 0;
        	break;
		}
		case SQUARE:
		{   
			ctrl_cnt++;
			if(ctrl_cnt == 30)
			{
			    water_power_rate = 1; 
			}
			else if(ctrl_cnt >=60)
			{
				water_power_rate = 16; 
				 ctrl_cnt = 0;
			}
        	break;
		}
		case TRIANGEL:
		{
			ctrl_cnt ++;
			if(ctrl_cnt >= 5)
			{
				ctrl_cnt = 0;
				if(ctrl_sign == 0)
				{
				    water_power_rate ++;
					if(water_power_rate >=16)
					{	
						ctrl_sign = 1;	
					}
				}
				else
				{
					water_power_rate --;
					if(water_power_rate <=1)
					{	
						ctrl_sign = 0;	
					}
				}
			}
        	break;
		}
		default:
		{
			//WATER_PUMP   = OFF; 
		
            traic_ctrl(WATER_PUMP,OFF); 
			water_power_rate = 0; 
        	break;
		}
	}
}
*/
static unsigned long int power_rate_set(unsigned char poz)
{
    unsigned long int power_rate = 0 ;
    switch(poz)
	{
		case 0:
		{
			power_rate = 0x00000000;
			break;
		}
		case 1:
		{
			power_rate = 0x00000001;
			break;
		}
		case 2:
		{
			power_rate = 0x10000001;
			break;
		}
		case 3:
		{
			power_rate = 0x10010001;
			break;
		}
		case 4:
		{
			power_rate = 0x10011001;
			break;
		}
		case 5:
		{
			power_rate = 0x10111001;
			break;
		}
		case 6:
		{
			power_rate = 0x10111101;
			break;
		}
		case 7:
		{
			power_rate =0x11111101;
			break;
		}
		case 8:
		{
			power_rate = 0x11111111;
			break;
		}
		case 9:
		{
			power_rate =0x11111115;
			break;
		}
		case 10:
		{
			power_rate = 0x51111115;
			break;
		}
		case 11:
		{
			power_rate = 0x51151115;
			break;
		}
		case 12:
		{
			power_rate = 0x51155115;
			break;
		}

		case 13:
		{
			power_rate =  0x51555115;
			break;
		}

		case 14:
		{
			power_rate =  0x51555515;
			break;
		}
		case 15:
		{
			power_rate = 0x55555515;
			break;
		}
		case 16:
		{
			power_rate = 0x55555555;
			break;
		}
		case 17:
		{
			power_rate = 0x55555557;;
			break;
		}
		case 18:
		{
			power_rate = 0x5555555F;
			break;
		}
		case 19:
		{
			power_rate = 0x5555557F;
			break;
		}
		case 20:
		{
			power_rate = 0x555557FF;
			break;
		}
		case 21:
		{
			power_rate = 0x55555FFF;
			break;
		}
		case 22:
		{
			power_rate = 0x55557FFF;
			break;
		}
		case 23:
		{
			power_rate = 0x5555FFFF;
			break;
		}
		case 24:
		{
			power_rate = 0x5557FFFF;
			break;
		}
		case 25:
		{
			power_rate = 0x555FFFFF;
			break;
		}
		case 26:
		{
			power_rate = 0x557FFFFF;
			break;
		}
		case 27:
		{
			power_rate = 0x55FFFFFF;
			break;
		}
		case 28:
		{
			power_rate =  0x57FFFFFF;
			break;
		}

		case 29:
		{
			power_rate =  0x5FFFFFFF;
			break;
		}

		case 30:
		{
			power_rate = 0x7FFFFFFF;
			break;
		}

		case 31:
		{
			power_rate = 0xFFFFFFFF;
			break;
		}
		
		default:
		{
			power_rate = 0x00000000;
			break;
		}
	}
	return power_rate;
}


 void massage_power_process(void)
{
	static unsigned char z_cnt = 0;
	static unsigned long int air_power = 0;
	static unsigned long int water_power = 0;
	if(zero_sign == 1)
	{
	   zero_sign = 0;
	   if(air_power & 0x01)
	   {
	       air_out_sign = 1;
	   }
	   else
	   {
	       air_out_sign = 0;
	   }
	   /*if(water_power & 0x01)
	   {
	       water_out_sign = 1;
	   }
	   else
	   {
	       water_out_sign = 0;
	   }*/
	   air_power = air_power >>1;
	  // water_power = water_power >>1;
	   if(++ z_cnt >=32)
	   {
	       z_cnt = 0;
		   air_power = power_rate_set(air_power_rate);
		//   water_power = power_rate_set(water_power_rate);
	   }

       
       //�˴������ȹر�Ƶ������


       
	}
}
void massage_ctrl(void)
{
    zero_sign = 1;
	if(air_out_sign ==1)
	{   
	   traic_ctrl(AIR_PUMP,ON);
	}
	else
	{ 
	   traic_ctrl(AIR_PUMP,OFF);
	}
	/*if(water_out_sign ==1)
	{
	  traic_ctrl(WATER_PUMP,ON);
	}
	else
	{
	   traic_ctrl(WATER_PUMP,OFF);
	}*/
}

#endif



uint8 read_massage_state (void)
{
    uint8 m_state = 0;
	if((water_pump_state!=STOP)
		||(air_pump_state!=STOP))
	   m_state = 0x01;
    else
       m_state = 0x00;
	return m_state;
}
void massage_ctrl_in_mian(void)
{ 
  
    //low_water_protect(); 
}



