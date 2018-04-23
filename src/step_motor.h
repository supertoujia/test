#ifndef __STEP_MOTOR_H__
#define __STEP_MOTOR_H__

#include "com_config.h"


#define FLOW_STEP_MOTOR 0
#define TEMP_STEP_MOTOR 1
#define STEP_MOTOR_NUM  2

void set_step_motor_running(uint8 num, int16 target, uint16 motor_pps_100us);
// void set_step_motor_pps_time(uint8 num, uint16 motor_pps_100us);
void set_step_motor_pps_time(uint8 num, int16 target, uint16 motor_pps_100us);

uint8 is_motor_ok(uint8 num);
int16 get_motor_current_pos(uint8 num);
int16 get_motor_target_pos(uint8 num);
void set_step_motor_stop(uint8 num);


void step_motors_intr_func(void);
void step_motors_init(void);
void clear_motor_pos(uint8 num);
#endif   
