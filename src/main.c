
#include "system.h"


//字配置
//CONFIG(0x29A4);
__CONFIG(0x29E4);
__CONFIG(0x0EFF);


void main(void)
{
	system_init();	               //系统初始化
	//while(1)
   // {
       task_run();	
    //}
	
	
}
