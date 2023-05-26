#include "led.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//LED 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//初始化PB0和PB1为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{	
	RCC->AHB4ENR|=1<<3;	//使能PORTB时钟 
	GPIO_Set(GPIOD,PIN14|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU); //PB0,PB1设置
	LED0(1);			//关闭DS0
	LED1(1);			//关闭DS1
}







