#ifndef __WM8978_H
#define __WM8978_H
#include "sys.h"    									
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//WM8978 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define WM8978_ADDR				0X1A	//WM8978的器件地址,固定为0X1A 
 
#define EQ1_80Hz		0X00
#define EQ1_105Hz		0X01
#define EQ1_135Hz		0X02
#define EQ1_175Hz		0X03

#define EQ2_230Hz		0X00
#define EQ2_300Hz		0X01
#define EQ2_385Hz		0X02
#define EQ2_500Hz		0X03

#define EQ3_650Hz		0X00
#define EQ3_850Hz		0X01
#define EQ3_1100Hz		0X02
#define EQ3_14000Hz		0X03

#define EQ4_1800Hz		0X00
#define EQ4_2400Hz		0X01
#define EQ4_3200Hz		0X02
#define EQ4_4100Hz		0X03

#define EQ5_5300Hz		0X00
#define EQ5_6900Hz		0X01
#define EQ5_9000Hz		0X02
#define EQ5_11700Hz		0X03

  
u8 WM8978_Init(void);
void WM8978_AUXIN_Cfg(u8 auxr);
void WM8978_HPread_Cfg(void);
void WM8978_ADDA_Cfg(u8 dacen,u8 adcen);
void WM8978_Input_Cfg(u8 micen_l,u8 micen_r);
void WM8978_Output_Cfg(u8 dacen,u8 bpsen);
void WM8978_out34_Cfg(u8 outen);
void WM8978_MIC_Gain(u8 gain);
void WM8978_ALC_Cfg(u8 sel,u8 gain);
void WM8978_ADC_Gain(u8 gain);
void WM8978_DAC_Gain(u8 gain);
void WM8978_LINEIN_Gain(u8 l_gain,u8 r_gain);
void WM8978_AUX_Gain(u8 aux_l,u8 aux_r);
u8 WM8978_Write_Reg(u8 reg,u16 val); 
u16 WM8978_Read_Reg(u8 reg);
void WM8978_HPvol_Set(u8 voll,u8 volr);
void WM8978_SPKvol_Set(u8 volx);
void WM8978_I2S_Cfg(u8 fmt,u8 len);
void WM8978_3D_Set(u8 depth);
void WM8978_EQ_3D_Dir(u8 dir); 
void WM8978_EQ1_Set(u8 cfreq,u8 gain); 
void WM8978_EQ2_Set(u8 cfreq,u8 gain);
void WM8978_EQ3_Set(u8 cfreq,u8 gain);
void WM8978_EQ4_Set(u8 cfreq,u8 gain);
void WM8978_EQ5_Set(u8 cfreq,u8 gain);
void WM8978_Inpga_muter(u8 l_mute,u8 r_mute);
void WM8978_TR_set(u8 trvolue,u8 gain,u8 alc);
#endif









