#ifndef __USART_H
#define __USART_H 
#include "sys.h"
#include "stdio.h"	  
//////////////////////////////////////////////////////////////////////////////////	   
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//串口1初始化 
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/6
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明  
//V1.1 20180815
//在USART1_IRQHandler函数添加清除溢出错误操作,防止由于接收溢出出现死机的bug
////////////////////////////////////////////////////////////////////////////////// 

#define USART_RX_LEN  			32  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收

#define UART_TX_BUF_MAX_SIZE	64
extern u8 DMA_IsReady ;
typedef struct{
	unsigned char head;
	unsigned char rear;
	unsigned char size;
	unsigned char dat[UART_TX_BUF_MAX_SIZE];
}UartTxBufDef;

extern uint8_t Uart_Send_Buffer[UART_TX_BUF_MAX_SIZE];
extern u8  USART_RX_BUF[USART_RX_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	

void uart1_init(u32 pclk2,u32 bound);
void uart3_init(u32 pclk2,u32 bound);

u8 DMA_IsTxComplete(void);
void CopyTBufToDMA(void);
void UART_DMAStart(uint8_t length);
void UartTx(uint8_t *wdat, uint8_t len);
unsigned char UART_TxBufSize(void);
void UART_DMAEnd(void);
void UART_Task(void);
void UART3_Task(void);
void UART3_DMAEnd(void);
#endif	   
















