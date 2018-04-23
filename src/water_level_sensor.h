#ifndef __WATER_LEVEL_SENSRO_H__
#define __WATER_LEVEL_SENSRO_H__


#define H_WATER_SENSOR       1
#define L_WATER_SENSOR       0


uint8 read_water_level(uint8 num);
void water_level_in_int(void);
void water_level_judge(void);

#endif
