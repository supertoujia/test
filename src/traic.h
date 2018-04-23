
#ifndef __TRAIC_H__
#define __TRAIC_H__

#define   ALL_TRAIC          0X00
#define   AIR_PUMP           0X01   
#define   WATER_PUMP         0X02
#define   HEAT_TUB           0X03    
#define   CIR_PUMP            0X04   
#define   CLEAN_VALVE        0X05

void traic_init(void);
void traic_ctrl(uint8 ctrl_num ,uint8 ctrl_value);


#endif
