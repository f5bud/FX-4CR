#include "sys.h"
#include "usart.h"
#include "dma.h"
#include "cat_kenwood.h"
////////////////////////////////////////////////////////////////////////////////// 	 
#define IC_706 0x48
#define CIV_ADDR IC_706

unsigned char UartRxLen = 0;
unsigned char UartRxBuf[32];
static UartTxBufDef UTBuffer;
 u8 DMA_IsReady = 1;
u8 DMA3_IsReady = 1;
__attribute__((section (".RAM_D1"))) uint8_t Uart_Send_Buffer[UART_TX_BUF_MAX_SIZE];
__attribute__((section (".RAM_D1"))) uint8_t Uart3_Send_Buffer[UART_TX_BUF_MAX_SIZE];
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//解决HAL库使用时,某些情况可能报错的bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef’ d in stdio.h. */ 
FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
	USART1->TDR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

void UartBufInit()
{
  UTBuffer.head = 0;
  UTBuffer.rear = 0;
  UTBuffer.size	= 0;
}

void UartTxBufWrite(unsigned char wdat)
{
	if(UTBuffer.size < UART_TX_BUF_MAX_SIZE)
	{
		UTBuffer.dat[ UTBuffer.rear++ ] = wdat;
		if(UTBuffer.rear >= UART_TX_BUF_MAX_SIZE)
		{
		  UTBuffer.rear = 0;
		}
		UTBuffer.size ++;	
	}
}

void UartTx(uint8_t *wdat, uint8_t len)
{
	for(uint8_t i=0; i<len; i++)
	{
		UartTxBufWrite(wdat[i]);
	}
}

unsigned char UartTxBufRead()
{
	unsigned char rval = 0;
	if(UTBuffer.size > 0)
	{
		rval = UTBuffer.dat[ UTBuffer.head++ ];
		if(UTBuffer.head >= UART_TX_BUF_MAX_SIZE)
		{
		  UTBuffer.head = 0;
		}							
		UTBuffer.size --; 
	}
  
  return rval;
}

unsigned char UART_TxBufSize()
{
	return UTBuffer.size;
}

void usart_rx_analysis(uint8_t wdat)
{
	if(wdat == ';')
	{
		Kenwood_CmdParser(UartRxBuf, UartRxLen);
		UartRxLen = 0;
	}
	else
	{
		UartRxBuf[ UartRxLen] = wdat;
		UartRxLen++;
		if(UartRxLen > USART_RX_LEN-1 )UartRxLen = 0;  
	}
}
void usart3_rx_analysis(uint8_t wdat)
{
	if(wdat == ';')
	{
		Kenwood_CmdParser(UartRxBuf, UartRxLen);
		UartRxLen = 0;
	}
	else
	{
		UartRxBuf[ UartRxLen ] = wdat;
		UartRxLen++;
		if(UartRxLen >USART_RX_LEN -1)UartRxLen = 0;  
	}
}
u8 DMA_IsTxComplete()
{
	return DMA_IsReady;
}

void CopyTBufToDMA()
{
	uint8_t len = UTBuffer.size;
	for(uint8_t i=0; i<len; i++)
	{
		Uart_Send_Buffer[i] = UartTxBufRead();
	}
}
void UART_DMAStart(uint8_t length)
{
	DMA_IsReady = 0;
	USART1->CR3=1<<7;
	MYDMA_Enable(DMA1_Stream7,length);
}
void UART_DMAEnd()
{
	if(DMA1->HISR&(1<<27))	//等待DMA2_Steam7传输完成
	{
		DMA1->HIFCR|=1<<27;	//清除DMA2_Steam7传输完成标志
	
		//允许再次发送 
		DMA_IsReady = 1;
		DMA1_Stream7->CR&=~(1<<0); 	//关闭DMA传输 
	}
}
//
void UART_Task()
{
	uint8_t size = 0;
	if (DMA_IsReady)
	{
		size = UART_TxBufSize(); 
		if(size > 0)
		{
			// 串口发送处理
			CopyTBufToDMA();
			UART_DMAStart( size );	
		}
	}
	//UART_DMAEnd();
}
////串口1中断服务程序
////注意,读取USARTx->SR能避免莫名其妙的错误   	
//u8 USART_RX_BUF[USART_RX_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
////接收状态
////bit15，	接收完成标志
////bit14，	接收到0x0d
////bit13~0，	接收到的有效字节数目
//u16 USART_RX_STA=0;       //接收状态标记	 
  
void USART1_IRQHandler(void)
{
	u8 res;	

	if(USART1->ISR&(1<<5))//接收到数据
	{	 
		res = USART1->RDR; 
		usart_rx_analysis(res);	
		//UART_Task();
	} 
	USART1->ICR|=1<<3;	//清除溢出错误,否则可能会卡死在串口中断服务函数里面
} 
/*
****************************************************************************
*	蓝牙传输 串口3
****************************************************************************
*/
void Uart3_CopyTBufToDMA()
{
  uint8_t len = UTBuffer.size;
  for(uint8_t i=0; i<len; i++)
  {
    Uart3_Send_Buffer[i] = UartTxBufRead();
  }
}
void UART3_DMAStart(uint8_t length)
{
	DMA3_IsReady = 0;
	USART3->CR3	=1<<7;           //使能串口1的DMA发送 
	MYDMA_Enable(DMA1_Stream6,length);
}
void UART3_DMAEnd()
{
	if(DMA1->HISR&(1<<21))	//等待DMA2_Steam7传输完成
	{
		DMA1->HIFCR|=1<<21;	//清除DMA2_Steam7传输完成标志
	
		//允许再次发送 
		DMA3_IsReady = 1;
		DMA1_Stream6->CR&=~(1<<0); 	//关闭DMA传输 
	}
}
//
void UART3_Task()
{
	uint8_t size = 0;
	if (DMA3_IsReady)
	{
		size = UART_TxBufSize(); 
		if(size > 0)
		{
			// 串口发送处理
			//UART3_DMAStart( size );	
			Uart3_CopyTBufToDMA();
			UART3_DMAStart( size );	
		}
	}
	//UART_DMAEnd();
}
//
void USART3_IRQHandler(void)
{
	u8 res;	

	if(USART3->ISR&(1<<5))//接收到数据
	{	 
		res = USART3->RDR; 
		usart3_rx_analysis(res);	
		//UART3_Task();
	} 
	USART3->ICR|=1<<3;	//清除溢出错误,否则可能会卡死在串口中断服务函数里面
} 
//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//注意:USART1的时钟,是可以通过RCC_D2CCIP2R选择的
//但是我们一般用默认的选择即可(默认选择rcc_pclk2作为串口1时钟)
//pclk2即APB2的时钟频率,一般为100Mhz
//bound:波特率 
void uart1_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;
	
	UartBufInit();
	
	temp=(pclk2*1000000+bound/2)/bound;	//得到USARTDIV@OVER8=0,采用四舍五入计算
	RCC->AHB4ENR|=1<<0;   	//使能PORTA口时钟  
	RCC->APB2ENR|=1<<4;  	//使能串口1时钟 
	GPIO_Set(GPIOA,PIN9|PIN10,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOA,9,7);	//PA9,AF7
	GPIO_AF_Set(GPIOA,10,7);//PA10,AF7  	   
	//波特率设置
 	USART1->BRR=temp; 		//波特率设置@OVER8=0 	
	USART1->CR1=0;		 	//清零CR1寄存器
	USART1->CR1|=0<<28;	 	//设置M1=0
	USART1->CR1|=0<<12;	 	//设置M0=0&M1=0,选择8位字长 
	USART1->CR1|=0<<15; 	//设置OVER8=0,16倍过采样 
	USART1->CR1|=1<<3;  	//串口发送使能 
	USART1->CR1|=1<<2;  	//串口接收使能
	USART1->CR1|=1<<5;    	//接收缓冲区非空中断使能	
	
	USART1->CR2=0;		 	//清零CR1寄存器
	USART1->CR2|=0<<11;	 	//串口时钟禁止
	USART1->CR2|=0<<10;	 	//数据低电平有效
	USART1->CR2|=1<<9; 	//配置CPHA使数据在第2个边沿的时候被捕获 
	USART1->CR2|=0<<8;  	//禁用最后一位,使对应的时钟脉冲不会再输出到SCLK引脚 
	//USART1->CR3=1<<7;           //使能串口1的DMA发送   
	MY_NVIC_Init(14,0,USART1_IRQn,4);//组2，最低优先级 
	USART1->CR1|=1<<0;  	//串口使
	USART1_TXDMA_Config(DMA1_Stream7,42);//DMA2,STEAM7,
	MY_NVIC_Init(14,0,DMA1_Stream7_IRQn,4);//抢占2，子优先级0，组2
}

void DMA1_Stream7_IRQHandler (void) 
{ 
	
	if(DMA1->HISR&(1<<27))	//等待DMA1_Steam7传输完成
	{
		DMA1->HIFCR|=1<<27;	//清除DMA1_Steam7传输完成标志
		DMA_IsReady = 1;
		//USART1->ICR|=1<<6;
		DMA1_Stream7->CR&=~(1<<0); 	//关闭DMA传输  
	}
	
}

//初始化IO 串口1
//pclk2:PCLK2时钟频率(Mhz)
//注意:USART1的时钟,是可以通过RCC_D2CCIP2R选择的
//但是我们一般用默认的选择即可(默认选择rcc_pclk2作为串口1时钟)
//pclk2即APB2的时钟频率,一般为100Mhz
//bound:波特率 
void uart3_init(u32 pclk2,u32 bound)
{  	 
	u32	temp;
	
	UartBufInit();
	
	temp=(pclk2*1000000+bound/2)/bound;	//得到USARTDIV@OVER8=0,采用四舍五入计算
	RCC->AHB4ENR|=1<<3;   	//使能PORTA口时钟  
	RCC->APB1LENR|=1<<18;  	//使能串口3时钟 
	GPIO_Set(GPIOD,PIN8|PIN9,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//PA9,PA10,复用功能,上拉输出
 	GPIO_AF_Set(GPIOD,8,7);	//PA9,AF7
	GPIO_AF_Set(GPIOD,9,7);//PA10,AF7  	   
	//波特率设置
 	USART3->BRR=temp; 		//波特率设置@OVER8=0 	
	USART3->CR1=0;		 	//清零CR1寄存器
	USART3->CR1|=0<<28;	 	//设置M1=0
	USART3->CR1|=0<<12;	 	//设置M0=0&M1=0,选择8位字长 
	USART3->CR1|=0<<15; 	//设置OVER8=0,16倍过采样 
	USART3->CR1|=1<<3;  	//串口发送使能 
	USART3->CR1|=1<<2;  	//串口接收使能
	USART3->CR1|=1<<5;    	//接收缓冲区非空中断使能	
	
	USART3->CR2=0;		 	//清零CR1寄存器
	USART3->CR2|=0<<11;	 	//串口时钟禁止
	USART3->CR2|=0<<10;	 	//数据低电平有效
	USART3->CR2|=1<<9; 	//配置CPHA使数据在第2个边沿的时候被捕获 
	USART3->CR2|=0<<8;  	//禁用最后一位,使对应的时钟脉冲不会再输出到SCLK引脚 
	USART3->ICR|=1<<6;
	//USART1->CR3=1<<7;           //使能串口1的DMA发送   
	MY_NVIC_Init(14,0,USART3_IRQn,4);//组2，最低优先级 
	//USART3->CR1|=1<<0;  	//串口使
	USART3_TXDMA_Config(DMA1_Stream6,46);//DMA2,STEAM7,
	MY_NVIC_Init(14,0,DMA1_Stream6_IRQn,4);//抢占2，子优先级0，组2
}

void DMA1_Stream6_IRQHandler (void) 
{ 
	
	if(DMA1->HISR&(1<<21))	//等待DMA1_Steam6传输完成
	{
		DMA1->HIFCR|=1<<21;	//清除DMA1_Steam6传输完成标志
		DMA3_IsReady = 1;
		DMA1_Stream6->CR&=~(1<<0); 	//关闭DMA传输  
	}
	
}









