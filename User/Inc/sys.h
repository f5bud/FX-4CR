#ifndef __SYS_H
#define __SYS_H	 
#include "stm32h7xx.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//系统时钟初始化	
//包括时钟设置/中断管理/GPIO设置等
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/6
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20180806
//1,修改PLL2的倍频系数为440,pll2_r_ck=220Mhz
//2,新增对pll2_p_ck的分频设置,2分频,得到220M的时钟频率
//3,修改pll1_q_ck的注释,改为4分频,200Mhz的频率说明
//////////////////////////////////////////////////////////////////////////////////  

#define SYSTEM_CLK		480000000//系统时钟
//0,不支持OS
//1,支持OS
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持OS
///////////////////////////////////////////////////////////////////////////////////
//定义一些常用的数据类型短关键字 
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;
 
typedef volatile uint32_t  vu32;
typedef volatile uint16_t vu16;
typedef volatile uint8_t  vu8; 

////////////////////////////////////////////////////////////////////////////////// 
//Ex_NVIC_Config专用定义
#define GPIO_A 				0
#define GPIO_B 				1
#define GPIO_C				2
#define GPIO_D 				3
#define GPIO_E 				4
#define GPIO_F 				5
#define GPIO_G 				6 
#define GPIO_H 				7 
#define GPIO_I 				8 
#define GPIO_J 				9 
#define GPIO_K 				10 

#define FTIR   				1  		//下降沿触发
#define RTIR   				2  		//上升沿触发

//GPIO设置专用宏定义
#define GPIO_MODE_IN    	0		//普通输入模式
#define GPIO_MODE_OUT		1		//普通输出模式
#define GPIO_MODE_AF		2		//AF功能模式
#define GPIO_MODE_AIN		3		//模拟输入模式

#define GPIO_SPEED_LOW		0		//GPIO速度(低速,12M)
#define GPIO_SPEED_MID		1		//GPIO速度(中速,60M)
#define GPIO_SPEED_FAST		2		//GPIO速度(快速,85M)
#define GPIO_SPEED_HIGH		3		//GPIO速度(高速,100M)  

#define GPIO_PUPD_NONE		0		//不带上下拉
#define GPIO_PUPD_PU		1		//上拉
#define GPIO_PUPD_PD		2		//下拉
#define GPIO_PUPD_RES		3		//保留 

#define GPIO_OTYPE_PP		0		//推挽输出
#define GPIO_OTYPE_OD		1		//开漏输出 

//GPIO引脚位置定义
#define PIN0				1<<0
#define PIN1				1<<1
#define PIN2				1<<2
#define PIN3				1<<3
#define PIN4				1<<4
#define PIN5				1<<5
#define PIN6				1<<6
#define PIN7				1<<7
#define PIN8				1<<8
#define PIN9				1<<9
#define PIN10				1<<10
#define PIN11				1<<11
#define PIN12				1<<12
#define PIN13				1<<13
#define PIN14				1<<14
#define PIN15				1<<15 
////////////////////////////////////////////////////////////////////////////////// 
u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq);		//系统时钟设置
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq); //时钟初始化  
void Sys_Soft_Reset(void);      							//系统软复位
void Cache_Enable(void);									//使能catch
void Sys_Standby(void);         							//待机模式 	
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);	//设置偏移地址
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);			//设置NVIC分组
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group);//设置中断
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM);				//外部中断配置函数(只对GPIOA~I)
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx);		//GPIO复用功能设置
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD);//GPIO设置函数 
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status);	//设置某个IO口的输出状态
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx);				//读取某个IO口的输入状态
//以下为汇编函数
void WFI_SET(void);		//执行WFI指令
void INTX_DISABLE(void);//关闭所有中断
void INTX_ENABLE(void);	//开启所有中断
void MSR_MSP(u32 addr);	//设置堆栈地址 
#endif











