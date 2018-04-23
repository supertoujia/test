#include "com_config.h"


#define 	PIC_CLK 		    16000000//4000000                    // 时钟频率
#define     BAUD                9600                                 //波特率
#define     SPBRGx_VAL          ((PIC_CLK/(16UL * BAUD) -1))  
#define     BOOT_CODE           0x32
#define     SPARE1_1            0x3b
#define     SPARE1_2            0xb3
#define     SPARE2              0x01
#define     END_CODE            0X34
#define     DATA_LENGTH         16
#define     DEVICE_ADD          0XAA
#define     BUSY                1
#define     FREE                0
#define     RS485_RX()          {TRISC0 = 0; RC0 = 0;}
#define     RS485_TX()          {TRISC0 = 0; RC0 = 1;}


//=================================================================
//===========================USART1=================================
//=================================================================
void usart1_init(uint16 baud);
void usart1_send_byte(char ch);
void usart1_send_data(unsigned	char	*send_data,unsigned	char data_length);
void usart1_send_interrupt(void);



static	unsigned	char	usart1_tx_data[64];			//发射的数据数组
static	unsigned	char	usart1_tx_data_length;		//发射数据的长度
volatile static  bit  tx1_sign;

//=================================================================
//===========================USART2=================================
//=================================================================

void usart2_init(uint16 baud);
void usart2_send_byte(char ch);
void usart2_send_data(unsigned	char *send_data,unsigned char data_length);
void usart2_send_interrupt(void);

static	unsigned	char	usart2_tx_data[64];			//发射的数据数组
static	unsigned	char	usart2_tx_data_length;		//发射数据的长度
volatile static  bit  tx2_sign;


/*****************************************************************************
 函 数 名  : usart1_init
 功能描述  : USART1初始化，波特率9600
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

void usart1_init(uint16 baud)
{
    TRISC6 = 1;
    TRISC7 = 1;
    TRISC0 = 0;
	//RS485_RX()                 //  用于RS485接收模式
	//配置发送寄存器
    TX1STAbits.TX9D=0; //无奇偶校验位  
    TX1STAbits.TRMT=0; //Transmit Shift Register Status bit
    TX1STAbits.BRGH = 1;
    TX1STAbits.SYNC=0;      
    TX1STAbits.TXEN=1;      
    TX1STAbits.TX9=0; //发送8位

	//配置接收寄存器
    RC1STAbits.RX9D=0; //无奇偶校验位    
    RC1STAbits.OERR=0;      
    RC1STAbits.FERR=0;      
    RC1STAbits.ADDEN=0;      
    RC1STAbits.CREN=1; //Continuous Receive Enable bit  
    // RCSTA2bits.SREN=0;      
    RC1STAbits.RX9=0; //接收8位
    RC1STAbits.SPEN=1; //Serial Port Enable bit 
	
    SPBRG = ((PIC_CLK/(16UL * baud) -1)); //SPBRGx_VAL;	
    RCIE = 1;
	//TXIE = 1;     
}
/*****************************************************************************
 函 数 名  : usart1_send_byte
 功能描述  : USART发送单个字节
 输入参数  : char ch要发送的字符
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void usart1_send_byte(char ch)
{
    unsigned int uart_cnt  = 0;
	//RS485_TX();                 //  用于RS485发送模式
	TXREG=ch;
	while(!TX1STAbits.TRMT)
	{
		uart_cnt  ++;
		if( uart_cnt  > 1000)
		{
			uart_cnt  = 0;
			//RS485_RX()                 //  用于RS485接收模式
			break;
		}
	}
}
/*****************************************************************************
 函 数 名  : usart1_send_interrupt
 功能描述  : USART1中断发送一个字节
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

void usart1_send_interrupt(void)
{	
    static unsigned char tx_index;
	unsigned int i = 0;
    tx1_sign = BUSY;
	//RS485_TX();                 //  用于RS485发送模式
	TXREG = usart1_tx_data[tx_index];
	tx_index ++;
	//如果发射超过正常协议的数据
	if(tx_index >=usart1_tx_data_length)
	{
		//则直接结束
		//禁止发射
		TXIE = 0;	
		while(i++<500);
		tx1_sign = FREE;
		tx_index = 0;
		//RS485_RX();                 //  用于RS485接收模式
	}
}
/*****************************************************************************
 函 数 名  : usart1_send_data
 功能描述  : USART1发送一串数据
 输入参数  : unsigned char *send_data发送起始地址
             unsigned	char data_length数据长度
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void usart1_send_data(unsigned char *send_data,unsigned	char data_length)
{
	unsigned char i;
	if(tx1_sign == FREE)
	{
		for(i=0;i<data_length;i++)
		{
			usart1_tx_data[i]=send_data[i];
		}
		usart1_tx_data_length=data_length;
		TXIE=1;	//允许发射中断
	}	
}

/*****************************************************************************
 函 数 名  : usart2_init
 功能描述  : USART2初始化，波特率设置9600
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

void usart2_init(uint16 baud)
{
	ANSG1 = 0;
    ANSG2 = 0;
    TRISG1 = 1;
    TRISG2 = 1;
	
	//配置发送寄存器
	TX2STAbits.TX9D = 0; 		//无奇偶校验位
	TX2STAbits.TRMT = 0; 		//Transmit Shift Register Status bit
	TX2STAbits.BRGH = 1;
	TX2STAbits.SYNC = 0;
	TX2STAbits.TXEN = 1;
	TX2STAbits.TX9  = 0; 		//发送8位
	// TXSTA2bits.CSRC=1;


    //配置接收寄存器
	RC2STAbits.RX9D = 0; 		//无奇偶校验位
	RC2STAbits.OERR = 0;
	RC2STAbits.FERR = 0;
	RC2STAbits.ADDEN = 0;
	RC2STAbits.CREN = 1; 		//Continuous Receive Enable bit
	// RCSTA2bits.SREN=0;
	RC2STAbits.RX9 = 0; 		//接收8位
	RC2STAbits.SPEN = 1; 		//Serial Port Enable bit
	
	SPBRG2 = ((PIC_CLK/(16UL * baud) -1));//SPBRGx_VAL;
	RC2IE = 1;
	//TX2IE = 1;
}
/*****************************************************************************
 函 数 名  : usart2_send_byte
 功能描述  : USART2放一个字节
 输入参数  : char ch要发送的字符
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void usart2_send_byte(char ch)
{
    unsigned int uart_cnt = 0;
	TX2REG=ch;
	while(!TX2STAbits.TRMT)
	{
		uart_cnt  ++;
		if( uart_cnt > 1000)
		{
			uart_cnt = 0;
			break;
		}
	}

}
/*****************************************************************************
 函 数 名  : usart2_send_interrupt
 功能描述  : 中断发送一个字节
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

void usart2_send_interrupt(void)
{	
    static unsigned char tx_index;
    tx2_sign = BUSY;
	TX2REG = usart2_tx_data[tx_index];
	tx_index ++;
	//如果发射超过正常协议的数据
	if(tx_index >=usart2_tx_data_length)
	{
		//则直接结束
		//禁止发射
		TX2IE = 0;	
		tx2_sign = FREE;
		tx_index = 0;
	}
}
/*****************************************************************************
 函 数 名  : usart2_send_data
 功能描述  : USART2发送一串数据
 输入参数  : unsigned char *send_data发送数据首地址，
             unsigned	char data_length发送长度
 
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/
void usart2_send_data(unsigned char *send_data,unsigned	char data_length)
{
	unsigned char i;
	if(tx2_sign == FREE)
	{
		for(i=0;i<data_length;i++)
		{
			usart2_tx_data[i]=send_data[i];
		}
		usart2_tx_data_length=data_length;
		TX2IE=1;	//允许发射中断
	}	
}

