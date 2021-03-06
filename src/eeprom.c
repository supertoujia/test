/*********************************************************************************************************
** 函数名称: eeprom_read_byte
** 功能描述: 
** 输入: 16位地址
** 输出: 读取到的数据8位
** 作者: 廖锦堂
** 日期: 2016年2月15日11:24
*********************************************************************************************************/
uint8 eeprom_read_byte(uint16 addr16)
{    
    EEADRH = (uint8)addr16>>8;
    EEADR  = (uint8)addr16;
    EEPGD = 0;//访问EEPROM
    CFGS = 0;//访问存储器
    RD = 1;//开始读
    NOP();
    return(EEDATA);    
}
/*********************************************************************************************************
** 函数名称: eeprom_write_byte
** 功能描述: 
** 输入: 16位地址，要写入的数据
** 输出: 写入成功1或失败0
** 作者: 廖锦堂
** 日期: 2016年2月15日11:25
*********************************************************************************************************/
uint8 eeprom_write_byte(uint16 addr16,uint8 data,uint8 should_be_0x55,uint8 shuld_be_0xaa)
{
    EEDATA = data;     
    EEADRH = (uint8)addr16>>8;
    EEADR  = (uint8)addr16;
    EEPGD = 0;//访问EEPROM
    CFGS = 0;//访问存储器
    
    GIE = 0;
    WREN = 1;//允许写操作
    EECON2 = 0X55;//写序列
    EECON2 = 0XAA;
    WR = 1;   
    while(WR == 1);//等待写操作完成
    WREN = 0;//禁止写操作
    GIE = 1;
    
    if(WRERR)
        {return 0;}
    else
        {return 1;}
}

