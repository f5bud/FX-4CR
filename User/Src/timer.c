#include "timer.h"
#include "led.h" 
#include "key.h"
#include "lcd.h"
#include "Audio_driver.h"
#include "KEY_Control.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//FX-4C
//定时器 驱动代码	   
//创建日期:2020/9/18
//版本：V1.0
//版权所有，盗版必究。								  
//********************************************************************************
//修改说明
//V1.1 20180607
//新增TIM3_PWM_Init函数,用于PWM输出	
//V1.2 20180607
//新增TIM5_CH1_Cap_Init函数,用于输入捕获	
//V1.3 20180613
//新增TIM15_CH2_PWM_Init函数,用于PWM DAC							  
////////////////////////////////////////////////////////////////////////////////// 	 
/* 保存 TIM定时中断到后执行的回调函数指针 */
//static void (*s_TIM_CallBack1)(void);
//static void (*s_TIM_CallBack2)(void);
//static void (*s_TIM_CallBack3)(void);
//static void (*s_TIM_CallBack4)(void);

/* 定义软件定时器结构体变量 */
static SOFT_TMR s_tTmr[TMR_COUNT] = {0};

/*
	全局运行时间，单位1ms
	最长可以表示 24.85天，如果你的产品连续运行时间超过这个数，则必须考虑溢出问题
*/
__IO int32_t g_iRunTime = 0;

static __IO uint8_t g_ucEnableSystickISR = 0;	/* 等待变量初始化 */

static void bsp_SoftTimerDec(SOFT_TMR *_tmr);
/*
*********************************************************************************************************
*	函 数 名: SysTick_init(u16 SYSCLK)
*	功能说明: 嘀哒定时器初始化
*	形    参: SYSCLK：使用内核时钟源,同CPU同频率
*			  Xms: SysTick中断的间隔时间
*	返 回 值: 无
*********************************************************************************************************
*/
void SysTick_init(u32 SYSCLK,u16 Xms)
{
	u32 reload;
	uint8_t i;

	/* 清零所有的软件定时器 */
	for (i = 0; i < TMR_COUNT; i++)
	{
		s_tTmr[i].Count = 0;
		s_tTmr[i].PreLoad = 0;
		s_tTmr[i].Flag = 0;
		s_tTmr[i].Mode = TMR_ONCE_MODE;	/* 缺省是1次性工作模式 */
	}
	
 	SysTick->CTRL|=1<<2;	/* SYSTICK使用内核时钟源,同CPU同频率 */	 
							
	reload =Xms * (SYSCLK/1000);	/* Xus中断一次 */
									/* reload为24位寄存器,最大值:16777216,在400M下,约合0.042s左右 */	
	SysTick->CTRL|=1<<1;   			/* 开启SYSTICK中断 */
	SysTick->LOAD=reload; 			/* 每1/delay_ostickspersec秒中断一次	*/
	SysTick->CTRL|=1<<0;   			/* 开启SYSTICK */   
}
/*
*********************************************************************************************************
*	函 数 名: bsp_SoftTimerDec
*	功能说明: 每隔1ms对所有定时器变量减1。必须被SysTick_ISR周期性调用。
*	形    参:  _tmr : 定时器变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_SoftTimerDec(SOFT_TMR *_tmr)
{
	if (_tmr->Count > 0)
	{
		/* 如果定时器变量减到1则设置定时器到达标志 */
		if (--_tmr->Count == 0)
		{
			_tmr->Flag = 1;

			/* 如果是自动模式，则自动重装计数器 */
			if(_tmr->Mode == TMR_AUTO_MODE)
			{
				_tmr->Count = _tmr->PreLoad;
			}
		}
	}
}
/*
*********************************************************************************************************
*	函 数 名:  SysTick_Handler(void)
*	功能说明: 在嘀哒定时器中断扫描按键
*	形    参: SYSCLK：使用内核时钟源,同CPU同频率
*	返 回 值: 无
*********************************************************************************************************
*/ 
void SysTick_Handler(void)
{
	//static uint16_t s_count = 0;
	static uint16_t k_count = 0;
	uint16_t i;
	/* 每隔1ms，对软件定时器的计数器进行减一操作 */
	//if (++s_count >= 1)
	//{
		//s_count = 0;
		for (i = 0; i < TMR_COUNT; i++)
		{
			bsp_SoftTimerDec(&s_tTmr[i]);
		}
	//}

	/* 每隔10ms，对KEY扫描定时器的计数器进行减一操作 */	
	if (++k_count >= 10)
	{
		k_count = 0;
		bsp_KeyScan();	/* 每隔10ms调用一次此函数，此函数在 key.c */
	}
}
/*
*********************************************************************************************************
*	函 数 名: bsp_StartTimer
*	功能说明: 启动一个定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartTimer(uint8_t _id, uint32_t _period)
{

	SysTick->CTRL|=0<<1;  			/* 关中断 */

	s_tTmr[_id].Count = _period;		/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 1次性工作模式 */

	SysTick->CTRL|=1<<1;  				/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_StartAutoTimer
*	功能说明: 启动一个自动定时器，并设置定时周期。
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位10ms
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period)
{
	SysTick->CTRL|=0<<1;  		/* 关中断 */

	s_tTmr[_id].Count = _period;		/* 实时计数器初值 */
	s_tTmr[_id].PreLoad = _period;		/* 计数器自动重装值，仅自动模式起作用 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_AUTO_MODE;	/* 自动工作模式 */

	SysTick->CTRL|=1<<1;  		/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_StopTimer
*	功能说明: 停止一个定时器
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_StopTimer(uint8_t _id)
{
	SysTick->CTRL|=0<<1;  	/* 关中断 */

	s_tTmr[_id].Count = 0;				/* 实时计数器初值 */
	s_tTmr[_id].Flag = 0;				/* 定时时间到标志 */
	s_tTmr[_id].Mode = TMR_ONCE_MODE;	/* 自动工作模式 */

	SysTick->CTRL|=1<<1;  		/* 开中断 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_CheckTimer
*	功能说明: 检测定时器是否超时
*	形    参:  	_id     : 定时器ID，值域【0,TMR_COUNT-1】。用户必须自行维护定时器ID，以避免定时器ID冲突。
*				_period : 定时周期，单位1ms
*	返 回 值: 返回 0 表示定时未到， 1表示定时到
*********************************************************************************************************
*/
uint8_t bsp_CheckTimer(uint8_t _id)
{
	if (_id >= TMR_COUNT)
	{
		return 0;
	}

	if (s_tTmr[_id].Flag == 1)
	{
		s_tTmr[_id].Flag = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}
/*
***************************************************************
* 	基本定时器6中断初始化
* 	TIM6的时钟来自APB1,当D2PPRE1≥2分频的时候
* 	TIM6的时钟为APB1时钟的2倍，而APB1为100M
* 	所以,TIM6的时钟频率为200Mhz
* 	arr：自动重装值。
* 	psc：时钟预分频数
* 	定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
* 	Ft=定时器工作频率,单位:Mhz
* 	这里使用的是定时器6的TRGO事件触发ADC和DAC进行数据转换；
***************************************************************
*/
void TIM6_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1LENR|=1<<4;				//TIM6时钟使能  
	//如果开启TIM3时钟后,直接开始配置TIM3,则会有问题
	//while((RCC->APB1LENR&(1<<4))==0)	//等待时钟设置OK  
 	TIM6->ARR=arr;  				//设定计数器自动重装值 
	TIM6->PSC=psc;  				//预分频器
	//TIM6->SMCR=0;					//SMS[3:0]=0000,CK_INT作为时钟源
	//TIM6->DIER|=1<<0;  				//允许更新中断
	//TIM6->CR2&=~(3<<4);				//
	TIM6->CR2|= 1<<5;				//选择更新事件作为触发输出 (TRGO)
	//TIM6->CR1 = 0;
	//TIM6->CR1&=~(1<<2);				//CMS[1:0]=00,边沿对齐模式
	//TIM6->CR1&=~(1<<1);				//DIR=0,向上计数模式
	//TIM6->CR1|=1<<0;				//使能定时器3
 // MY_NVIC_Init(15,0,TIM6_DAC_IRQn ,0);	//抢占1，子优先级3，组2		 
}
extern u32 m,n;
//定时器3中断服务程序	 
void TIM6_DAC_IRQHandler(void)
{ 	
	//static u32 timer;
	//static u8 led1sta=1;
	if(TIM6->SR&0X0001)//溢出中断
	{
		//if(++timer >32000)
		//{
			//LED1(led1sta^=1);
			m++;if(m>640000)m=0;
		//}
		//LCD_ShowNum(160,160,GRAY1,BLACK, 3,24,sd.Enc2_Value );
	}				   
	TIM6->SR&=~(1<<0);//清除中断标志位 	    
}

//通用定时器3中断初始化
//TIM3的时钟来自APB1,当D2PPRE1≥2分频的时候
//TIM3的时钟为APB1时钟的2倍，而APB1为100M
//所以,TIM3的时钟频率为200Mhz
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1LENR|=1<<0;				//TIM3时钟使能  
	//如果开启TIM3时钟后,直接开始配置TIM3,则会有问题
	//while((RCC->APB1LENR&(1<<0))==0)	//等待时钟设置OK  
 	TIM2->ARR=arr;  				//设定计数器自动重装值 
	TIM2->PSC=psc;  				//预分频器
	//TIM2->SMCR=0;					//SMS[3:0]=0000,CK_INT作为时钟源
	TIM2->CR2|= 1<<5;				//选择更新事件作为触发输出 (TRGO)
	//TIM2->DIER|=1<<0;  				//允许更新中断	  
	//TIM2->CR1&=~(3<<5);				//CMS[1:0]=00,边沿对齐模式
	//TIM2->CR1&=~(1<<4);				//DIR=0,向上计数模式
	TIM2->CR1|=1<<0;				//使能定时器3
  //	MY_NVIC_Init(1,3,TIM3_IRQn,2);	//抢占1，子优先级3，组2		 
}

//TIM3 PWM部分初始化 
//TIM3的时钟来自APB1,当D2PPRE1≥2分频的时候
//TIM3的时钟为APB1时钟的2倍，而APB1为100M
//所以,TIM3的时钟频率为200Mhz
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB1LENR|=1<<1;	//TIM3时钟使能    
	RCC->AHB4ENR|=1<<1;   	//使能PORTB时钟	
//	GPIO_Set(GPIOB,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PU);//复用功能,上拉输出
//	GPIO_AF_Set(GPIOB,1,2);	//PB1,AF2 
	
	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器不分频 
	TIM3->CCMR2|=6<<12;  	//CH4 PWM1模式		 
	TIM3->CCMR2|=1<<11; 	//CH4 预装载使能	   
	TIM3->CCER|=1<<12;   	//OC4 输出使能	
	TIM3->CCER|=1<<13;   	//OC4 低电平有效	   
	TIM3->CR1|=1<<7;   		//ARPE使能 
	TIM3->CR1|=1<<0;    	//使能定时器3											  
}  

//定时器5通道1输入捕获配置
//arr：自动重装值(TIM2,TIM5是32位的!!)
//psc：时钟预分频数
void TIM5_CH1_Cap_Init(u32 arr,u16 psc)
{		 
	RCC->APB1LENR|=1<<3;   	//TIM5 时钟使能 
	RCC->AHB4ENR|=1<<0;   	//使能PORTA时钟	
	GPIO_Set(GPIOA,PIN0,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PD);//复用功能,下拉
	GPIO_AF_Set(GPIOA,0,2);	//PA0,AF2
	  
 	TIM5->ARR=arr;  		//设定计数器自动重装值   
	TIM5->PSC=psc;  		//预分频器 

	TIM5->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
 	TIM5->CCMR1|=0<<4; 		//IC1F=0000 配置输入滤波器 不滤波
 	TIM5->CCMR1|=0<<10; 	//IC1PS=00 	配置输入分频,不分频 

	TIM5->CCER|=0<<1; 		//CC1P=0	上升沿捕获
	TIM5->CCER|=1<<0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中

	TIM5->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
	TIM5->DIER|=1<<1;   	//允许捕获1中断				
	TIM5->DIER|=1<<0;   	//允许更新中断	
	TIM5->CR1|=0x01;    	//使能定时器2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//抢占2，子优先级0，组2	   
}

//捕获状态
//[7]:0,没有成功的捕获;1,成功捕获到一次.
//[6]:0,还没捕获到低电平;1,已经捕获到低电平了.
//[5:0]:捕获低电平后溢出的次数(对于32位定时器来说,1us计数器加1,溢出时间:4294秒)
u8  TIM5CH1_CAPTURE_STA=0;	//输入捕获状态		    				
u32	TIM5CH1_CAPTURE_VAL;	//输入捕获值(TIM2/TIM5是32位)
//定时器5中断服务程序	 
void TIM5_IRQHandler(void)
{ 		    
	u16 tsr;
	tsr=TIM5->SR;
 	if((TIM5CH1_CAPTURE_STA&0X80)==0)//还未成功捕获	
	{
		if(tsr&0X01)//溢出
		{	     
			if(TIM5CH1_CAPTURE_STA&0X40)//已经捕获到高电平了
			{
				if((TIM5CH1_CAPTURE_STA&0X3F)==0X3F)//高电平太长了
				{
					TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获了一次
					TIM5CH1_CAPTURE_VAL=0XFFFFFFFF;
				}else TIM5CH1_CAPTURE_STA++;
			}	 
		}
		if(tsr&0x02)//捕获1发生捕获事件
		{	
			if(TIM5CH1_CAPTURE_STA&0X40)		//捕获到一个下降沿 		
			{	  			
				TIM5CH1_CAPTURE_STA|=0X80;		//标记成功捕获到一次高电平脉宽
			    TIM5CH1_CAPTURE_VAL=TIM5->CCR1;	//获取当前的捕获值.
	 			TIM5->CCER&=~(1<<1);			//CC1P=0 设置为上升沿捕获
			}else  								//还未开始,第一次捕获上升沿
			{
				TIM5CH1_CAPTURE_STA=0;			//清空
				TIM5CH1_CAPTURE_VAL=0;
				TIM5CH1_CAPTURE_STA|=0X40;		//标记捕获到了上升沿
				TIM5->CR1&=~(1<<0)		;    	//使能定时器2
	 			TIM5->CNT=0;					//计数器清空
	 			TIM5->CCER|=1<<1; 				//CC1P=1 设置为下降沿捕获
				TIM5->CR1|=0x01;    			//使能定时器2
			}		    
		}			     	    					   
 	}
	TIM5->SR=0;//清除中断标志位   
}

//TIM1 CH4 PWM部分初始化 
//TIM1的时钟来自APB2,当D2PPRE1≥2分频的时候
//TIM1的时钟为APB2时钟的2倍，而APB2为100M
//所以,TIM1的时钟频率为200Mhz
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_CH4_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB2ENR|=1<<0;	//TIM1时钟使能    
	RCC->AHB4ENR|=1<<4;   	//使能PORTA时钟	
	GPIO_Set(GPIOE,PIN14,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_MID,GPIO_PUPD_PD);//复用功能,上拉输出
	GPIO_AF_Set(GPIOE,14,1);	//PE14,AF2 
	
	TIM1->ARR=arr;			//设定计数器自动重装值 
	TIM1->PSC=psc;			//预分频器不分频 
	TIM1->CCMR2|=6<<12;  	//CH2 PWM1模式		 
	TIM1->CCMR2|=1<<11; 	//CH2 预装载使能	   
	//TIM1->CCER|=1<<12;   	//OC2 输出使能
	TIM1->CCER&=~(1<<13);   	//OC4 低电平有效	 
	TIM1->EGR=1<<0;			//软件控制产生更新事件,使写入PSC的值立即生效,否则将会要等到定时器溢出才会生效!
 	TIM1->BDTR|=1<<15;   	//MOE=1,主输出使能	
	//TIM1->BDTR|=1<<14;   	//MOE=1,主输出使能
	TIM1->CR1|=1<<7;   		//ARPE使能 
	//TIM1->CR1|=1<<0;    	//使能定时器1											  
}  











