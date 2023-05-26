#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
   	   		   
//IO方向设置
#define SDA_IN()  {GPIO_Set(GPIOB,PIN7,GPIO_MODE_IN,0,1,GPIO_PUPD_PU);}//PC11输入模式
#define SDA_OUT() {GPIO_Set(GPIOB,PIN7,GPIO_MODE_OUT,0,1,GPIO_PUPD_PU);}//PC11输出模式
//#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}//输入
//#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;}//输出
//IO操作函数	 
#define IIC_SCL(x)    GPIO_Pin_Set(GPIOB,PIN6,x)//SCL
#define IIC_SDA(x)    GPIO_Pin_Set(GPIOB,PIN7,x)//SDA	 
#define READ_SDA   	  GPIO_Pin_Get(GPIOB,PIN7)//输入SDA 

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号 
#endif
















