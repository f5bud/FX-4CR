#include "dma.h"																	   	  
#include "dwt.h"
#include "Audio_driver.h"
/*
******************************************************************************	 
* 	《本程序只供学习使用，未经作者许可，不得用于其它任何用途》						 *
* 	使用：FX-4C																 *
* 	功能：DMA 驱动代码														 *   
* 	作者：BG2IXD																 *
* 	创建日期:2020/9/18														 *
* 	版本：V1.0																 *
* 				《版权所有，盗版必究》											 *									  
******************************************************************************
*/
extern  __attribute__((section (".RAM_D1"))) uint8_t Uart_Send_Buffer[64];
extern  __attribute__((section (".RAM_D1"))) uint8_t Uart3_Send_Buffer[64];
extern  __attribute__((section (".RAM_D1"))) u16 ADC1_Buff0[SAMPLE_SIZE];
extern  __attribute__((section (".RAM_D1"))) u16 ADC1_Buff1[SAMPLE_SIZE];
extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff0[SAMPLE_SIZE ];
extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff1[SAMPLE_SIZE ];
extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff0[SAMPLE_SIZE];	
extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff1[SAMPLE_SIZE];
extern  __attribute__((section (".RAM_D1"))) u16 DAC2_Buff0[SAMPLE_SIZE/2];	
extern  __attribute__((section (".RAM_D1"))) u16 DAC2_Buff1[SAMPLE_SIZE/2];
/*
******************************************************************************************************
* DMA1数据流（7）的（9）通道配置
* 这里用作ADC1的数据传输
* 从外设->存储器模式/16位数据宽度/存储器增量模式/双缓冲模式
* DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7；这里选Stream7
* chx:DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)，
* ADC1是第9通道；
* par:外设地址: ADC1->DR
* mar:存储器地址: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:数据传输量 
******************************************************************************************************
*/
void ADC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2时钟使能  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;							//清空之前该stream上的所有中断标志

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//如果是DMA2,通道编号+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//得到对应的DMAMUX通道控制地址
	DMAMUXx->CCR=chx&0XFF;		//通道选择
	
	DMA_Streamx->PAR=(u32)&ADC1->DR;	//DMA外设地址
	DMA_Streamx->M0AR=(u32)&ADC1_Buff0;		//DMA 存储器0地址
	DMA_Streamx->M1AR=(u32)&ADC1_Buff1;		//DMA 存储器1地址
	DMA_Streamx->NDTR=SAMPLE_SIZE;		//传输数据的数目
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	//DMA_Streamx->CR|=1<<4;	//传输完成中断使能
	DMA_Streamx->CR|=0<<5;		//0：DMA 是流控制器
	DMA_Streamx->CR|=0<<6;		//外设到存储器模式
	DMA_Streamx->CR|=1<<8;		//循环模式(即使用双缓冲模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=1<<11;		//外设数据长度; 0:8位,1:16位
	DMA_Streamx->CR|=1<<13;		//存储器数据长度:16位
	DMA_Streamx->CR|=0<<16;		//低等优先级
	DMA_Streamx->CR|=1<<18;		//0：传输结束时不切换缓冲区；1：DMA 传输结束时切换目标存储区
	//DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	//DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	 
	DMA_Streamx->FCR=0X21;	//FIFO控制寄存器
	
	//MY_NVIC_Init(0,15,DMA1_Stream0_IRQn,0);//抢占2，子优先级0，组2	
	
	//DMA_Streamx->CR|=1<<0;		//开启DMA传输
}
/*
********************************************************************************************
* DMA1数据流（6）的（115）通道配置
* 这里用作ADC3的数据传输
* 从外设->存储器模式/16位数据宽度/存储器增量模式/双缓冲模式
* DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7；这里选Stream7
* chx:DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)，
* ADC3是第115通道；
* par:外设地址: ADC3->DR
* mar:存储器地址: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:数据传输量 
*********************************************************************************************
*/
void ADC3_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2时钟使能  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;							//清空之前该stream上的所有中断标志

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//如果是DMA2,通道编号+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//得到对应的DMAMUX通道控制地址
	DMAMUXx->CCR=chx&0XFF;		//通道选择
	
	DMA_Streamx->PAR=(u32)&ADC3->DR;	//DMA外设地址
	DMA_Streamx->M0AR=(u32)&ADC_Buff0;		//DMA 存储器0地址
	DMA_Streamx->M1AR=(u32)&ADC_Buff1;		//DMA 存储器1地址
	DMA_Streamx->NDTR=SAMPLE_SIZE;		//传输数据的数目
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	DMA_Streamx->CR|=1<<4;		//传输完成中断使能
	DMA_Streamx->CR|=0<<5;		//0：DMA 是流控制器
	DMA_Streamx->CR|=0<<6;		//外设到存储器模式
	DMA_Streamx->CR|=1<<8;		//循环模式(即使用双缓冲模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=1<<11;		//外设数据长度; 0:8位,1:16位
	DMA_Streamx->CR|=1<<13;		//存储器数据长度:16位
	//DMA_Streamx->CR|=1<<15;
	DMA_Streamx->CR|=2<<16;		//中等优先级
	DMA_Streamx->CR|=1<<18;		//0：传输结束时不切换缓冲区；1：DMA 传输结束时切换目标存储区
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	 
	DMA_Streamx->FCR=0X21;		//FIFO控制寄存器
	
	MY_NVIC_Init(0,12,DMA2_Stream0_IRQn,0);//抢占2，子优先级0，组2	
	//DMA_Streamx->CR|=1<<0;		//开启DMA传输
}
/*
********************************************************************************************
* DMA1数据流（5）的（67）通道配置
* 这里用作DAC1的数据传输
* 从存储器外设->外设模式/16位数据宽度/存储器增量模式/双缓冲模式
* DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7；这里选Stream5
* chx:DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)，
*     DAC1是第67通道；
* par:外设地址: DAC->DHR12R1
* mar:存储器地址: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:数据传输量 
*********************************************************************************************
*/
void DAC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2时钟使能  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;							//清空之前该stream上的所有中断标志

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//如果是DMA2,通道编号+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//得到对应的DMAMUX通道控制地址
	DMAMUXx->CCR=chx&0XFF;		//通道选择
	
	DMA_Streamx->PAR=(u32)&DAC1->DHR12R1;	//DMA外设地址
	DMA_Streamx->M0AR=(u32)&DAC2_Buff0 ;		//DMA 存储器0地址
	DMA_Streamx->M1AR=(u32)&DAC2_Buff1 ;		//DMA 存储器1地址
	DMA_Streamx->NDTR=SAMPLE_SIZE/2;		//传输数据的数目
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	//DMA_Streamx->CR|=1<<4;		//传输完成中断使能
	DMA_Streamx->CR|=0<<5;		//0：DMA 是流控制器
	DMA_Streamx->CR|=1<<6;		//存储器到外设模式
	DMA_Streamx->CR|=1<<8;		//循环模式(即使用双缓冲模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=1<<11;		//外设数据长度; 0:8位,1:16位
	DMA_Streamx->CR|=1<<13;		//存储器数据长度:16位
	DMA_Streamx->CR|=1<<16;		//中等优先级
	DMA_Streamx->CR|=1<<18;		//0：传输结束时不切换缓冲区；1：DMA 传输结束时切换目标存储区
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	 
	DMA_Streamx->FCR=0X21;		//FIFO控制寄存器
	
	//MY_NVIC_Init(2,0,DMA1_Stream5_IRQn,0);//抢占2，子优先级0，组2	
	//DMA_Streamx->CR|=1<<0;		//开启DMA传输
} 
/*
********************************************************************************************
* DMA1数据流（6）的（68）通道配置
* 这里用作DAC1的数据传输
* 从存储器外设->外设模式/16位数据宽度/存储器增量模式/双缓冲模式
* DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7；这里选Stream6
* chx:DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)，
*     DAC2是第68通道；
* par:外设地址: DAC->DHR12R2
* mar:存储器地址: DMA_Streamx->M0AR,DMA_Streamx->M1AR;
* ndtr:数据传输量 
*********************************************************************************************
*/
void DAC2_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2时钟使能  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;							//清空之前该stream上的所有中断标志

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//如果是DMA2,通道编号+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//得到对应的DMAMUX通道控制地址
	DMAMUXx->CCR=chx&0XFF;		//通道选择
	
	DMA_Streamx->PAR=(u32)&DAC1->DHR12R2;	//DMA外设地址
	DMA_Streamx->M0AR=(u32)&DAC2_Buff0 ;		//DMA 存储器0地址
	DMA_Streamx->M1AR=(u32)&DAC2_Buff1 ;		//DMA 存储器1地址
	DMA_Streamx->NDTR=SAMPLE_SIZE;		//传输数据的数目
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	//DMA_Streamx->CR|=1<<4;		//传输完成中断使能
	DMA_Streamx->CR|=0<<5;		//0：DMA 是流控制器
	DMA_Streamx->CR|=1<<6;		//存储器到外设模式
	DMA_Streamx->CR|=1<<8;		//循环模式(即使用双缓冲模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=1<<11;		//外设数据长度; 0:8位,1:16位
	DMA_Streamx->CR|=1<<13;		//存储器数据长度:16位
	DMA_Streamx->CR|=1<<16;		//中等优先级
	DMA_Streamx->CR|=1<<18;		//0：传输结束时不切换缓冲区；1：DMA 传输结束时切换目标存储区
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	 
	DMA_Streamx->FCR=0X21;		//FIFO控制寄存器
	
	//MY_NVIC_Init(2,0,DMA1_Stream5_IRQn,0);//抢占2，子优先级0，组2	
	//DMA_Streamx->CR|=1<<0;		//开启DMA传输
}
//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量 

void USART1_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2时钟使能  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;							//清空之前该stream上的所有中断标志

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//如果是DMA2,通道编号+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//得到对应的DMAMUX通道控制地址
	DMAMUXx->CCR=chx&0XFF;		//通道选择
	
	DMA_Streamx->PAR=(u32)&USART1->TDR;		//DMA外设地址
	DMA_Streamx->M0AR=(u32)&Uart_Send_Buffer;		//DMA 存储器0地址
	DMA_Streamx->NDTR=64;		//DMA 数据传输量
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	DMA_Streamx->CR|=1<<4;		//传输完成中断使能
	DMA_Streamx->CR|=1<<6;		//存储器到外设模式
	DMA_Streamx->CR|=0<<8;		//非循环模式(即使用普通模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=0<<11;		//外设数据长度:8位
	DMA_Streamx->CR|=0<<13;		//存储器数据长度:8位
	DMA_Streamx->CR|=1<<16;		//中等优先级
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	 
	//DMA_Streamx->FCR=0X21;	//FIFO控制寄存器
	//MY_NVIC_Init(15,0,DMA2_Stream5_IRQn,4);//抢占2，子优先级0，组2
} 
//开启一次DMA传输
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7 
//ndtr:数据传输量  
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr)
{
	while(DMA_Streamx->CR&0X1);	//确保DMA可以被设置  
	DMA_Streamx->NDTR=ndtr;		//DMA 存储器0地址 
	DMA_Streamx->CR|=1<<0;		//开启DMA传输
}	  
//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,范围:1~115(详见<<STM32H7xx参考手册>>16.3.2节,Table 116)
//par:外设地址
//mar:存储器地址
//ndtr:数据传输量 

void USART3_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx)
{ 
	DMA_TypeDef *DMAx;
	DMAMUX_Channel_TypeDef *DMAMUXx;
	u8 streamx;
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
		DMAx=DMA2;
		RCC->AHB1ENR|=1<<1;		//DMA2时钟使能  
	}else 
	{
		DMAx=DMA1; 
 		RCC->AHB1ENR|=1<<0;		//DMA1时钟使能 
	}
	while(DMA_Streamx->CR&0X01);//等待DMA可配置 
	streamx=(((u32)DMA_Streamx-(u32)DMAx)-0X10)/0X18;			//得到stream通道号
 	if(streamx>=6)DMAx->HIFCR|=0X3D<<(6*(streamx-6)+16);		//清空之前该stream上的所有中断标志
	else if(streamx>=4)DMAx->HIFCR|=0X3D<<6*(streamx-4);		//清空之前该stream上的所有中断标志
	else if(streamx>=2)DMAx->LIFCR|=0X3D<<(6*(streamx-2)+16);	//清空之前该stream上的所有中断标志
	else DMAx->LIFCR|=0X3D<<6*streamx;							//清空之前该stream上的所有中断标志

	if((u32)DMA_Streamx>(u32)DMA2)streamx+=8;					//如果是DMA2,通道编号+8 
	DMAMUXx=(DMAMUX_Channel_TypeDef *)(DMAMUX1_BASE+streamx*4);	//得到对应的DMAMUX通道控制地址
	DMAMUXx->CCR=chx&0XFF;		//通道选择
	
	DMA_Streamx->PAR=(u32)&USART3->TDR;		//DMA外设地址
	DMA_Streamx->M0AR=(u32)&Uart3_Send_Buffer;		//DMA 存储器0地址
	DMA_Streamx->NDTR=64;		//DMA 数据传输量
	DMA_Streamx->CR=0;			//先全部复位CR寄存器值 
	
	DMA_Streamx->CR|=1<<4;		//传输完成中断使能
	DMA_Streamx->CR|=1<<6;		//存储器到外设模式
	DMA_Streamx->CR|=0<<8;		//非循环模式(即使用普通模式)
	DMA_Streamx->CR|=0<<9;		//外设非增量模式
	DMA_Streamx->CR|=1<<10;		//存储器增量模式
	DMA_Streamx->CR|=0<<11;		//外设数据长度:8位
	DMA_Streamx->CR|=0<<13;		//存储器数据长度:8位
	DMA_Streamx->CR|=1<<16;		//中等优先级
	DMA_Streamx->CR|=0<<21;		//外设突发单次传输
	DMA_Streamx->CR|=0<<23;		//存储器突发单次传输
	 
	//DMA_Streamx->FCR=0X21;	//FIFO控制寄存器
	//MY_NVIC_Init(15,0,DMA2_Stream5_IRQn,4);//抢占2，子优先级0，组2
}


























