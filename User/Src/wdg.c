#include "wdg.h"
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//看门狗 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/6
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//V1.1 20180606
//增加了窗口看门狗相关函数。				  
////////////////////////////////////////////////////////////////////////////////// 	 


//初始化独立看门狗
//prer:分频数:0~7(只有低3位有效!)
//rlr:自动重装载值,0~0XFFF.
//分频因子=4*2^prer.但最大值只能是256!
//rlr:重装载寄存器值:低11位有效.
//时间计算(大概):Tout=((4*2^prer)*rlr)/32 (ms).
void IWDG_Init(u8 prer,u16 rlr) 
{
	IWDG1->KR=0X5555;//使能对IWDG->PR和IWDG->RLR的写		 										  
  	IWDG1->PR=prer;  //设置分频系数   
  	IWDG1->RLR=rlr;  //从加载寄存器 IWDG->RLR  
	IWDG1->KR=0XAAAA;//reload											   
  	IWDG1->KR=0XCCCC;//使能看门狗	
}
//喂独立看门狗
void IWDG_Feed(void)
{
	IWDG1->KR=0XAAAA;//reload											   
}

//保存WWDG计数器的设置值,默认为最大. 
u8 WWDG_CNT=0x7f; 
//初始化窗口看门狗 	
//tr   :T[6:0],计数器值 
//wr   :W[6:0],窗口值 
//fprer:分频系数（WDGTB）,范围:0~7,表示2^WDGTB分频
//Fwwdg=PCLK3/(4096*2^fprer). 一般PCLK3=100Mhz
//溢出时间=(4096*2^fprer)*(tr-0X3F)/PCLK3
//假设fprer=4,tr=7f,PCLK3=100Mhz
//则溢出时间=4096*16*64/100Mhz=41.94ms
void WWDG_Init(u8 tr,u8 wr,u8 fprer) 
{     
	RCC->APB3ENR|=1<<6; 	//使能wwdg时钟 
	WWDG_CNT=tr&WWDG_CNT;   //初始化WWDG_CNT.  
	RCC->GCR|=1<<0;			//WW1RSC=1,复位WWDG1,硬件清零 
	WWDG1->CFR|=fprer<<11;	//PCLK1/4096再除2^fprer 
	WWDG1->CFR&=0XFF80;      
	WWDG1->CFR|=wr;			//设定窗口值      
	WWDG1->CR|=WWDG_CNT; 	//设定计数器值 
	WWDG1->CR|=1<<7;  		//开启看门狗      
	MY_NVIC_Init(2,3,WWDG_IRQn,2);//抢占2，子优先级3，组2     
	WWDG1->SR=0X00;			//清除提前唤醒中断标志位 
	WWDG1->CFR|=1<<9;		//使能提前唤醒中断 
} 
//重设置WWDG计数器的值 
void WWDG_Set_Counter(u8 cnt) 
{ 
	WWDG1->CR=(cnt&0x7F);	//重设置7位计数器 
} 
//窗口看门狗中断服务程序 
void WWDG_IRQHandler(void) 
{      
	static u8 led1sta=1;
	if(WWDG1->SR&0X01)		//先判断是否发生了WWDG提前唤醒中断
	{
		WWDG1->SR=0X00;				//清除提前唤醒中断标志位 
		WWDG_Set_Counter(WWDG_CNT);	//重设窗口看门狗的值!         
		LED1(led1sta^=1); 			//LED1闪烁
	}
}










