#include "com_config.h"
#include "system.h"
#include "traic.h"

#define   AIR_PUMP_IO_OUT()           {TRISE6 = 0; LATE6 = 0;}   //RE6
#define   WATER_PUMP_IO_OUT()         {TRISE7 = 0; LATE7 = 0;}   //RE7
#define   HEAT_TUB_IO_OUT()           {TRISD0 = 0; LATD0 = 0;}   //RE7 D0
#define   CIR_PUM_IO_OUT()            {TRISE5 = 0; LATE5 = 0;}   //RE5
#define   CLEAN_VALVE_IO_OUT()        {TRISE3 = 0; LATE3 = 0;}   //RE3


#define   AIR_PUMP_IO            LATE6 //RE6
#define   WATER_PUMP_IO          LATE7 //RE7
#define   HEAT_TUB_IO            LATD0 //RD0 //RE7
#define   CIR_PUM_IO             LATE5 //RE4
#define   CLEAN_VALVE_IO         LATE3 //RE3

void traic_ctrl(uint8 ctrl_num ,uint8 ctrl_value)
{
    switch(ctrl_num)
    {
        case AIR_PUMP:
		{
             AIR_PUMP_IO =  ctrl_value;
			 break;
        }
		case WATER_PUMP:
		{
             WATER_PUMP_IO =  ctrl_value;
			 break;
        }
		case HEAT_TUB:
		{
             HEAT_TUB_IO =  ctrl_value;
			 break;
        }
	    case CIR_PUMP:
		{
             CIR_PUM_IO =  ctrl_value;
			 break;
        }
		case CLEAN_VALVE:
		{
             CLEAN_VALVE_IO =  ctrl_value;
			 break;
        }
		case ALL_TRAIC :
		{
             AIR_PUMP_IO    = ctrl_value;         
             WATER_PUMP_IO  = ctrl_value;          
             HEAT_TUB_IO    = ctrl_value;           
             CIR_PUM_IO     = ctrl_value;          
             CLEAN_VALVE_IO = ctrl_value;          
			 break;
		}
		default:
		{
             AIR_PUMP_IO    = OFF;         
             WATER_PUMP_IO  = OFF;          
             HEAT_TUB_IO    = OFF;           
             CIR_PUM_IO     = OFF;          
             CLEAN_VALVE_IO = OFF;          
			 break;
        }

    }

}

void traic_init(void)
{

   AIR_PUMP_IO_OUT();           
   WATER_PUMP_IO_OUT() ;        
   HEAT_TUB_IO_OUT() ;          
   CIR_PUM_IO_OUT();            
   CLEAN_VALVE_IO_OUT();  
}



