#include "sai.h" 
#include "Audio_driver.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//SAI 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/15
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved	
//********************************************************************************
//升级说明
//V1.1 20180817
//1,新增SAIB_Init、SAIA_RX_DMA_Init、SAI_Rec_Start和SAI_Rec_Stop等函数	
//2,新增:sai_ltdc_clkset函数,用于设置LTDC时钟,确保在修改pll3n的时候,不改变LTDC的像素时钟
////////////////////////////////////////////////////////////////////////////////// 	
//extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff0[SAMPLE_SIZE ];
//extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff1[SAMPLE_SIZE ];
//extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff0[SAMPLE_SIZE];	
//extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff1[SAMPLE_SIZE];
 
//SAI Block A初始化,I2S,飞利浦标准
//mode:工作模式,00,主发送器;01,主接收器;10,从发送器;11,从接收器
//cpol:0,时钟下降沿选通;1,时钟上升沿选通
//datalen:数据大小,0/1,未用到,2,8位;3,10位;4,16位;5,20位;6,24位;7,32位.
void SAIA_Init(u8 mode,u8 cpol,u8 datalen)
{ 
	u32 tempreg=0;
	RCC->D2CCIP1R&=~(7<<0);		//SAI1SEL[2:0]清零
	RCC->D2CCIP1R|=2<<0;		//SAI1SEL[2:0]=2,选择pll3_p_ck作为SAI1的时钟源
	RCC->APB2ENR|=1<<22;		//使能SAI1时钟
	RCC->APB2RSTR|=1<<22;		//复位SAI1
	RCC->APB2RSTR&=~(1<<22);	//结束复位  
	tempreg|=mode<<0;			//设置SAI1工作模式
	tempreg|=0<<2;				//设置SAI1协议为:自由协议(支持I2S/LSB/MSB/TDM/PCM/DSP等协议)
	tempreg|=datalen<<5;		//设置数据大小
	tempreg|=0<<8;				//数据MSB位优先
	tempreg|=(u16)cpol<<9;		//数据在时钟的上升/下降沿选通
	tempreg|=0<<10;				//音频模块同步
	tempreg|=0<<12;				//立体声模式
	tempreg|=1<<13;				//立即驱动音频模块输出
	tempreg|=0<<19;				//使能主时钟分频器(MCKDIV)
	SAI1_Block_A->CR1=tempreg;	//设置CR1寄存器
	
	tempreg=(64-1)<<0;			//设置帧长度为64,左通道32个SCK,右通道32个SCK.
	tempreg|=(32-1)<<8;			//设置帧同步有效电平长度,在I2S模式下=1/2帧长.
	tempreg|=1<<16;				///FS信号为SOF信号+通道识别信号
	tempreg|=0<<17;				///FS低电平有效(下降沿)
	tempreg|=1<<18;				//在slot0的第一位的前一位使能FS,以匹配飞利浦标准	 
	SAI1_Block_A->FRCR=tempreg;
	
	tempreg=0<<0;				//slot偏移(FBOFF)为0
	tempreg|=2<<6;				//slot大小为32位
	tempreg|=(2-1)<<8;			//slot数为2个
	tempreg|=(1<<17)|(1<<16);	//使能slot0和slot1
	SAI1_Block_A->SLOTR=tempreg;//设置slot 
	
	SAI1_Block_A->CR2=1<<0;		//设置FIFO阀值:1/4 FIFO  
	SAI1_Block_A->CR2|=1<<3;	//FIFO刷新 
} 

//SAI Block B初始化,I2S,飞利浦标准,同步模式
//mode:工作模式,00,主发送器;01,主接收器;10,从发送器;11,从接收器
//cpol:0,时钟下降沿选通;1,时钟上升沿选通
//datalen:数据大小,0/1,未用到,2,8位;3,10位;4,16位;5,20位;6,24位;7,32位.
void SAIB_Init(u8 mode,u8 cpol,u8 datalen)
{ 
	u32 tempreg=0;  
	tempreg|=mode<<0;			//设置SAI1工作模式
	tempreg|=0<<2;				//设置SAI1协议为:自由协议(支持I2S/LSB/MSB/TDM/PCM/DSP等协议)
	tempreg|=datalen<<5;		//设置数据大小
	tempreg|=0<<8;				//数据MSB位优先
	tempreg|=(u16)cpol<<9;		//数据在时钟的上升/下降沿选通
	tempreg|=1<<10;				//使能异步模式
	tempreg|=0<<12;				//立体声模式
	tempreg|=1<<13;				//立即驱动音频模块输出
	SAI1_Block_B->CR1=tempreg;	//设置CR1寄存器
	
	tempreg=(64-1)<<0;			//设置帧长度为64,左通道32个SCK,右通道32个SCK.
	tempreg|=(32-1)<<8;			//设置帧同步有效电平长度,在I2S模式下=1/2帧长.
	tempreg|=1<<16;				///FS信号为SOF信号+通道识别信号
	tempreg|=0<<17;				///FS低电平有效(下降沿)
	tempreg|=1<<18;				//在slot0的第一位的前一位使能FS,以匹配飞利浦标准	 
	SAI1_Block_B->FRCR=tempreg;
	
	tempreg=0<<0;				//slot偏移(FBOFF)为0
	tempreg|=2<<6;				//slot大小为32位
	tempreg|=(2-1)<<8;			//slot数为2个
	tempreg|=(1<<17)|(1<<16);	//使能slot0和slot1
	SAI1_Block_B->SLOTR=tempreg;//设置slot 
	
	SAI1_Block_B->CR2=1<<0;		//设置FIFO阀值:1/4 FIFO  
	SAI1_Block_B->CR2|=1<<3;	//FIFO刷新  
	SAI1_Block_B->CR1|=1<<17;	//使能DMA
	SAI1_Block_B->CR1|=1<<16;	//使能SAI1 Block B
} 

//SAI Block A采样率设置
//采样率计算公式(以NOMCK=0,OSR=0为前提):
//Fmclk=256*Fs=sai_x_ker_ck/MCKDIV[5:0]
//Fs=sai_x_ker_ck/(256*MCKDIV[5:0])
//Fsck=Fmclk*(FRL[7:0]+1)/256=Fs*(FRL[7:0]+1)
//其中:
//sai_x_ker_ck=(HSE/PLL3DIVM)*(PLL3DIVN+1)/(PLL3DIVP+1)
//HSE:一般为25Mhz
//PLL3DIVM:1~63,表示1~63分频
//PLL3DIVN:3~511,表示4~512倍频 
//PLL3DIVP:0~127,表示1~128分频 
//MCKDIV:0~63,表示1~63分频,0也是1分频,推荐设置为偶数
//SAI A分频系数表@PLL3DIVM=25,HSE=25Mhz,即vco输入频率为1Mhz 
//表格式:
//采样率|(PLL3DIVN+1)|(PLL3DIVP+1)|MCKDIV
const u16 SAI_PSC_TBL[][4]=
{
	{800 ,256,5,25},	//8Khz采样率
	{1102,302,107,0},	//11.025Khz采样率 
	{1600,426,2,52},	//16Khz采样率
	{2205,429,38,2},	//22.05Khz采样率
	{3200,426,1,52},	//32Khz采样率
	{4410,429,1,38},	//44.1Khz采样率
	{4800,467,1,38},	//48Khz采样率
	{8820,429,1,19},	//88.2Khz采样率
	{9600,467,1,19},	//96Khz采样率
	{17640,271,1,6},	//176.4Khz采样率 
	{19200,295,6,0},	//192Khz采样率
};
#if daniel
/////////////////////////////////////////////////////////////////////////////////////
#include "ltdc.h"
//分辨率:像素时钟对应表,前面为横向分辨率,后面为目标像素时钟频率
const u16 ltdc_clk_table[4][2]=
{
	480,9,
	800,33,
	1024,50,
	1280,50,
};
//专门针对LTDC的一个时钟设置函数
//pll3n,当前的pll3倍频系数.
//void sai_ltdc_clkset(u16 pll3n)
//{
//	u8 i=0;
//	u8 pll3r=0;
//	if(lcdltdc.pwidth==0)return;	//不是RGB屏,不需要设置
//	for(i=0;i<4;i++)				//查找是否有对应RGB屏的分辨率
//	{
//		if(lcdltdc.pwidth==ltdc_clk_table[i][0])break;	//找到了则直接退出
//	}
//	if(i==4)return;					//找遍了也没找到对应的分辨率
//	pll3r=pll3n/ltdc_clk_table[i][1];//得到计算后的pll3r的值
//	if(pll3n>(pll3r*ltdc_clk_table[i][1]))pll3r+=1;	
//	//LTDC_Clk_Set(pll3n,25,pll3r);	//重新设置PLL3的R分频.
//}
/////////////////////////////////////////////////////////////////////////////////////
#endif
//设置SAIA的采样率(@NOMCK=0,OSR=0)
//samplerate:采样率,单位:Hz
//返回值:0,设置成功;1,无法设置.
u8 SAIA_SampleRate_Set(u32 samplerate)
{ 
	u16 retry=0; 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//缩小10倍    
	for(i=0;i<(sizeof(SAI_PSC_TBL)/8);i++)	//看看改采样率是否可以支持
	{
		if(samplerate==SAI_PSC_TBL[i][0])break;
	} 
	if(i==(sizeof(SAI_PSC_TBL)/8))return 1;	//搜遍了也找不到
	RCC->CR&=~(1<<28);						//关闭PLL3时钟 
	while(((RCC->CR&(1<<29)))&&(retry<0X1FFF))retry++;//等待PLL3时钟失锁
	RCC->PLLCKSELR&=~(0X3F<<20);			//清除DIVM3[5:0]原来的设置
	RCC->PLLCKSELR|=25<<20;					//DIVM3[5:0]=25,设置PLL3的预分频系数
	tempreg=RCC->PLL3DIVR;					//读取PLL3DIVR的值
	tempreg&=0XFFFF0000;					//清除DIVN和PLL3DIVP原来的设置
	tempreg|=(u32)(SAI_PSC_TBL[i][1]-1)<<0;	//设置DIVN[8:0]
	tempreg|=(u32)(SAI_PSC_TBL[i][2]-1)<<9;	//设置DIVP[6:0]
	RCC->PLL3DIVR=tempreg;					//设置PLL3DIVR寄存器
	RCC->PLLCFGR|=1<<22;					//DIVP3EN=1,使能pll3_p_ck  
	RCC->CR|=1<<28;							//开启PLL3
	while((RCC->CR&1<<29)==0);				//等待PLL3开启成功. 

	tempreg=SAI1_Block_A->CR1;			
	tempreg&=~(0X3F<<20);					//清除MCKDIV[5:0]设置
	tempreg|=(u32)SAI_PSC_TBL[i][3]<<20;	//设置MCKDIV[5:0]
	//tempreg|=1<<17;							//使能DMA
	//tempreg|=1<<16;							//使能SAI1 Block A
	SAI1_Block_A->CR1=tempreg;				//配置MCKDIV[5:0],同时使能SAI1 Block A 
	SAI1_Block_A->CR1|=1<<17;	//使能DMA
	SAI1_Block_A->CR1|=1<<16;	//使能SAI1 Block B
	//sai_ltdc_clkset(SAI_PSC_TBL[i][1]);		//针对RGB屏,需要重新修改PLL3R的值
	return 0;
}  
//SAIA TX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
//width:位宽(存储器和外设,同时设置),0,8位;1,16位;2,32位;
void SAIA_TX_DMA_Init(u32* buf0,u32* buf1,u16 num,u8 width)
{   
	RCC->AHB1ENR|=1<<1;			//DMA2时钟使能  
	RCC->D3AMR|=1<<0;			//DMAMUX时钟使能 
	while(DMA2_Stream3->CR&0X01);//等待DMA2_Stream1可配置
	DMAMUX1_Channel11->CCR=87;	//DMA2_Stream3的通道选择: 87,即SAI1_A对应的通道
								//详见<<STM32H7xx参考手册>>16.3.2节,Table 119
									
	DMA2->LIFCR|=0X3D<<22;		//清空通道3上所有中断标志
	DMA2_Stream3->FCR=0X0000021;//设置为默认值	
	
	DMA2_Stream3->PAR=(u32)&SAI1_Block_A->DR;//外设地址为:SAI1_Block_A->DR
	DMA2_Stream3->M0AR=(u32)buf0;//内存1地址
	DMA2_Stream3->M1AR=(u32)buf1;//内存2地址
	DMA2_Stream3->NDTR=num;		//暂时设置长度为1
	DMA2_Stream3->CR=0;			//先全部复位CR寄存器值  
	DMA2_Stream3->CR|=1<<6;		//存储器到外设模式 
	DMA2_Stream3->CR|=1<<8;		//循环模式
	DMA2_Stream3->CR|=0<<9;		//外设非增量模式
	DMA2_Stream3->CR|=1<<10;	//存储器增量模式
	DMA2_Stream3->CR|=(u16)width<<11;//外设数据长度:16位/32位
	DMA2_Stream3->CR|=(u16)width<<13;//存储器数据长度:16位/32位
	DMA2_Stream3->CR|=2<<16;	//高优先级
	DMA2_Stream3->CR|=1<<18;	//双缓冲模式
	DMA2_Stream3->CR|=0<<21;	//外设突发单次传输
	DMA2_Stream3->CR|=0<<23;	//存储器突发单次传输
	DMA2_Stream3->CR|=0<<25;	//选择通道0 SAI1_A通道 

	DMA2_Stream3->FCR&=~(1<<2);	//不使用FIFO模式
	DMA2_Stream3->FCR&=~(3<<0);	//无FIFO 设置
	
	//DMA2_Stream3->CR|=1<<4;		//开启传输完成中断
	//MY_NVIC_Init(5,0,DMA2_Stream3_IRQn,0);	//抢占1，子优先级0，组2  
}  

//SAIB RX DMA配置
//设置为双缓冲模式,并开启DMA传输完成中断
//buf0:M0AR地址.
//buf1:M1AR地址.
//num:每次传输数据量
//width:位宽(存储器和外设,同时设置),0,8位;1,16位;2,32位;
void SAIB_RX_DMA_Init(u32* buf0,u32* buf1,u16 num,u8 width)
{  

	RCC->AHB1ENR|=1<<1;			//DMA2时钟使能  
	RCC->D3AMR|=1<<0;			//DMAMUX时钟使能 
	while(DMA2_Stream5->CR&0X01);//等待DMA2_Stream5可配置
	DMAMUX1_Channel13->CCR=88;	//DMA2_Stream5的通道选择: 88,即SAI1_B对应的通道
								//详见<<STM32H7xx参考手册>>16.3.2节,Table 119
	
	DMA2->HIFCR|=0X3D<<6*1;		//清空通道5上所有中断标志
	DMA2_Stream5->FCR=0X0000021;//设置为默认值	
	
	DMA2_Stream5->PAR=(u32)&SAI1_Block_B->DR;//外设地址为:SAI1_Block_B->DR
	DMA2_Stream5->M0AR=(u32)buf0;//内存1地址
	DMA2_Stream5->M1AR=(u32)buf1;//内存2地址
	DMA2_Stream5->NDTR=num;		//暂时设置长度为1
	DMA2_Stream5->CR=0;			//先全部复位CR寄存器值  
	DMA2_Stream5->CR|=0<<6;		//外设到存储器模式 
	DMA2_Stream5->CR|=1<<8;		//循环模式
	DMA2_Stream5->CR|=0<<9;		//外设非增量模式
	DMA2_Stream5->CR|=1<<10;	//存储器增量模式
	DMA2_Stream5->CR|=(u16)width<<11;//外设数据长度:16位/32位
	DMA2_Stream5->CR|=(u16)width<<13;//存储器数据长度:16位/32位
	DMA2_Stream5->CR|=1<<16;	//中等优先级
	DMA2_Stream5->CR|=1<<18;	//双缓冲模式
	DMA2_Stream5->CR|=0<<21;	//外设突发单次传输
	DMA2_Stream5->CR|=0<<23;	//存储器突发单次传输
	DMA2_Stream5->CR|=0<<25;	//选择通道0 SAI1_B通道 

	DMA2_Stream5->FCR&=~(1<<2);	//不使用FIFO模式
	DMA2_Stream5->FCR&=~(3<<0);	//无FIFO 设置
	
	DMA2_Stream5->CR|=1<<4;		//开启传输完成中断
	MY_NVIC_Init(15,0,DMA2_Stream5_IRQn,4);	//抢占1，子优先级1，组2  
} 

//SAI DMA回调函数指针
void (*sai_tx_callback)(void);	//TX回调函数 
void (*sai_rx_callback)(void);	//RX回调函数

////DMA2_Stream3中断服务函数
//void DMA2_Stream3_IRQHandler(void)
//{      
//	if(DMA2->LISR&(1<<27))			//DMA2_Steam3,传输完成标志
//	{ 
//		DMA2->LIFCR|=1<<27;			//清除传输完成中断
//      	sai_tx_callback();			//执行回调函数,读取数据等操作在这里面处理 
//		SCB_CleanInvalidateDCache();//清除无效的D-Cache 
//	}   											 
//}  
////DMA2_Stream5中断服务函数
//void DMA2_Stream5_IRQHandler(void)
//{      
//	if(DMA2->HISR&(1<<11))			//DMA2_Steam5,传输完成标志
//	{ 
//		DMA2->HIFCR|=1<<11;			//清除传输完成中断
//      	sai_rx_callback();			//执行回调函数,读取数据等操作在这里面处理  
//		SCB_CleanInvalidateDCache();//清除无效的D-Cache
//	}   											 
//}  
//SAI开始播放
void SAI_Play_Start(void)
{   	  
	DMA2_Stream3->CR|=1<<0;		//开启DMA TX传输  		
}
//关闭I2S播放
void SAI_Play_Stop(void)
{   	 
	DMA2_Stream3->CR&=~(1<<0);	//结束播放	 	 
} 
//SAI开始录音
void SAI_Rec_Start(void)
{   	    
	DMA2_Stream5->CR|=1<<0;		//开启DMA RX传输 		
}
//关闭SAI录音
void SAI_Rec_Stop(void)
{   	  
	DMA2_Stream5->CR&=~(1<<0);	//结束录音		 
}





