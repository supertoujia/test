#ifndef __DRAIN_WATER_H__
#define __DRAIN_WATER_H__



void drain_valve_init(void);
void drain_water_ctrl(uint8 cmd,uint8 value);
void open_valve(void);
void close_valve(void);


#endif

