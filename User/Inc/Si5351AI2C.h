#ifndef __SI5351A_IIC_H
#define __SI5351A_IIC_H
#include "sys.h"
//#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK MiniSTM32开发板
//IIC驱动 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/9
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
   	   		   
////IO方向设置
//#define SI5351A_SDA_IN()  {GPIOB->MODER&=~(3<<(14*2));GPIOB->MODER|=0<<14*2;}	//PB14设置输入;
//#define SI5351A_SDA_OUT() {GPIOB->MODER&=~(3<<(14*2));GPIOB->MODER|=1<<14*2;} 	//PB14设置为输出;
//#define SI5351A_SCL_IN()  {GPIOC->MODER&=~(3<<(8*2));GPIOB->MODER|=0<<8*2;}		//PC8设置为输入;
////IO操作宏定义	 
//#define SI5351A_IIC_SCL(x)    GPIO_Pin_Set(GPIOC,PIN8,x)	//输出SCL
//#define SI5351A_READ_SCL	  GPIO_Pin_Get(GPIOC,PIN8)    	//输入SCL

//#define SI5351A_IIC_SDA(x)    GPIO_Pin_Set(GPIOB,PIN14,x)	//输出SDA	 
//#define SI5351A_READ_SDA  	  GPIO_Pin_Get(GPIOB,PIN14)   	//输入SDA 


//IIC所有操作函数
//void SI5351A_IIC_Init(void);                //初始化IIC的IO口				 
//void SI5351A_IIC_Start(void);				//发送IIC开始信号
//void SI5351A_IIC_Stop(void);	  			//发送IIC停止信号
//void SI5351A_IIC_Send_Byte(u8 txd);			//IIC发送一个字节
//u8 SI5351A_IIC_Read_Byte(u8 ack);//IIC读取一个字节
//u8 SI5351A_IIC_Wait_Ack(void); 				//IIC等待ACK信号
//void SI5351A_IIC_Ack(void);					//IIC发送ACK信号
//void SI5351A_IIC_NAck(void);				//IIC不发送ACK信号

//void SI5351A_IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
//u8 SI5351A_IIC_Read_One_Byte(u8 daddr,u8 addr);	
void SI5351A_Init(void);

#define SI_CLK0_CONTROL	16			// Register definitions
#define SI_CLK1_CONTROL	17
#define SI_CLK2_CONTROL	18
#define SI_SYNTH_PLL_A	26
#define SI_SYNTH_PLL_B	34
#define SI_SYNTH_MS_0		42
#define SI_SYNTH_MS_1		50
#define SI_SYNTH_MS_2		58
/*	初始相位设置 */
#define SI5351_CLK0_PHASE_OFFSET			165
#define SI5351_CLK1_PHASE_OFFSET			166
#define SI5351_CLK2_PHASE_OFFSET			167
#define SI_PLL_RESET		177

#define SI_R_DIV_1		0x00			// R-division ratio definitions
#define SI_R_DIV_2		0x10
#define SI_R_DIV_4		0x20
#define SI_R_DIV_8		0x30
#define SI_R_DIV_16		0x40
#define SI_R_DIV_32		0x50
#define SI_R_DIV_64		0x60
#define SI_R_DIV_128	0x70

#define SI_CLK_SRC_PLL_A	0x00
#define SI_CLK_SRC_PLL_B	0x20

#define XTAL_FREQ	25000000
// Crystal frequency

void si5351aOutputOff(uint8_t clk);
void si5351aSetFrequency_LoA(u32 Xtalfreq,uint32_t frequency);
void si5351aSetFrequency_Bfo(u32 Xtalfreq,uint32_t frequency);
void si5351aSetFrequency_FFT(uint32_t bfo,uint32_t fft);

#endif //SI5351A_H
