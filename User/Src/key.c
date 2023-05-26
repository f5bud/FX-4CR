
/*
*********************************************************************************************************
*
*	模块名称 : 独立按键驱动模块
*	文件名称 : key.c
*	版    本 : V1.0
*	说    明 : 扫描独立按键，具有软件滤波机制，具有按键FIFO。可以检测如下事件：
*				(1) 按键按下
*				(2) 按键弹起
*				(3) 长按键
*				(4) 长按时自动连发
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0    2013-02-01 armfly  正式发布
*		V1.1    2013-06-29 armfly  增加1个读指针，用于bsp_Idle() 函数读取系统控制组合键（截屏）
*								   增加 K1 K2 组合键 和 K2 K3 组合键，用于系统控制
*
*********************************************************************************************************
*/

#include "key.h"
#include "dwt.h"

/*	
	如果用于其它硬件，请修改GPIO定义和 IsKeyDown1 - IsKeyDown13 函数

	如果用户的按键个数小于13个，你可以将多余的按键全部定义为和第1个按键一样，并不影响程序功能
	#define KEY_COUNT    8	  这个在 bsp_key.h 文件中定义
*/
/********************************************************************************************************/
//u16 adc_key=0;//ADC按键变量
//extern u8 TX_flong;

//#define PIN_K1	    PDin(0)
//#define PIN_K2	    PDin(1)
//#define PIN_K3	    PDin(2)
//#define PIN_K4	    PDin(3)
//#define PIN_K5	    PDin(4)
//#define PIN_K6	    PDin(5)
//#define PIN_K7	    PDin(6)
//#define PIN_K8	    PDin(7)
//#define PIN_K9	  	PBin(3)
//#define PIN_K10		PBin(4)
//#define PIN_K11	    PBin(5)
//#define PIN_K12	  	PEin(4)
//#define PIN_K13		PEin(3)
static KEY_T s_tBtn[KEY_COUNT];
static KEY_FIFO_T s_tKey;		/* 按键FIFO变量,结构体 */

static void bsp_InitKeyVar(void);
static void bsp_InitKeyHard(void);
static void bsp_DetectKey(uint8_t i);

/*
*********************************************************************************************************
*	函 数 名: IsKeyDownX
*	功能说明: 判断按键是否按下
*	形    参: 无
*	返 回 值: 返回值1 表示按下，0表示未按下
*********************************************************************************************************
*/
	static uint8_t IsKeyDown1(void) {if (PIN_K1 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown2(void) {if (PIN_K2 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown3(void) {if (PIN_K3 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown4(void) {if (PIN_K4 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown5(void) {if (PIN_K5 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown6(void) {if (PIN_K6 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown7(void) {if (PIN_K7 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown8(void) {if (PIN_K8 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown9(void) {if (PIN_K9 == 0) return 1;else return 0;}
	static uint8_t IsKeyDown10(void){if (PIN_K10== 0) return 1;else return 0;}
	static uint8_t IsKeyDown11(void){if (PIN_K11== 0) return 1;else return 0;}
	static uint8_t IsKeyDown12(void){if (PIN_K12== 0) return 1;else return 0;}
	static uint8_t IsKeyDown13(void){if (PIN_K13== 0) return 1;else return 0;}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKey
*	功能说明: 初始化按键. 该函数被 bsp_Init() 调用。
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitKey(void)
{
	bsp_InitKeyVar();		/* 初始化按键变量 */
	bsp_InitKeyHard();		/* 初始化按键硬件 */
}

/*
*********************************************************************************************************
*	函 数 名: bsp_PutKey
*	功能说明: 将1个键值压入按键FIFO缓冲区。可用于模拟一个按键。
*	形    参:  _KeyCode : 按键代码
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_PutKey(uint8_t _KeyCode)
{
	s_tKey.Buf[s_tKey.Write] = _KeyCode;

	if (++s_tKey.Write  >= KEY_FIFO_SIZE)
	{
		s_tKey.Write = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey
*	功能说明: 从按键FIFO缓冲区读取一个键值。
*	形    参:  无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey(void)
{
	uint8_t ret;

	if (s_tKey.Read == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read];

		if (++s_tKey.Read >= KEY_FIFO_SIZE)
		{
			s_tKey.Read = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKey2
*	功能说明: 从按键FIFO缓冲区读取一个键值。独立的读指针。
*	形    参:  无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
uint8_t bsp_GetKey2(void)
{
	uint8_t ret;

	if (s_tKey.Read2 == s_tKey.Write)
	{
		return KEY_NONE;
	}
	else
	{
		ret = s_tKey.Buf[s_tKey.Read2];

		if (++s_tKey.Read2 >= KEY_FIFO_SIZE)
		{
			s_tKey.Read2 = 0;
		}
		return ret;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_GetKeyState
*	功能说明: 读取按键的状态
*	形    参:  _ucKeyID : 按键ID，从0开始
*	返 回 值: 1 表示按下， 0 表示未按下
*********************************************************************************************************
*/
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID)
{
	return s_tBtn[_ucKeyID].State;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SetKeyParam
*	功能说明: 设置按键参数
*	形    参：_ucKeyID : 按键ID，从0开始
*			_LongTime : 长按事件时间
*			 _RepeatSpeed : 连发速度
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed)
{
	s_tBtn[_ucKeyID].LongTime = _LongTime;			/* 长按时间 0 表示不检测长按键事件 */
	s_tBtn[_ucKeyID].RepeatSpeed = _RepeatSpeed;			/* 按键连发的速度，0表示不支持连发 */
	s_tBtn[_ucKeyID].RepeatCount = 0;						/* 连发计数器 */
}
/*
*********************************************************************************************************
*	函 数 名: bsp_ClearKey
*	功能说明: 清空按键FIFO缓冲区
*	形    参：无
*	返 回 值: 按键代码
*********************************************************************************************************
*/
void bsp_ClearKey(void)
{
	s_tKey.Read = s_tKey.Write;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyHard
*	功能说明: 配置按键对应的GPIO
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyHard(void)
{
	RCC->AHB4ENR|=1<<0;	    //使能PORTA时钟 
	RCC->AHB4ENR|=1<<1;     //使能PORTB时钟
	RCC->AHB4ENR|=1<<2;  	//使能PORTC时钟 
	//RCC->AHB4ENR|=1<<3;     //使能PORTD时钟
	RCC->AHB4ENR|=1<<4;     //使能PORTE时钟
	GPIO_Set(GPIOA,PIN2|PIN3|PIN6|PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //PB0,PB1设置
	GPIO_Set(GPIOB,PIN0|PIN1,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //PB0,PB1设置
	GPIO_Set(GPIOC,PIN4|PIN5,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //设置上拉输入
	//GPIO_Set(GPIOD,PIN0|PIN1|PIN2|PIN3|PIN4|PIN7,GPIO_MODE_IN,0,0,GPIO_PUPD_PU);	//设置上拉输入
	GPIO_Set(GPIOE,PIN11|PIN12|PIN13|PIN14|PIN15,GPIO_MODE_IN,0,0,GPIO_PUPD_PU); //设置为上拉输入 	
}
/*
*********************************************************************************************************
*	函 数 名: bsp_InitKeyVar
*	功能说明: 初始化按键变量
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitKeyVar(void)
{
	uint8_t i;

	/* 对按键FIFO读写指针清零 */
	s_tKey.Read = 0;
	s_tKey.Write = 0;
	s_tKey.Read2 = 0;
	//adc_key=ADC1->JDR1;
	/* 给每个按键结构体成员变量赋一组缺省值 */
	for (i = 0; i < KEY_COUNT; i++)
	{
		s_tBtn[i].LongTime = KEY_LONG_TIME;			/* 长按时间 0 表示不检测长按键事件 */
		s_tBtn[i].Count = KEY_FILTER_TIME / 2;		/* 计数器设置为滤波时间的一半 */
		s_tBtn[i].State = 0;							/* 按键缺省状态，0为未按下 */
		//s_tBtn[i].KeyCodeDown = 3 * i + 1;				/* 按键按下的键值代码 */
		//s_tBtn[i].KeyCodeUp   = 3 * i + 2;				/* 按键弹起的键值代码 */
		//s_tBtn[i].KeyCodeLong = 3 * i + 3;				/* 按键被持续按下的键值代码 */
		s_tBtn[i].RepeatSpeed = 0;						/* 按键连发的速度，0表示不支持连发 */
		s_tBtn[i].RepeatCount = 0;						/* 连发计数器 */
	}
	
	/* 判断按键按下的函数 */
	s_tBtn[0].IsKeyDownFunc = IsKeyDown1;
	s_tBtn[1].IsKeyDownFunc = IsKeyDown2;
	s_tBtn[2].IsKeyDownFunc = IsKeyDown3;
	s_tBtn[3].IsKeyDownFunc = IsKeyDown4;
	s_tBtn[4].IsKeyDownFunc = IsKeyDown5;
	s_tBtn[5].IsKeyDownFunc = IsKeyDown6;
	s_tBtn[6].IsKeyDownFunc = IsKeyDown7;
	s_tBtn[7].IsKeyDownFunc = IsKeyDown8;
	s_tBtn[8].IsKeyDownFunc = IsKeyDown9;
	s_tBtn[9].IsKeyDownFunc = IsKeyDown10;
	s_tBtn[10].IsKeyDownFunc = IsKeyDown11;
	s_tBtn[11].IsKeyDownFunc = IsKeyDown12;
	s_tBtn[12].IsKeyDownFunc = IsKeyDown13;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_DetectKey
*	功能说明: 检测一个按键。非阻塞状态，必须被周期性的调用。
*	形    参:  按键结构变量指针
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_DetectKey(uint8_t i)
{
	KEY_T *pBtn;

	/*
		如果没有初始化按键函数，则报错
		if (s_tBtn[i].IsKeyDownFunc == 0)
		{
			printf("Fault : DetectButton(), s_tBtn[i].IsKeyDownFunc undefine");
		}
	*/

	pBtn = &s_tBtn[i];
	if (pBtn->IsKeyDownFunc())
	{
		if (pBtn->Count < KEY_FILTER_TIME)
		{
			pBtn->Count = KEY_FILTER_TIME;
		}
		else if(pBtn->Count < 2 * KEY_FILTER_TIME)
		{
			pBtn->Count++;
		}
		else
		{
			if (pBtn->State == 0)
			{
				pBtn->State = 1;

				/* 发送按钮按下的消息 */
				bsp_PutKey((uint8_t)(3 * i + 1));
			}

			if (pBtn->LongTime > 0)
			{
				if (pBtn->LongCount < pBtn->LongTime)
				{
					/* 发送按钮持续按下的消息 */
					if (++pBtn->LongCount == pBtn->LongTime)
					{
						/* 键值放入按键FIFO */
						bsp_PutKey((uint8_t)(3 * i + 3));
					}
				}
				else
				{
					if (pBtn->RepeatSpeed > 0)
					{
						if (++pBtn->RepeatCount >= pBtn->RepeatSpeed)
						{
							pBtn->RepeatCount = 0;
							/* 常按键后，每隔10ms发送1个按键 */
							bsp_PutKey((uint8_t)(3 * i + 1));
						}
					}
				}
			}
		}
	}
	else
	{
		if (pBtn->LongCount >=   KEY_LONG_TIME)
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* 发送按钮弹起的消息 */
				bsp_PutKey((uint8_t)(0));
			}
		}
		if(pBtn->LongCount < KEY_LONG_TIME)
		{
			if (pBtn->State == 1)
			{
				pBtn->State = 0;

				/* 发送按钮弹起的消息 */
				bsp_PutKey((uint8_t)(3 * i + 2));
			}
		}
		pBtn->Count = 0;
		pBtn->LongCount = 0;
		pBtn->RepeatCount = 0;
	}
}

/*
*********************************************************************************************************
*	函 数 名: bsp_KeyScan
*	功能说明: 扫描所有按键。非阻塞，被systick中断周期性的调用
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_KeyScan(void)
{
	uint8_t i;

	for (i = 0; i < KEY_COUNT; i++)
	{
		bsp_DetectKey(i);
	}
}
/*
*********************************************************************************************************
*	函 数 名: SysTick_init(u16 SYSCLK)
*	功能说明: 嘀哒定时器初始化
*	形    参: SYSCLK：使用内核时钟源,同CPU同频率
*			  Xms: SysTick中断的间隔时间
*	返 回 值: 无
*********************************************************************************************************
*/
//void SysTick_init(u32 SYSCLK,u16 Xms)
//{
//	u32 reload;
// 	SysTick->CTRL|=1<<2;	//SYSTICK使用内核时钟源,同CPU同频率	 
//							//不论是否使用OS,fac_us都需要使用 
//	reload =Xms * SYSCLK/1000;	//Xms中断一次
//							//reload为24位寄存器,最大值:16777216,在400M下,约合0.042s左右	
//	SysTick->CTRL|=1<<1;   					//开启SYSTICK中断
//	SysTick->LOAD=reload; 					//每1/delay_ostickspersec秒中断一次	
//	SysTick->CTRL|=1<<0;   					//开启SYSTICK    
//}
/*
*********************************************************************************************************
*	函 数 名:  SysTick_Handler(void)
*	功能说明: 在嘀哒定时器中断扫描按键
*	形    参: SYSCLK：使用内核时钟源,同CPU同频率
*	返 回 值: 无
*********************************************************************************************************
*/ 
//void SysTick_Handler(void)
//{
//	//static uint8_t s_count = 0;	
//	//if (++s_count >= 10)
//	//{
//	//	s_count = 0;

//		bsp_KeyScan();	/* 每隔10ms调用一次此函数，此函数在 bsp.c */
//	//}
//}
/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
