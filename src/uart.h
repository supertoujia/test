#ifndef __USART_H__
#define __USART_H__

#include "com_config.h"

/****************************对外接口相关函数定义***********************************/

//串口1相关函数声明
extern void usart1_init(uint16 baud);
extern void usart1_send_byte(char ch);
extern void get_usart1_data(unsigned char *usart1_dat);
extern void usart1_rec_int(void);
extern void usart1_send_interrupt(void);
extern void usart1_send_data(unsigned	char	*send_data,unsigned	char data_length);
extern bit usart1_data_check(void);


//串口1相关函数声明
extern void usart2_init(uint16 baud);
extern void usart2_send_byte(char ch);
extern void get_usart2_data(unsigned char *usart2_dat);
extern void usart2_rec_int(void);
extern void usart2_send_interrupt(void);
extern void usart2_send_data(unsigned	char	*send_data,unsigned	char data_length);
extern bit  usart2_data_check(void);



#endif


