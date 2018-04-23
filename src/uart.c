#include "com_config.h"


#define 	PIC_CLK 		    16000000//4000000                    // ʱ��Ƶ��
#define     BAUD                9600                                 //������
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



static	unsigned	char	usart1_tx_data[64];			//�������������
static	unsigned	char	usart1_tx_data_length;		//�������ݵĳ���
volatile static  bit  tx1_sign;

//=================================================================
//===========================USART2=================================
//=================================================================

void usart2_init(uint16 baud);
void usart2_send_byte(char ch);
void usart2_send_data(unsigned	char *send_data,unsigned char data_length);
void usart2_send_interrupt(void);

static	unsigned	char	usart2_tx_data[64];			//�������������
static	unsigned	char	usart2_tx_data_length;		//�������ݵĳ���
volatile static  bit  tx2_sign;


/*****************************************************************************
 �� �� ��  : usart1_init
 ��������  : USART1��ʼ����������9600
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void usart1_init(uint16 baud)
{
    TRISC6 = 1;
    TRISC7 = 1;
    TRISC0 = 0;
	//RS485_RX()                 //  ����RS485����ģʽ
	//���÷��ͼĴ���
    TX1STAbits.TX9D=0; //����żУ��λ  
    TX1STAbits.TRMT=0; //Transmit Shift Register Status bit
    TX1STAbits.BRGH = 1;
    TX1STAbits.SYNC=0;      
    TX1STAbits.TXEN=1;      
    TX1STAbits.TX9=0; //����8λ

	//���ý��ռĴ���
    RC1STAbits.RX9D=0; //����żУ��λ    
    RC1STAbits.OERR=0;      
    RC1STAbits.FERR=0;      
    RC1STAbits.ADDEN=0;      
    RC1STAbits.CREN=1; //Continuous Receive Enable bit  
    // RCSTA2bits.SREN=0;      
    RC1STAbits.RX9=0; //����8λ
    RC1STAbits.SPEN=1; //Serial Port Enable bit 
	
    SPBRG = ((PIC_CLK/(16UL * baud) -1)); //SPBRGx_VAL;	
    RCIE = 1;
	//TXIE = 1;     
}
/*****************************************************************************
 �� �� ��  : usart1_send_byte
 ��������  : USART���͵����ֽ�
 �������  : char chҪ���͵��ַ�
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void usart1_send_byte(char ch)
{
    unsigned int uart_cnt  = 0;
	//RS485_TX();                 //  ����RS485����ģʽ
	TXREG=ch;
	while(!TX1STAbits.TRMT)
	{
		uart_cnt  ++;
		if( uart_cnt  > 1000)
		{
			uart_cnt  = 0;
			//RS485_RX()                 //  ����RS485����ģʽ
			break;
		}
	}
}
/*****************************************************************************
 �� �� ��  : usart1_send_interrupt
 ��������  : USART1�жϷ���һ���ֽ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void usart1_send_interrupt(void)
{	
    static unsigned char tx_index;
	unsigned int i = 0;
    tx1_sign = BUSY;
	//RS485_TX();                 //  ����RS485����ģʽ
	TXREG = usart1_tx_data[tx_index];
	tx_index ++;
	//������䳬������Э�������
	if(tx_index >=usart1_tx_data_length)
	{
		//��ֱ�ӽ���
		//��ֹ����
		TXIE = 0;	
		while(i++<500);
		tx1_sign = FREE;
		tx_index = 0;
		//RS485_RX();                 //  ����RS485����ģʽ
	}
}
/*****************************************************************************
 �� �� ��  : usart1_send_data
 ��������  : USART1����һ������
 �������  : unsigned char *send_data������ʼ��ַ
             unsigned	char data_length���ݳ���
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

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
		TXIE=1;	//�������ж�
	}	
}

/*****************************************************************************
 �� �� ��  : usart2_init
 ��������  : USART2��ʼ��������������9600
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void usart2_init(uint16 baud)
{
	ANSG1 = 0;
    ANSG2 = 0;
    TRISG1 = 1;
    TRISG2 = 1;
	
	//���÷��ͼĴ���
	TX2STAbits.TX9D = 0; 		//����żУ��λ
	TX2STAbits.TRMT = 0; 		//Transmit Shift Register Status bit
	TX2STAbits.BRGH = 1;
	TX2STAbits.SYNC = 0;
	TX2STAbits.TXEN = 1;
	TX2STAbits.TX9  = 0; 		//����8λ
	// TXSTA2bits.CSRC=1;


    //���ý��ռĴ���
	RC2STAbits.RX9D = 0; 		//����żУ��λ
	RC2STAbits.OERR = 0;
	RC2STAbits.FERR = 0;
	RC2STAbits.ADDEN = 0;
	RC2STAbits.CREN = 1; 		//Continuous Receive Enable bit
	// RCSTA2bits.SREN=0;
	RC2STAbits.RX9 = 0; 		//����8λ
	RC2STAbits.SPEN = 1; 		//Serial Port Enable bit
	
	SPBRG2 = ((PIC_CLK/(16UL * baud) -1));//SPBRGx_VAL;
	RC2IE = 1;
	//TX2IE = 1;
}
/*****************************************************************************
 �� �� ��  : usart2_send_byte
 ��������  : USART2��һ���ֽ�
 �������  : char chҪ���͵��ַ�
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

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
 �� �� ��  : usart2_send_interrupt
 ��������  : �жϷ���һ���ֽ�
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void usart2_send_interrupt(void)
{	
    static unsigned char tx_index;
    tx2_sign = BUSY;
	TX2REG = usart2_tx_data[tx_index];
	tx_index ++;
	//������䳬������Э�������
	if(tx_index >=usart2_tx_data_length)
	{
		//��ֱ�ӽ���
		//��ֹ����
		TX2IE = 0;	
		tx2_sign = FREE;
		tx_index = 0;
	}
}
/*****************************************************************************
 �� �� ��  : usart2_send_data
 ��������  : USART2����һ������
 �������  : unsigned char *send_data���������׵�ַ��
             unsigned	char data_length���ͳ���
 
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

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
		TX2IE=1;	//�������ж�
	}	
}

