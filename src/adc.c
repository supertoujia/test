#include"adc.h"
#include "io_setting.h"

/****************************��ر�������***********************************/

unsigned int ad_value[6];                	//ADת����ֵ�ݴ�����

uint16 tempt_ad[4];

/****************************��غ�������***********************************/
//void adc_init(void);                   //ADC��ʼ��
void adc_go_on(void);                    //����ADC
void read_adc(void);                     //��ȡADת�����
unsigned int  read_adc_value(unsigned char channel );//��ѯ������ȡADת�����
unsigned int adc_read(unsigned char channel);   //��ȡADת����ֵ

/*****************************************************************************
 �� �� ��  : adc_init
 ��������  : ADCģ���ʼ��
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
void adc_init(void)
{
    ANSELF = 0B11000000;              //ʹ��AD����RF2/AN7��RF3/AN8��RF7/AN5
	TRISF  |= 0B11000000;              //����Ӧ��AD��������Ϊ����
	//ANSA3 = 1;
	//TRISA3 = 1;
	ADFM   = 1;                       //�Ҷ���
	ADCS2  = 1;ADCS1 = 1;ADCS0 = 0;   //ת��ʱ��  Fsoc/8  001
	// FVRCON |= 0B10000011;              //ʹ��ADC�ڲ��ο���ѹΪ4.096V
	
	ADPREF1 = 0; ADPREF0 = 0;         //ѡ�����ο���ѹ�ڲ��ο���ѹ
	ADNREF  = 0;                      //ѡ�񸺲ο���ѹVSS
	ADCON0 = (AN_5<< 2) ;		      //ѡ����Ӧͨ����ʹ��ADC, FOSC/2 osc.��������
	NOP();

	ADIF = 0;
	ADIE = 1;
	ADON = 1; 							 //����ADģ��	
	ADGO = 1;	
}
/*****************************************************************************
 �� �� ��  : adc_go_on
 ��������  : ����ADCת��
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
void adc_go_on(void)
{
	ADIE = 1;
	ADON = 1; 
    ADGO = 1;                       
}

/*****************************************************************************
 �� �� ��  :  read_adc
 ��������  : ��ȡADת����ֵ��������������
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
void read_adc(void)
{
    unsigned int adc_temp;
	static unsigned char ad_channel = 0;               //ADͨ����
	adc_temp = (unsigned int)(ADRESH<<8)+ADRESL;		
	switch(ad_channel)
	{
	    case TEMP_MID:             //�¶ȴ�����1
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
		case TEMP_IN:            //�¶ȴ�����2
		{
			ad_value[TEMP_IN] = adc_temp;
			ADCON0 = (AN_5<<2);
			ad_channel = TEMP_MID;
			break;
    	}
		case VREF_ADC:            
		{
			ad_value[VREF_ADC] = adc_temp;
			ADCON0 = (AN_5<<2);    //�����ѹ���
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
 �� �� ��  :  read_adc_value
 ��������  : ��ȡ��Ӧͨ��ADֵ
 �������  : channel ADCͨ����
 �������  :  ad_value[channel]ͨ����AD��ֵ
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/
unsigned int  read_adc_value(unsigned char channel )
{
     return ad_value[channel];
}


/*****************************************************************************
 �� �� ��  : adc_read
 ��������  : ��ѯ�ķ�����ȡ��Ӧͨ��ADֵ
 �������  : channel ADCͨ����
 �������  : ad_valueͨ����AD��ֵ
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2016��5��13��
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

unsigned int adc_read(unsigned char channel)
{
	long int ad_value = 0;
    unsigned char i,cnt;
	ANSELF = 0B00001100;              //ʹ��AD����RF2/AN7��RF3/AN8
    TRISF  = 0B00001100;              //����Ӧ��AD��������Ϊ����
    ANSELA = 0B00001000;              //ʹ��AD����RA3/AN3
    TRISA3 = 1;
	ADCON0 = (channel << 2) + 0x01;		//ѡ����Ӧͨ����ʹ��ADC, FOSC/2 osc.��������
	NOP();
	NOP();
	for(i=0;i<16;i++)
	{
	    ADGO = 1;
        while(ADGO)
        {
            cnt++	    ;		//�ȴ�ת�����
            if(cnt>=80)
                break;
        }
        ad_value =ad_value+ ADRESH*4 + ADRESL/64;
	}
	ad_value=ad_value/16;
	return (ad_value);
}

