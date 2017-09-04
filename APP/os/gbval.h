/************************************************************************
 * 名称：全局变量管理
 * 简介：
 * 编写：邹应龙
 * 更新：2017-9-2                                              
 ************************************************************************/
#ifndef _GBVAL_H_
#define _GBVAL_H_

/************************************************************************
 *******************全局变量管理表****************** 
********************前20个属于计数器***************
 * 0，debug专用
 * 1，eeprom数据需要保存
 * 2，当前闹钟序号
 * 3，闹钟蜂鸣器打开
 * 4，当前继电器序号
 * 5，当前GUI偏移
 * 6，是否存在ds18b20 
 *******************后20个属于定时器***************
 * 20，闪烁半秒定时，=0时赋值1s
 * 21，不闪烁计时，3s
 * 22，熄灭屏幕计时，5min
 * 23，慢速长按键计时，2s
 * 24，快速长按键计时，5s
 * 25，蜂鸣器计时，根据函数
 * 26，设置界面无按键30s回到待机
                              
 ***********************************************************************/
#define GB_DEBUG		0
#define GB_EEP			1
#define GB_ALARM		2
#define GB_ALARM_OPEN	3
#define GB_RELAY		4
#define GB_GUI_OFFSET	5
#define GB_DS18B20		6

#define GB_FLASH		20
#define GB_NOFLASH		21
#define GB_SLEEP		22
#define GB_KEY_L1		23
#define GB_KEY_L2		24
#define GB_BEEP 		25		
#define GB_WAIT			26

/**********************************************************************
* 有三种类型，互斥，计数，定时
* 互斥和计数是可以放在一起的，无非是加1和加n的区别
* 定时是需要在中断中改变数值的
* 因此全局变量分为两部分，前半部分用于定时，后半部分用于计数
* 内部都是u16，2字节
************************************************************************/ 

#define GBVAL_MAX			40			//总共40个全局变量
#define GBVAL_TMR			20			//前面20个用于计数，即后20个用于定时

void gbvar_increase(u8 num);
void gbvar_reduce(u8 num);
void gbvar_set(u8 num, u16 cnt);
u8 gbvar_get(u8 num);
void gbvar_clr(u8 num);

void gbvar_tmr(void);
void gbvar_init(void);
#endif