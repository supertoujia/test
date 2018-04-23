#ifndef __KEEP_WARM_H__
#define __KEEP_WARM_H__

#include "com_config.h"



void keep_warm_in_int(void);
void keep_warm_in_mian(void);
void set_keep_warm_cmd(uint8 cmd);
void set_keep_warm_tempt(uint8 tempt);
uint8 read_keep_warm_tempt(void) ;  





#endif
