#include"temperature.h"
#include"adc.h"

//	  //PT100, 2分法， 查表， 根据电阻的AD 值计算温度。
//	  //R25=50KΩ  精度：±1% （ 分压电阻=56KΩ，5V）	 B25/50=3950K 精度：±1%	
//	   uint16 const RTD_TAB_PT100[29] =    //表格是以5度为一步，即-30, -25, - 20..... 
//	  { 
//		  59  ,79	  ,104	  ,135	  ,171	  ,212	  ,260	  ,312	  ,368	  ,425	  , 	  //-30.c  ...15.c
//		  484	  ,541	  ,596	  ,647	  ,695	  ,737	  ,776	  ,809	  ,839	  ,864	  ,   //20.c   ...	65.c
//		  886	  ,905	  ,921	  ,935	  ,947	  ,957	  ,966	  ,973	  ,980	  , 		  //70.c   ...	110.c
//	  }; 
//	  
  //PT100, 2分法， 查表， 根据电阻的AD 值计算温度。
  //R25=50KΩ  精度：±1% （ 分压电阻=50KΩ，5V）	 B25/50=3950K 精度：±1%	
//	   uint16 const RTD_TAB_PT100[29] =    //表格是以5度为一步，即-30, -25, - 20..... 
//	  { 
//		
//		53	,	71	,	94	,	122 	,	155 	,	194 	,	238 	,	288 	,	341 	,	398 	,//-30.c  ...15.c
//		455 	,	512 	,	567 	,	620 	,	669 	,	713 	,	754 	,	789 	,	821 	,	848 	,//20.c   ...  65.c
//		872 	,	893 	,	910 	,	925 	,	939 	,	950 	,	959 	,	968 	,	975 	,//70.c   ...  110.c
//		}; 

//PT100, 2分法， 查表， 根据电阻的AD 值计算温度。
//R25=50KΩ  精度：±1% （ 分压电阻=49.9KΩ，5V）    B25/50=3950K 精度：±1%	  
//uint16 const RTD_TAB_PT100[29] =	//表格是以5度为一步，即-30, -25, - 20..... 
//{ 
//	53	,	71	,	94	,	122 	,	155 	,	194 	,	238 	,	287 	,	341 	,	397 	,//-30.c  ...15.c
//	454 	,	511 	,	567 	,	620 	,	668 	,	713 	,	753 	,	789 	,	820 ,	848 	,//20.c   ...  65.c
//	872 	,	892 	,	910 	,	925 	,	938 	,	950 	,	959 	,	967 	,	975 	,//70.c   ...  110.c
//}; 

//	  //PT100, 2分法， 查表， 根据电阻的AD 值计算温度。
//	  //R25=50KΩ  精度：±1% （ 分压电阻=56KΩ，5V）	 B25/50=3950K 精度：±1%	
volatile unsigned int const temp_tab[29] =    //表格是以5度为一步，即-30, -25, - 20..... 
{ 
  59  ,79	  ,104	  ,135	  ,171	  ,212	  ,260	  ,312	  ,368	  ,425	  , 	  //-30.c  ...15.c
  484	  ,541	  ,596	  ,647	  ,695	  ,737	  ,776	  ,809	  ,839	  ,864	  ,   //20.c   ...	65.c
  886	  ,905	  ,921	  ,935	  ,947	  ,957	  ,966	  ,973	  ,980	  , 		  //70.c   ...	110.c
}; 

volatile unsigned int const temp_tab1[160] = {
     165, 169, 173, 177, 181, 185, 189, 194, 198, 203, 	 //   0 -10 ~ -5.5摄氏度
     207, 212, 216, 221, 226, 231, 235, 240, 245, 250, 	 //  10  -5 ~ -0.5摄氏度
	 255, 260, 266, 271, 276, 281, 287, 292, 297, 303, 	 //  20   0 ~  4.5摄氏度
	 308, 314, 319, 325, 331, 336, 342, 348, 354, 359, 	 //  30   5 ~  9.5摄氏度
	 365, 371, 377, 382, 388, 394, 400, 406, 412, 418, 	 //  40  10 ~ 14.5摄氏度
	 424, 429, 435, 441, 447, 453, 459, 465, 471, 477, 	 //  50  15 ~ 19.5摄氏度
	 483, 488, 494, 500, 506, 512, 517, 523, 529, 535, 	 //  60  20 ~ 24.5摄氏度
	 540, 546, 552, 557, 563, 568, 574, 579, 585, 590, 	 //  70  25 ~ 29.5摄氏度
	 595, 601, 606, 611, 617, 622, 627, 632, 637, 642, 	 //  80  30 ~ 34.5摄氏度 
	 647, 652, 657, 662, 667, 671, 676, 681, 685, 690, 	 //  90  35 ~ 39.5摄氏度 
	 695, 699, 703, 708, 712, 716, 720, 725, 729, 733, 	 // 100  40 ~ 44.5摄氏度
	 737, 741, 745, 749, 753, 757, 761, 764, 768, 771, 	 // 110  45 ~ 49.5摄氏度
	 775, 779, 782, 786, 789, 792, 795, 799, 802, 805, 	 // 120  50 ~ 54.5摄氏度 
	 808, 811, 815, 818, 821, 823, 826, 829, 832, 835, 	 // 130  55 ~ 59.5摄氏度 
	 838, 840, 843, 846, 848, 851, 853, 856, 858, 860, 	 // 140  60 ~ 64.5摄氏度 
	 863, 865, 867, 870, 872, 874, 876, 878, 881, 883	 // 150  65 ~ 69.5摄氏度
	 };
/*****************************************************************************
 函 数 名  : calculate_temperature
 功能描述  : 二分法查表计算温度
 输入参数  :  tab_length查表长度， temp_ad采集的温AD值
 输出参数  : temp_value 温度值
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史	   :
  1.日	  期   : 2016年5月13日
 作    者  : SJY
 修改内容  : 新生成函数

*****************************************************************************/

unsigned int  calculate_temperature(unsigned char tab_length,unsigned channel)
{

	unsigned char left;
	unsigned char right;
	unsigned char mid;
	unsigned int temp_ad = 0;
	char    temp_i = 0;
	float temp_value = 0;

	temp_ad = read_adc_value(channel);   //读取AD值
	left = 0;
	right = tab_length-1;
	mid = (left+right)/2;

	if(temp_ad >temp_tab[right])
		return 110;
	if(temp_ad <temp_tab[left])
		return 0;
	while(((right-left)!=1)&&(temp_tab[mid]!=temp_ad))
	{
	    
		if(temp_tab[mid]< temp_ad)
		{
			left = mid;
		}
		else if(temp_tab[mid] > temp_ad)
		{
            right = mid;    
		}
		mid = (left+right)/2; 
	}
	/* while( (right-left)!=1 ) // 2分法查表。 
    { 
	    if (temp_ad < temp_tab[mid]) 
	    { 
		    right = mid; 
		    mid = (right + left) / 2; 
	    } 
	    else if (temp_ad > temp_tab[mid]) 
	    { 
		    left = mid;
		   mid = (right + left) / 2;
	    } 
	    else 
	    { 
		    temp_i =mid*5 - 30; 
		    temp_value = (float)temp_i;       
	        return temp_value; 
	    } 
    } */	
	temp_i = mid*5 -30;
	temp_value = (((temp_ad-temp_tab[left])*5.0)
		           /(temp_tab[right]-temp_tab[left])) +temp_i ;
		           
   // return (unsigned int)(temp_value*10);
   return (unsigned int)(temp_value*10);
}
float calculate (unsigned char tab_length,unsigned int temp_ad)
{

	unsigned char low;
	unsigned char hig;
	unsigned char midl;
	float   temp_i = 0;
	float temp_value = 0;
	low = 0;
	hig = tab_length-1;
	midl = (hig+low)/2;

	if(temp_ad >temp_tab1[hig])
		return 110;
	if(temp_ad <temp_tab1[low])
		return 0;
	while(((hig-low)!=1)&&(temp_tab1[midl]!=temp_ad))
	{
	    
		if(temp_tab1[midl]< temp_ad)
		{
			low = midl;
		}
		else if(temp_tab1[midl] > temp_ad)
		{
            hig = midl;
		}
		midl = (hig+low)/2; 
	}
	temp_i = midl*0.5 -10;
	temp_value = (((temp_ad-temp_tab1[low])*0.5)
		           /(temp_tab1[hig]-temp_tab1[low])) +temp_i ;
    return temp_value;

}

