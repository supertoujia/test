#include"adc.h"
#include "io_setting.h"

/****************************相关变量定义***********************************/

unsigned int ad_value[6];                	//AD转换数值暂存数组

uint16 tempt_ad[4];

/****************************相关函数定义***********************************/
//void adc_init(void);                   //ADC初始化
void adc_go_on(void);                    //启动ADC
void read_adc(void);                     //读取AD转换结果
unsigned int  read_adc_value(unsigned char channel );//查询方法读取AD转换结果
unsigned int adc_read(unsigned char channel);   //读取AD转换数值

/*****************************************************************************
 函 数 名  : adc_init
 功能描述  : ADC模块初始化
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
void adc_init(void)
{
    ANSELF = 0B11000000;              //使能AD引脚RF2/AN7、RF3/AN8、RF7/AN5
	TRISF  |= 0B11000000;              //将相应的AD引脚设置为输入
	//ANSA3 = 1;
	//TRISA3 = 1;
	ADFM   = 1;                       //右对齐
	ADCS2  = 1;ADCS1 = 1;ADCS0 = 0;   //转换时钟  Fsoc/8  001
	// FVRCON |= 0B10000011;              //使能ADC内部参考电压为4.096V
	
	ADPREF1 = 0; ADPREF0 = 0;         //选择正参考电压内部参考电压
	ADNREF  = 0;                      //选择负参考电压VSS
	ADCON0 = (AN_5<< 2) ;		      //选择相应通道，使能ADC, FOSC/2 osc.结果左对齐
	NOP();

	ADIF = 0;
	ADIE = 1;
	ADON = 1; 							 //开启AD模块	
	ADGO = 1;	
}
/*****************************************************************************
 函 数 名  : adc_go_on
 功能描述  : 启动ADC转换
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
void adc_go_on(void)
{
	ADIE = 1;
	ADON = 1; 
    ADGO = 1;                       
}

/*****************************************************************************
 函 数 名  :  read_adc
 功能描述  : 读取AD转换数值，存在数组里面
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
void read_adc(void)
{
    unsigned int adc_temp;
	static unsigned char ad_channel = 0;               //AD通道数
	adc_temp = (unsigned int)(ADRESH<<8)+ADRESL;		
	switch(ad_channel)
	{
	    case TEMP_MID:             //温度传感器1
		{
           
            tempt_ad[3]= tempt_ad[2];
            tempt_ad[2]= tempt_ad[1];
            tempt_ad[1]= tempt_ad[0];
            tempt_ad[0]= adc_temp;
			ad_value[TEMP_MID] = (tempt_ad[0]+tempt_ad[0]+tempt_ad[0]+tempt_ad[0])/4;
			ADCON0 = (AN_11<<2);
			ad_channel = TEMP_IN;		
			break;
    	}
		case TEMP_IN:            //温度传感器2
		{
			ad_value[TEMP_IN] = adc_temp;
			ADCON0 = (AN_5<<2);
			ad_channel = TEMP_MID;
			break;
    	}
		case VREF_ADC:            
		{
			ad_value[VREF_ADC] = adc_temp;
			ADCON0 = (AN_5<<2);    //输入电压检测
			ad_channel = TEMP_MID;
			break;
    	}
		default:
		{
			ADCON0 = (AN_5<<2);
			ad_channel = TEMP_MID;
			ADIE = 0;
			break;
    	}
	}   
}

/*****************************************************************************
 函 数 名  :  read_adc_value
 功能描述  : 读取相应通道AD值
 输入参数  : channel ADC通道数
 输出参数  :  ad_value[channel]通道的AD数值
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/
unsigned int  read_adc_value(unsigned char channel )
{
     return ad_value[channel];
}


/*****************************************************************************
 函 数 名  : adc_read
 功能描述  : 查询的方法读取相应通道AD值
 输入参数  : channel ADC通道数
 输出参数  : ad_value通道的AD数值
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

unsigned int adc_read(unsigned char channel)
{
	long int ad_value = 0;
    unsigned char i,cnt;
	ANSELF = 0B00001100;              //使能AD引脚RF2/AN7、RF3/AN8
    TRISF  = 0B00001100;              //将相应的AD引脚设置为输入
    ANSELA = 0B00001000;              //使能AD引脚RA3/AN3
    TRISA3 = 1;
	ADCON0 = (channel << 2) + 0x01;		//选择相应通道，使能ADC, FOSC/2 osc.结果左对齐
	NOP();
	NOP();
	for(i=0;i<16;i++)
	{
	    ADGO = 1;
        while(ADGO)
        {
            cnt++	    ;		//等待转换完成
            if(cnt>=80)
                break;
        }
        ad_value =ad_value+ ADRESH*4 + ADRESL/64;
	}
	ad_value=ad_value/16;
	return (ad_value);
}

