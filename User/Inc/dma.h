#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//DMA 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/13
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 
 

void ADC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);//配置DMAx_CHx
void ADC3_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);//
void DAC1_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);//
void DAC2_DMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);
void USART1_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);
void USART3_TXDMA_Config(DMA_Stream_TypeDef *DMA_Streamx,u8 chx);
void MYDMA_Enable(DMA_Stream_TypeDef *DMA_Streamx,u16 ndtr);	//使能一次DMA传输		   
#endif






























