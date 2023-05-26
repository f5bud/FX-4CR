#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
/*
*******************************************************************************	 
* 	本程序只供学习使用，未经作者许可，不得用于其它任何用途
*	 FX-4C 
*	定时器 驱动代码	   
*	BG2IXD
*	QQ:1413894726
*	创建日期:2020/9/9
*	版本：V1.1
*	版权所有，盗版必究。									  
********************************************************************************
*/	 

/* TIM6时钟 */
#define TIM6_CLOCK		200000000

/* 通过改变TIM1->CCR4的值改变占空比，从而改变CW侧音音量 */
#define CW_SIDETONE_VOL	 TIM1->CCR4

//通过改变TIM3->CCR4的值来改变占空比，从而控制LED0的亮度
#define LED0_PWM_VAL TIM3->CCR4    

//TIM15 CH2作为PWM DAC的输出通道 
#define PWM_DAC_VAL TIM15->CCR2 
/*
*	在此定义若干个软件定时器全局变量
*	注意，必须增加__IO 即 volatile，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。
*/
#define TMR_COUNT	4		/* 软件定时器的个数 （定时器ID范围 0 - 3) */

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
	TMR_ONCE_MODE = 0,		/* 一次工作模式 */
	TMR_AUTO_MODE = 1		/* 自动定时工作模式 */
}TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
	volatile uint8_t Mode;		/* 计数器模式，1次性 */
	volatile uint8_t Flag;		/* 定时到达标志  */
	volatile uint32_t Count;	/* 计数器 */
	volatile uint32_t PreLoad;	/* 计数器预装值 */
}SOFT_TMR;

void SysTick_init(u32 SYSCLK,u16 Xms);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
void TIM6_Int_Init(u16 arr,u16 psc);/* TIM6触发ADC/DAC */
void TIM2_Int_Init(u16 arr,u16 psc);
void TIM3_Int_Init(u16 arr,u16 psc); 
void TIM3_PWM_Init(u32 arr,u32 psc);
void TIM5_CH1_Cap_Init(u32 arr,u16 psc);
void TIM1_CH4_PWM_Init(u32 arr,u32 psc);
#endif























