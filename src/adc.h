#ifndef __ADC_H__
#define __ADC_H__

#include "pic.h"
#include "stdlib.h"
#include "string.h"

#define TEMP_MID       0
#define TEMP_IN        1
#define VREF_ADC       2
#define T1_VALUE      read_adc_value(0)     //T1温度
#define T2_VALUE      read_adc_value(1)      //T2温度
#define VREF          read_adc_value(2)     //输入电压
/****************************对外接口相关函数定义***********************************/
extern void adc_init(void);                   //ADC初始化
extern void adc_go_on(void);                 //启动ADC
extern void read_adc(void);                   //读取AD转换结果
extern unsigned int read_adc_value(unsigned char channel );//查询方法读取AD转换结果
extern unsigned int adc_read(unsigned char channel);   //读取AD转换数值

#endif
