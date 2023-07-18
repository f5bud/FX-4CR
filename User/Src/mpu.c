#include "mpu.h" 
#include "usart.h" 
#include "led.h" 
#include "dwt.h" 
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

//禁止MPU保护
void MPU_Disable(void)
{  
	SCB->SHCSR&=~(1<<16);	//禁止MemManage 
	MPU->CTRL&=~(1<<0);		//禁止MPU
}

//开启MPU保护
void MPU_Enable(void)
{ 
	MPU->CTRL=(1<<2)|(1<<0);//使能PRIVDEFENA,使能MPU 
	SCB->SHCSR|=1<<16;		//使能MemManage
}

//将nbytes转换为2为底的指数.
//NumberOfBytes:字节数.
//返回值:以2为底的指数值
u8 MPU_Convert_Bytes_To_POT(u32 nbytes)
{
	u8 count=0;
	while(nbytes!=1)
	{
		nbytes>>=1;
		count++;
	}
	return count;
} 

//设置某个区域的MPU保护
//baseaddr:MPU保护区域的基址(首地址)
//size:MPU保护区域的大小(必须是32的倍数,单位为字节)
//rnum:MPU保护区编号,范围:0~7,最大支持8个保护区域
//ap:访问权限,访问关系如下:
//0,无访问（特权&用户都不可访问）
//1,仅支持特权读写访问
//2,禁止用户写访问（特权可读写访问）
//3,全访问（特权&用户都可访问）
//4,无法预测(禁止设置为4!!!)
//5,仅支持特权读访问
//6,只读（特权&用户都不可以写）
//详见:STM32F7编程手册.pdf,4.6节,Table 89.
//sen:是否允许共用;0,不允许;1,允许
//cen:是否允许cache;0,不允许;1,允许
//ben:是否允许缓冲;0,不允许;1,允许
//返回值;0,成功.
//    其他,错误.
u8 MPU_Set_Protection(u32 baseaddr,u32 size,u32 rnum,u8 ap,u8 sen,u8 cen,u8 ben)
{
	u32 tempreg=0;
	u8 rnr=0;
	if((size%32)||size==0)return 1;	//大小不是32的倍数,或者size为0,说明参数错误
	rnr=MPU_Convert_Bytes_To_POT(size)-1;//转换为2为底的指数值
	MPU_Disable();					//设置之前,先禁止MPU保护
	MPU->RNR=rnum;					//设置保护区域
	MPU->RBAR=baseaddr;				//设置基址
	tempreg|=0<<28;					//允许指令访问(允许读取指令)
	tempreg|=((u32)ap)<<24;			//设置访问权限,
	tempreg|=0<<19;					//设置类型扩展域为level0
	tempreg|=((u32)sen)<<18;		//是否允许共用
	tempreg|=((u32)cen)<<17;		//是否允许cache
	tempreg|=((u32)ben)<<16;		//是否允许缓冲
	tempreg|=0<<8;					//禁止子区域
	tempreg|=rnr<<1;				//设置保护区域大小
	tempreg|=1<<0;					//使能该保护区域 
    MPU->RASR=tempreg;				//设置RASR寄存器
	MPU_Enable();					//设置完毕,使能MPU保护
	return 0;
}

//设置需要保护的存储块
//必须对部分存储区域进行MPU保护,否则可能导致程序运行异常
//比如MCU屏不显示,摄像头采集数据出错等等问题...
void MPU_Memory_Protection(void)
{
	MPU_Set_Protection(0x20000000,128*1024,1,MPU_REGION_FULL_ACCESS,0,1,1);		//保护整个DTCM,共128K字节,禁止共用,允许cache,允许缓冲
	
	MPU_Set_Protection(0x24000000,512*1024,2,MPU_REGION_FULL_ACCESS,1,1,1);		//保护整个AXI SRAM,共512K字节,禁止共用,允许cache,允许缓冲
	MPU_Set_Protection(0x30000000,512*1024,3,MPU_REGION_FULL_ACCESS,0,1,0);		//保护整个SRAM1~SRAM3,共288K字节,禁止共用,允许cache,允许缓冲
	MPU_Set_Protection(0x38000000,64*1024,4,MPU_REGION_FULL_ACCESS,0,0,0);		//保护整个SRAM4,共64K字节,禁止共用,允许cache,允许缓冲

	MPU_Set_Protection(0x60000000,64*1024*1024,5,MPU_REGION_FULL_ACCESS,0,0,0);	//保护MCU LCD屏所在的FMC区域,,共64M字节,禁止共用,禁止cache,禁止缓冲
	MPU_Set_Protection(0XC0000000,32*1024*1024,6,MPU_REGION_FULL_ACCESS,0,0,0);	//保护SDRAM区域,共32M字节,禁止共用,允许cache,允许缓冲
	MPU_Set_Protection(0X80000000,256*1024*1024,7,MPU_REGION_FULL_ACCESS,1,1,1);//保护整个NAND FLASH区域,共256M字节,禁止共用,禁止cache,禁止缓冲
}

//MemManage错误处理中断
//进入此中断以后,将无法恢复程序运行!!
void MemManage_Handler(void)
{ 
	//LED1(0);							//点亮DS1
	//printf("Mem Access Error!!\r\n"); 	//输出错误信息
	delay_ms(1000);	
	//printf("Soft Reseting...\r\n");		//提示软件重启
	delay_ms(1000);	
	Sys_Soft_Reset();					//软复位
}














