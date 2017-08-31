/************************************************************************
 * 名称：TM1640驱动，同时支持并口和串口
 * 简介：串口支持不同亮度和多根时钟线，并口只能单根时钟线
 * 编写：邹应龙
 * 更新：2017-8-17  
 ************************************************************************/
 #include "../main.h"

#define ds18b20_delay(n) 	{_delay_us(n);} 
/************************************************************************
 * 功能：复位
 * 描述：
 * 形参：
 * 返回：                       
 ************************************************************************/
static void ds18b20_rst(void)	   
{                 
	DS18B20_IO_OUT; //SET PA0 OUTPUT
	DS18B20_IO_L; //拉低DQ
    ds18b20_delay(750);    //拉低750us
    DS18B20_IO_H; //DQ=1 
	ds18b20_delay(15);     //15US
}

/************************************************************************
 * 功能：等待DS18B20的回应
 * 描述：
 * 形参：
 * 返回：0~未检测到存在，1~存在                       
 ************************************************************************/
static u8 ds18b20_check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN;								//input
    while (DS18B20_IO_R&&retry<200)
	{
		retry++;
		ds18b20_delay(1);
	};	 
	if(retry>=200)	return 1;
	else 			retry=0;
    while (!DS18B20_IO_R&&retry<240)
	{
		retry++;
		ds18b20_delay(1);
	};
	if(retry>=240)return 0;	    
	return 1;
}

/************************************************************************
 * 功能：从DS18B20读取一个位
 * 描述：
 * 形参：
 * 返回：1/0                       
 ************************************************************************/
static u8 ds18b20_read_bit(void) 			 // read one bit
{
    u8 data;
	DS18B20_IO_OUT;//SET PA0 OUTPUT
    DS18B20_IO_L; 
	ds18b20_delay(2);
    DS18B20_IO_H; 
	DS18B20_IO_IN;//SET PA0 INPUT
	ds18b20_delay(12);
	if(DS18B20_IO_R)	data=1;
    else				data=0;	 
    ds18b20_delay(50);           
    return data;
}

/************************************************************************
 * 功能：从DS18B20读取一个字节
 * 描述：
 * 形参：
 * 返回：读到的数据                      
 ************************************************************************/
static u8 ds18b20_read_byte(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=ds18b20_read_bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}

/************************************************************************
 * 功能：写一个字节到DS18B20
 * 描述：
 * 形参：要写入的字节
 * 返回：读到的数据                      
 ************************************************************************/
static void ds18b20_write_byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_IO_L;				// Write 1
            ds18b20_delay(2);                            
            DS18B20_IO_H;
            ds18b20_delay(60);             
        }
        else 
        {
            DS18B20_IO_L;				// Write 0
            ds18b20_delay(60);             
            DS18B20_IO_H;
            ds18b20_delay(2);                          
        }
    }
}

/************************************************************************
 * 功能：开始温度转换
 * 描述：
 * 形参：
 * 返回：                      
 ************************************************************************/
static void ds18b20_start(void)
{   						               
    ds18b20_rst();	   
	ds18b20_check();	 
    ds18b20_write_byte(0xCC);// skip rom
    ds18b20_write_byte(0x44);// convert
} 
  
/************************************************************************
 * 功能：初始化DS18B20的IO口 DQ 同时检测DS的存在
 * 描述：
 * 形参：
 * 返回：0~不存在，1~存在                      
 ************************************************************************/ 	 
u8 ds18b20_init(void)
{
	DS18B20_IO_H;
	ds18b20_rst();
	return ds18b20_check();
}  
 
/************************************************************************
 * 功能：从ds18b20得到温度值
 * 描述：精度0.1C
 * 形参：
 * 返回：温度值 （-550~1250）                     
 ************************************************************************/ 
s16 ds18B20_get_temp(void)
{
    u8 temp;
    u8 TL,TH;
	s16 tem;
    ds18b20_start();                    // ds1820 start convert
    ds18b20_rst();
    ds18b20_check();	 
    ds18b20_write_byte(0xCC);// skip rom
    ds18b20_write_byte(0xBE);// convert	    
    TL=ds18b20_read_byte(); // LSB   
    TH=ds18b20_read_byte(); // MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=tem*5/8;//转换     
	if(temp)	return tem; //返回温度值
	else		return -tem;    
} 
  