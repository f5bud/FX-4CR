 
/*
*********************************************************************************************************
*
*	模块名称 : 按键驱动模块
*	文件名称 : bsp_key.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*********************************************************************************************************
*/
#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
/*
**************************************************************************************************************
*
*	
*	IXD_202 按键口线分配：
*		K1 键       : PA2     SSB
*		K2 键       : PB1     M.V
*		K3 键       : PA3     CW
*		K4 键   	: PE11    AM.FM
*		K5 键 		: PA6    RIT
*		K6 键 		: PE12    A/B
*		K7 键		: PA7    IF.ATT
*		K8 键   	: PE13    PWR
*		K9 键		: PC4	 MENU
*		K10键		: PE14	 FILTER
*		K11键		: PC5	 F
*		K12键		: PE15
*		K13键		: PB0
**************************************************************************************************************
*/
#define PIN_K1		GPIO_Pin_Get(GPIOA,PIN2)//PDin(0)
#define PIN_K2	    GPIO_Pin_Get(GPIOB,PIN1)//PDin(1)
#define PIN_K3	    GPIO_Pin_Get(GPIOA,PIN3)//PDin(2)
#define PIN_K4	    GPIO_Pin_Get(GPIOE,PIN11)//PDin(3)
#define PIN_K5	    GPIO_Pin_Get(GPIOA,PIN6)//PDin(4)
#define PIN_K6	    GPIO_Pin_Get(GPIOE,PIN12)//PAin(7)
#define PIN_K7	    GPIO_Pin_Get(GPIOA,PIN7)//PCin(4)
#define PIN_K8	    GPIO_Pin_Get(GPIOE,PIN13)//PDin(7)
#define PIN_K9	  	GPIO_Pin_Get(GPIOC,PIN4)//PBin(9)
#define PIN_K10		GPIO_Pin_Get(GPIOE,PIN14)//PEin(12)
#define PIN_K11	    GPIO_Pin_Get(GPIOC,PIN5)//PEin(13)
#define PIN_K12	  	GPIO_Pin_Get(GPIOE,PIN15)//PEin(4)
#define PIN_K13		GPIO_Pin_Get(GPIOB,PIN0)//PEin(3)
/* 按键个数,10个独立键*/
#define KEY_COUNT    13

/* 根据应用程序的功能重命名按键宏 */
#define DOWN_K1		KEY_1_DOWN
#define UP_K1		KEY_1_UP
#define LONG_K1		KEY_1_LONG

#define DOWN_K2		KEY_2_DOWN
#define UP_K2		KEY_2_UP
#define LONG_K2		KEY_2_LONG

#define DOWN_K3		KEY_3_DOWN
#define UP_K3		KEY_3_UP
#define LONG_K3		KEY_3_LONG

#define DOWN_K4		KEY_4_DOWN		
#define UP_K4		KEY_4_UP
#define LONG_K4		KEY_4_LONG

#define DOWN_K5		KEY_5_DOWN		
#define UP_K5		KEY_5_UP
#define LONG_K5		KEY_5_LONG

#define DOWN_K6		KEY_6_DOWN		
#define UP_K6		KEY_6_UP
#define LONG_K6		KEY_6_LONG

#define DOWN_K7		KEY_7_DOWN		
#define UP_K7		KEY_7_UP
#define LONG_K7		KEY_7_LONG

#define DOWN_K8		KEY_8_DOWN		
#define UP_K8		KEY_8_UP
#define LONG_K8		KEY_8_LONG

#define DOWN_K9		KEY_9_DOWN		
#define UP_K9	    KEY_9_UP
#define LONG_K9		KEY_9_LONG

#define DOWN_K10	KEY_10_DOWN		
#define UP_K10  	KEY_10_UP
#define LONG_K10	KEY_10_LONG

#define DOWN_K11	KEY_11_DOWN		
#define UP_K11	    KEY_11_UP
#define LONG_K11	KEY_11_LONG

#define DOWN_K12	KEY_12_DOWN		
#define UP_K12  	KEY_12_UP
#define LONG_K12	KEY_12_LONG

#define DOWN_K13	KEY_13_DOWN		
#define UP_K13  	KEY_13_UP
#define LONG_K13	KEY_13_LONG

/* 按键ID, 主要用于bsp_KeyState()函数的入口参数 */
typedef enum
{
	KID_K1 = 0,
	KID_K2,
	KID_K3,
	KID_K4,
	KID_K5,
	KID_K6,
	KID_K7,
	KID_K8,
	KID_K9,
	KID_K10,
	KID_K11,
	KID_K12,
	KID_K13
}KEY_ID_E;

/*
	按键滤波时间50ms, 单位10ms。
	只有连续检测到50ms状态不变才认为有效，包括弹起和按下两种事件
	即使按键电路不做硬件滤波，该滤波机制也可以保证可靠地检测到按键事件
*/
#define KEY_FILTER_TIME   5
#define KEY_LONG_TIME     100			/* 单位10ms， 持续1秒，认为长按事件 */

/*
	每个按键对应1个全局的结构体变量。
*/
typedef struct
{
	/* 下面是一个函数指针，指向判断按键手否按下的函数 */
	u8 (*IsKeyDownFunc)(void); /* 按键按下的判断函数,1表示按下 */

	u8  Count;			/* 滤波器计数器 */
	u16 LongCount;		/* 长按计数器 */
	u16 LongTime;		/* 按键按下持续时间, 0表示不检测长按 */
	u8  State;			/* 按键当前状态（按下还是弹起） */
	u8  RepeatSpeed;	/* 连续按键周期 */
	u8  RepeatCount;	/* 连续按键计数器 */
}KEY_T;

/*
	定义键值代码, 必须按如下次序定时每个键的按下、弹起和长按事件

	推荐使用enum, 不用#define，原因：
	(1) 便于新增键值,方便调整顺序，使代码看起来舒服点
	(2) 编译器可帮我们避免键值重复。
*/
typedef enum
{
	KEY_NONE = 0,			/* 0 表示按键事件 */

	KEY_1_DOWN,				/* 1键按下 */
	KEY_1_UP,				/* 1键弹起 */
	KEY_1_LONG,				/* 1键长按 */

	KEY_2_DOWN,				/* 2键按下 */
	KEY_2_UP,				/* 2键弹起 */
	KEY_2_LONG,				/* 2键长按 */

	KEY_3_DOWN,				/* 3键按下 */
	KEY_3_UP,				/* 3键弹起 */
	KEY_3_LONG,				/* 3键长按 */

	KEY_4_DOWN,				/* 4键按下 */
	KEY_4_UP,				/* 4键弹起 */
	KEY_4_LONG,				/* 4键长按 */

	KEY_5_DOWN,				/* 5键按下 */
	KEY_5_UP,				/* 5键弹起 */
	KEY_5_LONG,				/* 5键长按 */

	KEY_6_DOWN,				/* 6键按下 */
	KEY_6_UP,				/* 6键弹起 */
	KEY_6_LONG,				/* 6键长按 */

	KEY_7_DOWN,				/* 7键按下 */
	KEY_7_UP,				/* 7键弹起 */
	KEY_7_LONG,				/* 7键长按 */

	KEY_8_DOWN,				/* 8键按下 */
	KEY_8_UP,				/* 8键弹起 */
	KEY_8_LONG,				/* 8键长按 */

	/* 组合键 */
	KEY_9_DOWN,				/* 9键按下 */
	KEY_9_UP,				/* 9键弹起 */
	KEY_9_LONG,				/* 9键长按 */

	KEY_10_DOWN,			/* 10键按下 */
	KEY_10_UP,				/* 10键弹起 */
	KEY_10_LONG,			/* 10键长按 */
	
	KEY_11_DOWN,			/* 11键按下 */
	KEY_11_UP,				/* 11键弹起 */
	KEY_11_LONG,			/* 11键长按 */

	KEY_12_DOWN,			/* 12键按下 */
	KEY_12_UP,				/* 12键弹起 */
	KEY_12_LONG,			/* 12键长按 */
	
	KEY_13_DOWN,			/* 13键按下 */
	KEY_13_UP,				/* 13键弹起 */
	KEY_13_LONG,			/* 13键长按 */
}KEY_ENUM;

/* 按键FIFO用到变量 */
#define KEY_FIFO_SIZE	13
typedef struct
{
	u8 Buf[KEY_FIFO_SIZE];		/* 键值缓冲区 */
	u8 Read;					/* 缓冲区读指针1 */
	u8 Write;					/* 缓冲区写指针 */
	u8 Read2;					/* 缓冲区读指针2 */
}KEY_FIFO_T;


/* 供外部调用的函数声明 */
void bsp_InitKey(void);
void bsp_KeyScan(void);
void bsp_PutKey(uint8_t _KeyCode);
uint8_t bsp_GetKey(void);
uint8_t bsp_GetKey2(void);
uint8_t bsp_GetKeyState(KEY_ID_E _ucKeyID);
void bsp_SetKeyParam(uint8_t _ucKeyID, uint16_t _LongTime, uint8_t  _RepeatSpeed);
void bsp_ClearKey(void);
//void SysTick_init(u32 SYSCLK,u16 Xms);
#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
