#include "stdio.h"
//系统相关定义声明
#define MAX_TASKS    30                                        /* 最大任务数*/
#define MS  *8  





/**********************************相关函数声明****************************************/

void os_time_tick(void);


/**********************************相关变量声明****************************************/

//定义程序任务基本结构
typedef struct 
{
    void (*func_process)(void);               //程序任务指针
    uint16 period;                            //任务对应的时间片周期，支持最大65535*系统时间基准
    uint16 time ;                            //每次执行完任务对应的时间
} _task_;

static _task_ tasks[] =
{

}


static _task_ *sys_task;        //定义一个程序任务指针
static uint16 system_tick = 0;              //系统滴答时间


/*****************************************************************************
 函 数 名  : task_run
 功能描述  : 执行相关系统任务
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2017年3月9日17:21:26
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

void task_run(void)
{
    uint8 len = sizeof(tasks)/ sizeof(_task_) ;    //计算任务的个数   
    
    system_tick = 0;    
    do
    {
        for(sys_task = tasks; sys_task < tasks+len; sys_task++)        //查询是否有任务需要执行
        {
            if((system_tick - sys_task->time) > = ( sys_task->period ))//查询任务周期是否到达
            {
                sys_task->time = system_tick;   //时间片重新赋值 
                sys_task->func_process();       //调用任务
            }
        }
    }while(1);
}


  

