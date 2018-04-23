#include "stdio.h"
//ϵͳ��ض�������
#define MAX_TASKS    30                                        /* ���������*/
#define MS  *8  





/**********************************��غ�������****************************************/

void os_time_tick(void);


/**********************************��ر�������****************************************/

//���������������ṹ
typedef struct 
{
    void (*func_process)(void);               //��������ָ��
    uint16 period;                            //�����Ӧ��ʱ��Ƭ���ڣ�֧�����65535*ϵͳʱ���׼
    uint16 time ;
} _task_;

static _task_ tasks[] =
{

}


static _task_ *sys_task;        //����һ����������ָ��
static uint16 system_tick = 0;              //ϵͳ�δ�ʱ��


/*****************************************************************************
 �� �� ��  : task_run
 ��������  : ִ�����ϵͳ����
 �������  : ��
 �������  : ��
 �� �� ֵ  :
 ���ú���  :
 ��������  :

 �޸���ʷ      :
  1.��    ��   : 2017��3��9��17:21:26
 ��    ��  : SJY
 �޸�����  : �����ɺ���

*****************************************************************************/

void task_run(void)
{
    uint8 len = sizeof(tasks)/ sizeof(_task_) ;    //��������ĸ���   
    
    system_tick = 0;    
    do
    {
        for(sys_task = tasks; sys_task < tasks+len; sys_task++)        //��ѯ�Ƿ���������Ҫִ��
        {
            if((system_tick - sys_task->time) > = ( sys_task->period ))//��ѯ���������Ƿ񵽴�
            {
                sys_task->time = system_tick;   //ʱ��Ƭ���¸�ֵ 
                sys_task->func_process();       //��������
            }
        }
    }while(1);
}


  
