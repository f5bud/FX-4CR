#ifndef __ADC_H
#define __ADC_H	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//ADC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/10
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

#define ADC_CH0 0 //通道 0
#define ADC_CH1 1 //通道 1
#define ADC_CH2 2 //通道 2
#define ADC_CH3 3 //通道 3
#define ADC_CH4 4 //通道 4
#define ADC_CH5 5 //通道 5
#define ADC_CH6 6 //通道 6
#define ADC_CH7 7 //通道 7
#define ADC_CH8 8 //通道 8
#define ADC_CH9 9 //通道 9
#define ADC_CH10 10 //通道 10
#define ADC_CH11 11 //通道 11
#define ADC_CH12 12 //通道 12
#define ADC_CH13 13 //通道 13
#define ADC_CH14 14 //通道 14
#define ADC_CH15 15 //通道 15
#define ADC_CH16 16 //通道 16
#define ADC_CH17 17 //通道 17
#define ADC_CH18 18 //通道 18  	      	    
	   									   
void Adc1_Init(void); 				//ADC1初始化
void Adc1_Channel_Selection(u8 ch);
u16  Get_Adc1(u8 ch); 				//获得某个通道值 
u16 Get_Adc1_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值 
void Adc2_Init(void); 				//ADC2初始化
u16  Get_Adc2(u8 ch); 				//获得某个通道值 
u16 Get_Adc2_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值
void Adc3_Init(void); 				//ADC3初始化
u16  Get_Adc3(u8 ch); 				//获得某个通道值 
void Adc3_Channel_Selection(u8 ch);//ADC3通道变换
void Adc2_Channel_Selection(u8 ch);
#endif 















