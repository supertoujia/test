#include "com_config.h"
#ifndef __LAMP_H__
#define __LAMP_H__




extern void lamp_init(void);
extern void lamp_on(void);
extern void lamp_off(void);
extern void lamp_pwm_on(void);
extern void lamp_color_change(void);
extern void color_switch_on(void);
extern void color_switch_off(void);
extern void lamp_1ms_process(void);
extern void lamp_process(void);
extern void lamp_ctrl(unsigned char lamp_color);
extern void lamp_out_process(void);






#endif
