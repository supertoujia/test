#ifndef __TIMER_H__
#define __TIMER_H__



#include "com_config.h"





/* timer */
void open_timer_0(uint16 ms, uint8 intr_en, uint8 intr_pr,uint8 tmr_on);
void set_timer_0_period(void);
void close_timer_0(void);

void open_timer_1(uint16 ms, uint8 intr_en, uint8 intr_pr, uint8 tmr_on);
void set_timer_1_period(void);
void close_timer_1(void);

void open_timer_2(uint16 _4us, uint8 intr_en, uint8 intr_pr, uint8 tmr_on);
void close_timer_2(void);

void open_timer_3(uint16 ms, uint8 intr_en, uint8 intr_pr,uint8 tmr_on);
void set_timer_3_period(void);
void close_timer_3(void);

void open_timer_4(uint16 _64us, uint8 intr_en, uint8 intr_pr,uint8 tmr_on);
void close_timer_4(void);

void open_timer_5(uint16 ms, uint8 intr_en, uint8 intr_pr,uint8 tmr_on);
uint16 get_timer_5(void);
void set_timer_5_period(void);
void close_timer_5(void);

void open_timer_6(uint16 _1us, uint8 intr_en, uint8 intr_pr,uint8 tmr_on);
void close_timer_6(void);

void open_timer_7(uint16 ms, uint8 intr_en, uint8 intr_pr,uint8 tmr_on);
void set_timer_7_period(void);
void close_timer_7(void);






#endif   
