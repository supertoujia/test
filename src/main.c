
#include "system.h"


//������
//CONFIG(0x29A4);
__CONFIG(0x29E4);
__CONFIG(0x0EFF);


void main(void)
{
	system_init();	               //ϵͳ��ʼ��
	//while(1)
   // {
       task_run();	
    //}
	
	
}
