#ifndef __MASSAGE_H__
#define __MASSAGE_H__


#define  POWER_CTRL_NONE          0X00           //�����й��ʿ���
#define  POWER_CTRL_FOR_AIR       0X01           //ֻ�Կ����ý��й��ʿ���
#define  POWER_CTRL_ALL           0X02           //�����й��ʿ��ƣ�ˮ���ñ�Ƶ��


//#define MASSAGE_CTRL_MODE  POWER_CTRL_NONE                  
#define MASSAGE_CTRL_MODE  POWER_CTRL_FOR_AIR               
//#define MASSAGE_CTRL_MODE  POWER_CTRL_ALL                   


void zero_corss_init(void);
void massage_init(void);
void massage_set(unsigned char work_mode,unsigned char grade);
void massage_ctrl_in_mian(void);
uint8 read_massage_state (void);
void massage_ctrl(void);
void massage_power_process(void);


#endif
