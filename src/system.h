#ifndef __SYSTEM_H__
#define __SYSTEM_H__


#include "com_config.h"


extern void interrupt_enable(void);
extern void interrupt_disable(void);
extern void interrupt_init(void);
extern void wdt_enable(void);	
extern void wdt_disable(void);	
extern void feed_wdt(void);
extern void system_init(void);
extern void task_run(void);
#endif 
