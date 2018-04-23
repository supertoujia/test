#include "com_config.h"
#include "system.h"
#include "uart.h"
#include"temperature.h"
#include "water_level_sensor.h"
#include "drain_water.h"
#include "massage.h"
#include "lamp.h"
#include "keep_warm.h"


#define     BOOT_CODE           0x32
#define     SPARE1_1            0x3b
#define     SPARE1_2            0xb3
#define     SPARE2              0x01
#define     END_CODE            0X34
#define     DATA_LENGTH         16
#define     DEVICE_ADD          0X04
#define     BUSY                1
#define     FREE                0
#define     TAP_SWITCH_CMD      0X01
#define     WATER_FLOW_CMD      0X04
#define     WATER_TEMPT_CMD     0X02


//控制命令
#define     NONE          0X00         //无状态
#define     TAP_SWITCH    0X01         //水龙头出水通道切换(此时流量与温度对应发生变化)        
#define     TEMPT_POZ     0X02         // 水龙头出水温度档位状态     
#define     TEMPT_VAL     0X03         //水龙头出水温度位置状态 
#define     FLOW_VALUE    0X04         // 水龙头流量档位状态
#define     WATER_LEVEL   0X05         //液位状态    
#define     LED_CTRL      0X06         // LED状态状态  
#define     MASSAGE_CTRL  0X07         //按摩功能状态 
#define     KEEP_WARM     0X08         //浴缸保温温度  
#define     DRAIN_WATER   0X09         //排水阀状态 
#define     STATE_SEND    0X0A         //来自魔镜的的状态接收


#define     CTRL_CMD      rc1_data[3]

#define     TIME_100MS          10
#define     TIME_5S             50

//串口1相关数据
volatile static  unsigned char rc1_data_temp[64];
volatile static  unsigned char rc1_data[16];
volatile static  unsigned char tx1_data[16];

volatile static  bit  rc1_sign;

//串口2相关数据
volatile static  unsigned char rc2_data_temp[64];
volatile static  unsigned char rc2_data[16];
volatile static  unsigned char tx2_data[16];

volatile static  bit  rc2_sign;

//volatile uint16 com_ctrl_time = 0;              //串口控制时间

volatile static uint8 rec_from_mirror = 0;   //接收来自魔镜状态标志位
volatile static uint8 massage_reg = 0;       //按摩状态缓存




//以下函数临时增加，应对上海展魔镜控制
void read_message_ctrl(void)
{
     if(rec_from_mirror == 0) return ;

     massage_set(massage_reg,0);
     

}

/*****************************************************************************
 函 数 名  : usart1_rec_int
 功能描述  : USART1中断接收数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void rec_from_wifi_int(void)
{
       static unsigned char rc_index = 0;
       unsigned char data_temp;
       data_temp = RCREG;
	   rc1_data_temp[rc_index] = data_temp;
	   
       if(rc1_data_temp[rc_index] == SPARE1_1)                               //起始码校验
       {
		if( ( rc1_data_temp[rc_index-1] == BOOT_CODE )&& ( rc_index>0) )
		{
			rc_index = 1;
			rc1_data_temp[0] = BOOT_CODE; 
			rc1_data_temp[1] = 0x3b;
		}
	   }
		else if(( rc1_data_temp[rc_index] == END_CODE) &&
		       ( rc1_data_temp[0]  == BOOT_CODE) && ( rc_index == 15 ) )   //起始码、结束码校验
		{
		 	rc1_sign = 1;           //接收一帧数据成功
		 	rc_index = 0;
		}
	   rc_index ++;
	   if(rc_index>50)
	   {
		   rc_index = 0;
	   }

}




/*****************************************************************************
 函 数 名  : send_water_level
 功能描述  : 发送液位给水龙头
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年3月17日09:25:30
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void send_water_level(void)
{
//    unsigned char i;
	//for(i = 0;i<16;i++)
	//{
	//	usart2_tx_data[i] = usart1_rx_data[i];
	//}
	tx1_data[0] = 0x32;
	tx1_data[1] = 0x3b;
	tx1_data[2] = 0x01;
	tx1_data[3] = 0x05;
	tx1_data[15] = 0x34;
	if(read_water_level(1) == 0)                //高液位状态
	{
        tx1_data[6] &= ~0x80 ;      
	}
	else
	{
	    tx1_data[6] |= 0x80;
	}
	if(read_water_level(0) == 0)              //低液位状态
	{
        tx1_data[6] &= ~0x40 ;
	}
	else
	{
	    tx1_data[6] |= 0x40;
	}
	//tx1_data[6] | = read_massage_state();
	if((read_massage_state()) == 0x01)                    //按摩状态
	{
		tx1_data[6] |= 0x08;
	}
	else
	{
		tx1_data[6] &= ~0x08;
	}
	tx1_data[14] =  tx1_data[1]^tx1_data[2]^tx1_data[3]^tx1_data[4]
				          ^tx1_data[5]^tx1_data[6]^tx1_data[7]^tx1_data[8]
				          ^tx1_data[9]^tx1_data[10]^tx1_data[11]^tx1_data[12]
				          ^tx1_data[13];
	usart2_send_data((uint8 *)tx1_data,16);         //用UART2转发给温控盒	
}

void send_to_tempt_box(void)
{
    uint8 i= 0;
	for(i = 0;i<16;i++)
	{
        tx1_data[i] = rc1_data[i];
	}
	//tx1_data[0] = 0x32;	
    //tx1_data[1] = 0x3b;
    tx1_data[2] = 0x01;

	
	//如果不是控制出水命令，则返回
    if(tx1_data[3]!= 0x01)
		return ;


	
	
    if(read_water_level(1) == 0)                //高液位状态
	{
        tx1_data[6] &= ~0x80 ;      
	}
	else
	{
	    tx1_data[6] |= 0x80;
	}
	if(read_water_level(0) == 0)              //低液位状态
	{
        tx1_data[6] &= ~0x40 ;
	}
	else
	{
	    tx1_data[6] |= 0x40;
	}
	//tx1_data[6] | = read_massage_state();
	if((read_massage_state()) == 0x01)                    //按摩状态
	{
		tx1_data[6] |= 0x08;
	}
	else
	{
		tx1_data[6] &= ~0x08;
	}
	tx1_data[14] =  tx1_data[1]^tx1_data[2]^tx1_data[3]^tx1_data[4]
				          ^tx1_data[5]^tx1_data[6]^tx1_data[7]^tx1_data[8]
				          ^tx1_data[9]^tx1_data[10]^tx1_data[11]^tx1_data[12]
				          ^tx1_data[13];
	usart2_send_data((uint8 *)tx1_data,16); 

}
/*****************************************************************************
 函 数 名  : wifi_data_check
 功能描述  : USART数据校验
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/
bit wifi_data_check(void)
{     
	unsigned char i = 0;
	unsigned char check_temp;
	if(rc1_sign == 1)
	{
	        rc1_sign = 0;
		if(( rc1_data_temp[1] == SPARE1_1)
			&&( rc1_data_temp[2] == SPARE2))
		{
		 	for(i = 0;i < DATA_LENGTH ; i++)
	 		{
	 			rc1_data[i] = rc1_data_temp[i] ;
	 		}
			check_temp  = rc1_data[1]^rc1_data[2]^rc1_data[3]^rc1_data[4]
				          ^rc1_data[5]^rc1_data[6]^rc1_data[7]^rc1_data[8]
				          ^rc1_data[9]^rc1_data[10]^rc1_data[11]^rc1_data[12]
				          ^rc1_data[13];
		    if(check_temp == rc1_data[14] )
		    {
			//usart1_send_data(rc1_data,16);
		    	return 1;
			}
			else
			{
			    return 0;

			}
		}
		return 0;
	}
	return 0;

}
void wifi_cmd_process(void)
{

     switch (CTRL_CMD)
     {
       case LED_CTRL:                       //LED灯控制
	   {
	   	    lamp_ctrl(rc1_data[8]);
            read_message_ctrl();           //读取按摩状态控制
	   	    break;
	   } 
	   case MASSAGE_CTRL:                  //按摩控制
	   {
	   	    static  uint8 mode = 0,grad = 0;
            mode = rc1_data[9]&0x03;
            grad = (rc1_data[9]&0xf0)>>4;		
            massage_set(mode,0);          
	   	    break;
	   }       
	   case KEEP_WARM:                    //保温控制
	   {
	   	    set_keep_warm_tempt(rc1_data[10]);
            read_message_ctrl();           //读取按摩状态控制
	   	    break;
	   }
	   case DRAIN_WATER:                  //排水
	   {
	   	    drain_water_ctrl(DRAIN_WATER,rc1_data[11]); 
	   	    break;
	   }
       
       //以下沈家耀在2017年5月16日20:00:07增加
       //应对上海展魔镜       
       case STATE_SEND :
       {
             set_keep_warm_tempt(rc1_data[10]);
             rec_from_mirror = 1;                 //已经接收到魔镜数据
             massage_reg = rc1_data[9]&0x03;      //暂存按摩状态
	   	     break;
       }

     }

}
/*****************************************************************************
 函 数 名  : get_usart1_data
 功能描述  : 读取USART1接收的数据串
 输入参数  : unsigned char *usart1_dat接收数组首地址
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/
void get_usart1_data(unsigned char *usart1_dat)
{
    unsigned char i = 0;
 	for(i = 0;i < (DATA_LENGTH ) ; i++)
	{
		usart1_dat[i] = rc1_data[i] ;
	}
}


/*****************************************************************************
 函 数 名  : usart2_rec_int
 功能描述  : USART2中断接收数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/
void rec_temp_box_int(void)
{
       static unsigned char rc_index = 0;
       unsigned char data_temp;
       data_temp = RC2REG;
	   rc2_data_temp[rc_index] = data_temp;
	   
       if(rc2_data_temp[rc_index] == BOOT_CODE)                               //起始码校验
       {
        	rc_index = 0;
	        rc2_data_temp[rc_index] = BOOT_CODE; 
       }
	   else if(( rc2_data_temp[rc_index] == END_CODE) &&
		      (rc2_data_temp[0]  == BOOT_CODE) && ( rc_index == 15 ) )   //起始码、结束码校验
	   {
		 	rc2_sign = 1;           //接收一帧数据成功
		 	rc_index = 0;
	   }
	   rc_index ++;
	   if(rc_index>50)
	   {
		   rc_index = 0;
	   }

}
/*****************************************************************************
 函 数 名  : usart2_data_check
 功能描述  : USART2校验
 输入参数  : 无
 输出参数  : 校验结果，0或者1
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

bit temp_box_data_check(void)
{     
	unsigned char i = 0;
	unsigned char check_temp;
	if(rc2_sign == 1)
	{
	    rc2_sign = 0;
		if(( rc2_data_temp[1] == SPARE1_1)
			&&( rc2_data_temp[2] == SPARE2))
		{
		 	for(i = 0;i < DATA_LENGTH ; i++)
	 		{
	 			rc2_data[i] = rc2_data_temp[i] ;
	 		}
			check_temp  = rc2_data[1]^rc2_data[2]^rc2_data[3]^rc2_data[4]
				          ^rc2_data[5]^rc2_data[6]^rc2_data[7]^rc2_data[8]
				          ^rc2_data[9]^rc2_data[10]^rc2_data[11]^rc2_data[12]
				          ^rc2_data[13];
		    if(check_temp == rc2_data[14] )
		    {
		      // usart1_send_data(rc2_data,16);
		       return 1;
			}
			else
			{
			   return 0;
			}
		}
		return 0;
	}
	return 0;
}
void temp_box_cmd_process(void)
{

     switch (rc2_data[3])
     {
       case LED_CTRL:                       //LED灯控制
	   {
	   	    lamp_ctrl(rc2_data[8]);
	   	    break;
	   } 
	   case MASSAGE_CTRL:                  //按摩控制
	   {
	   	    static  uint8 mode = 0,grad = 0;
            mode = rc2_data[9]&0x03;
            grad = (rc2_data[9]&0xf0)>>4;		
            massage_set(mode,0);          
	   	    break;
	   }       
	   case KEEP_WARM:                    //保温控制
	   {
	   	    set_keep_warm_tempt(rc2_data[10]);
	   	    break;
	   }
	   case DRAIN_WATER:                  //排水
	   {
	   	    drain_water_ctrl(DRAIN_WATER,rc2_data[11]); 
	   	    break;
	   }

       //以下临时增加，应对上海展
       case TAP_SWITCH:
       {
           read_message_ctrl();           //读取按摩状态控制
       }
     }

}

/*****************************************************************************
 函 数 名  : get_usart2_data
 功能描述  : 读取USART2接收的数据
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void get_usart2_data(unsigned char *usart2_dat)
{
    unsigned char i = 0;
 	for(i = 0;i < (DATA_LENGTH ) ; i++)
	{
		*(usart2_dat++) = rc2_data[i] ;
	}
}
void host_com_in_main(void)
{
    static uint16 host_cnt = 0;

	if(temp_box_data_check()==1)    //旋钮只控制按摩
	{     
       // mode = rc2_data[9]&0x03;
        //grad = (rc2_data[9]&0xf0)>>4;		
        //massage_set(rc2_data[9]&0x03,0);    
        temp_box_cmd_process();
	}
	if(wifi_data_check()==1)
	{
        host_cnt = 0;
        wifi_cmd_process();                 //命令执行处理
		send_to_tempt_box();               //反馈信息给温控盒
	}
	else
	{
        if(host_cnt ++>TIME_5S)
        {
            host_cnt = 0;
		    send_water_level();               //反馈信息给温控盒
        }
	}	
} 














