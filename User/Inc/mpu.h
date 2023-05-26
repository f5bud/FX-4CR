#ifndef __MPU_H
#define __MPU_H	 
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//MPU内存保护 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/6/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
//********************************************************************************
//升级说明 
//无
//////////////////////////////////////////////////////////////////////////////////	 


//MPU的详细设置关系，请看：《STM32H7编程手册.pdf》
//这个文档的6.6节,Table 91.
//MPU保护区域许可属性定义（拷贝自stm32h7xx_hal_cortex.h）
//定义MPU->RASR寄存器AP[26:24]位的设置值
#define  MPU_REGION_NO_ACCESS      	((uint8_t)0x00U)		//无访问（特权&用户都不可访问）
#define  MPU_REGION_PRIV_RW        	((uint8_t)0x01U) 		//仅支持特权读写访问
#define  MPU_REGION_PRIV_RW_URO    	((uint8_t)0x02U)  		//禁止用户写访问（特权可读写访问）
#define  MPU_REGION_FULL_ACCESS    	((uint8_t)0x03U)  		//全访问（特权&用户都可访问）
#define  MPU_REGION_PRIV_RO        	((uint8_t)0x05U) 		//仅支持特权读访问
#define  MPU_REGION_PRIV_RO_URO    	((uint8_t)0x06U)		//只读（特权&用户都不可以写）
 
void MPU_Disable(void);
void MPU_Enable(void);
u8 MPU_Convert_Bytes_To_POT(u32 nbytes);
u8 MPU_Set_Protection(u32 baseaddr,u32 size,u32 rnum,u8 ap,u8 sen,u8 cen,u8 ben);
void MPU_Memory_Protection(void);
#endif

















