#ifndef __gpio_set_H
#define __gpio_set_H	
#include "sys.h"

//
//	电源控制 POWER_SET
//
#define POWER_SET_ON	GPIO_Pin_Set(GPIOB,PIN5,0)
#define POWER_SET_OFF	GPIO_Pin_Set(GPIOB,PIN5,1)
#define POWER_READ		GPIO_Pin_Get(GPIOB,PIN5)
//
//收发转换 PTT
//
#define RX		 	1	//RX
#define TX			0	//TX
#define PTT_IN()			{GPIO_Set(GPIOB,PIN14,GPIO_MODE_IN,1,0,GPIO_PUPD_PU);}
#define PTT_OUT()			{GPIO_Set(GPIOB,PIN14,GPIO_MODE_OUT,1,0,GPIO_PUPD_PU);}
#define PTT_RT				GPIO_Pin_Get(GPIOB,PIN14) 	//TX
#define PTT_RT_VOX(x)		GPIO_Pin_Set(GPIOB,PIN14,x) 	//TX

//
//收发转换控制 
//

#define CONTROL_RX		 0x01	//RX
#define CONTROL_TX		 0x00	//TX
#define TR_CONTROL(x)	 GPIO_Pin_Set(GPIOD,PIN7,x)	//TX
#define TR_READ	 		 GPIO_Pin_Get(GPIOD,PIN7)	//TX
//
//	RF AMP
//
#define AMP_ON			GPIO_Pin_Set(GPIOB,PIN8,1) 	//
#define AMP_OFF			GPIO_Pin_Set(GPIOB,PIN8,0) 	//
#define AMP_READ	 	GPIO_Pin_Get(GPIOB,PIN8)
//
// IF ATT  
//
#define	ATT_ON			GPIO_Pin_Set(GPIOC,PIN1,0) 	//
#define ATT_OFF			GPIO_Pin_Set(GPIOC,PIN1,1) 	//
#define ATT_READ	 	GPIO_Pin_Get(GPIOC,PIN1)
//
//VOX  
//
//#define VOX_OUT(x)		GPIO_Pin_Set(GPIOC,PIN0,x) 	//LCD_BL
//
// MIC电源  
//
#define MIC_POWER(x)		GPIO_Pin_Set(GPIOA,PIN13,x)//0 供电；1 断电
//
//	按键背光
//
#define KEY_BL(x)		GPIO_Pin_Set(GPIOD,PIN15,x) 	//KEY_BL;0 熄灭；1 点亮；

//#define KEY_BL1(x)		GPIO_Pin_Set(GPIOA,PIN3,x) 	//KEY_BL
//
//	蓝牙供电
//
#define BLUETOOTH_POWER(x)		GPIO_Pin_Set(GPIOD,PIN14,x)//1 供电；0 断电
//#define BLUETOOTH_ON		GPIO_Pin_Get(GPIOC,PIN8)
#define BLUETOOTH_DAT		GPIO_Pin_Get(GPIOC,PIN9)
//#define MIC			PEout(15)	//MIC VDDDETEC
/*
*	耳机检测
*/
#define HP_DETEC_1	GPIO_Pin_Get(GPIOC,PIN8)
#define HP_DETEC_2	GPIO_Pin_Get(GPIOB,PIN4)
//
// CW 电键 
//
#define CW_A		0
#define CW_B		1
#define CW_RX		1
#define CW_TX		0
#define CW_K_DI		GPIO_Pin_Get(GPIOD,PIN10) 	// CW keyA
#define CW_K_DA		GPIO_Pin_Get(GPIOD,PIN11) 	// CW keyB
//
//声卡电源
//
//#define SOUND_CARD_POW(x)		GPIO_Pin_Set(GPIOB,PIN3,x) 	//KEY_BL 
//
// RF输入带通控制IO 
//
#define BAND_A0(x)		GPIO_Pin_Set(GPIOD,PIN6,x)
#define BAND_A1(x)	    GPIO_Pin_Set(GPIOD,PIN5,x)
#define BAND_A2(x)	    GPIO_Pin_Set(GPIOD,PIN4,x)
#define BAND_A3(x)	    GPIO_Pin_Set(GPIOD,PIN3,x)
//#define BAND_80(x)		GPIO_Pin_Set(GPIOD,PIN15,x)
//#define BAND_60(x)	    GPIO_Pin_Set(GPIOC,PIN14,x)
//#define BAND_40(x)	    GPIO_Pin_Set(GPIOA,PIN8,x)
//#define BAND_30(x)	    GPIO_Pin_Set(GPIOC,PIN9,x)
//#define BAND_20(x)	    GPIO_Pin_Set(GPIOD,PIN11,x)
//#define BAND_15(x)	    GPIO_Pin_Set(GPIOD,PIN10,x)
//#define BAND_10(x)      GPIO_Pin_Set(GPIOD,PIN9,x)
//#define BAND_RX(x)	    GPIO_Pin_Set(GPIOD,PIN8,x)
//
/* RF功率输出低通控制IO */
#define LPF_80M(x)			GPIO_Pin_Set(GPIOD,PIN0,x)	//1 接通；0 断开；
#define LPF_60_40M(x)		GPIO_Pin_Set(GPIOC,PIN12,x)
#define LPF_30_20M(x)		GPIO_Pin_Set(GPIOD,PIN1,x)
#define LPF_17_15M(x)		GPIO_Pin_Set(GPIOC,PIN10,x)
#define LPF_12_10M(x)		GPIO_Pin_Set(GPIOD,PIN2,x)
#define LPF_6M(x)			GPIO_Pin_Set(GPIOA,PIN14,x)

void GPIO_Init(void);
#endif
