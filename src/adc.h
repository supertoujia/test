#ifndef __ADC_H__
#define __ADC_H__

#include "pic.h"
#include "stdlib.h"
#include "string.h"

#define TEMP_MID       0
#define TEMP_IN        1
#define VREF_ADC       2
#define T1_VALUE      read_adc_value(0)     //T1�¶�
#define T2_VALUE      read_adc_value(1)      //T2�¶�
#define VREF          read_adc_value(2)     //�����ѹ
/****************************����ӿ���غ�������***********************************/
extern void adc_init(void);                   //ADC��ʼ��
extern void adc_go_on(void);                 //����ADC
extern void read_adc(void);                   //��ȡADת�����
extern unsigned int read_adc_value(unsigned char channel );//��ѯ������ȡADת�����
extern unsigned int adc_read(unsigned char channel);   //��ȡADת����ֵ

#endif
