#include "pic.h"
#include "stdlib.h"
#include "string.h"

#ifndef __TEMPETATURE_H__
#define __TEMPERATURE_H__

#define T1_VALUE      read_adc_value(0)      //T1�¶�
#define T2_VALUE      read_adc_value(1)      //T2�¶�

/****************************����ӿ���غ�������***********************************/

extern unsigned int calculate_temperature(unsigned char tab_length,unsigned channel);

extern float calculate (unsigned char tab_length,unsigned int temp_ad);


#endif
