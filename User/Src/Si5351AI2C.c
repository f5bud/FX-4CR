//#include "sys.h"
#include "Si5351AI2C.h"
#include "myiic.h"
#include "dwt.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32
//SI5351A IIC驱动 代码	   
//修改日期:20117/11/16
//版本：V1.0
//版权所有，盗版必究。
//BG2IXD
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//PA12 SCL
//PA13 SDA
//extern FREQ f_Data;
#define I2C_WRITE 0Xc0 //写寄存器
#define I2C_READ  0Xc1 //读寄存器
//初始化IIC
//void SI5351A_IIC_Init(void)
//{					     
// 	RCC->AHB1ENR|=1<<1;//先使能外设IO PORTC时钟
//	RCC->AHB1ENR|=1<<2;//先使能外设IO PORTC时钟	
//	GPIO_Set(GPIOB,PIN14,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU); //PA0设置为下拉输入
//	GPIO_Set(GPIOC,PIN8,GPIO_MODE_OUT,0,0,GPIO_PUPD_PU); //PA0设置为下拉输入 	
//	SI5351A_IIC_SDA(1);
//	SI5351A_IIC_SCL(1);
//}
////产生IIC起始信号
//void SI5351A_IIC_Start(void)
//{
//	SI5351A_SDA_OUT();     //sda线输出
//	SI5351A_IIC_SDA(1);	  	  
//	SI5351A_IIC_SCL(1);
//	delay_us(2);
// 	SI5351A_IIC_SDA(0);//START:when CLK is high,DATA change form high to low 
//	delay_us(2);
//	SI5351A_IIC_SCL(0);//钳住I2C总线，准备发送或接收数据 
//}	  
////产生IIC停止信号
//void SI5351A_IIC_Stop(void)
//{
//	SI5351A_SDA_OUT();//sda线输出
//	SI5351A_IIC_SCL(0);
//	SI5351A_IIC_SDA(0);
// 	delay_us(2); 
//	SI5351A_IIC_SCL(1);//STOP:when CLK is high DATA change form low to high
// 	delay_us(2); 
//	SI5351A_IIC_SDA(1);//发送I2C总线结束信号 						   	
//}
////等待应答信号到来
////返回值：1，接收应答失败
////        0，接收应答成功
//u8 SI5351A_IIC_Wait_Ack(void)
//{
//	u8 ucErrTime=0;
//	SI5351A_SDA_IN();      //SDA设置为输入  
//	SI5351A_IIC_SDA(1);
//	delay_us(2);	   
//	SI5351A_IIC_SCL(1);
//	delay_us(2);	 
//	while(SI5351A_READ_SDA)
//	{
//		ucErrTime++;
//		if(ucErrTime>240)
//		{
//			SI5351A_IIC_Stop();
//			return 1;
//		}
//	}
//	SI5351A_IIC_SCL(0);//时钟输出0 	   
//	return 0;  
//} 
////产生ACK应答
//void SI5351A_IIC_Ack(void)
//{
//	SI5351A_IIC_SCL(0);
//	SI5351A_SDA_OUT();
//	SI5351A_IIC_SDA(0);
//	delay_us(2);
//	SI5351A_IIC_SCL(1);
//	delay_us(2);
//	SI5351A_IIC_SCL(0);
//}
////不产生ACK应答		    
//void SI5351A_IIC_NAck(void)
//{
//	SI5351A_IIC_SCL(0);
//	SI5351A_SDA_OUT();
//	SI5351A_IIC_SDA(1);
//	delay_us(2);
//	SI5351A_IIC_SCL(1);
//	delay_us(2);
//	SI5351A_IIC_SCL(0);
//}					 				     
////SI5351A_IIC发送一个字节
////返回从机有无应答
////1，有应答
////0，无应答			  
//void SI5351A_IIC_Send_Byte(u8 txd)
//{                        
//    u8 t;   
//	SI5351A_SDA_OUT(); 	    
//    SI5351A_IIC_SCL(0);//拉低时钟开始数据传输
//    for(t=0;t<8;t++)
//    {              
//        SI5351A_IIC_SDA((txd&0x80)>>7);
//        txd<<=1; 	  
//		delay_us(2);   //这三个延时都是必须的
//		SI5351A_IIC_SCL(1);
//		delay_us(2); 
//		SI5351A_IIC_SCL(0);	
//		delay_us(2);
//    }	 
//} 	    
////读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
//u8 SI5351A_IIC_Read_Byte(u8 ack)
//{
//	unsigned char i,receive=0;
//	SI5351A_SDA_IN();//SDA设置为输入
//    for(i=0;i<8;i++ )
//	{
//        SI5351A_IIC_SCL(0); 
//        delay_us(2);
//		SI5351A_IIC_SCL(1);
//        receive<<=1;
//        if(SI5351A_READ_SDA)receive++;   
//		delay_us(2); 
//    }					 
//    if (!ack)
//        SI5351A_IIC_NAck();//发送nACK
//    else
//        SI5351A_IIC_Ack(); //发送ACK   
//    return receive;
//}
//
//void i2cSendRegister(u8 reg, u16 data)
//{
//	//uint8_t stts;
//	
//	//stts = i2cStart();
//	SI5351A_IIC_Start();
//	//if (stts != I2C_START) return 1;
//	//stts = i2cByteSend(I2C_WRITE);
//	SI5351A_IIC_Send_Byte( I2C_WRITE);
//	//if (stts != I2C_SLA_W_ACK) return 2;
//	SI5351A_IIC_Wait_Ack();
//	
//	//SI5351A_IIC_Ack();
//	//stts = i2cByteSend(reg);
//	SI5351A_IIC_Send_Byte(reg);
//	//if (stts != I2C_DATA_ACK) return 3;
//	SI5351A_IIC_Wait_Ack();
//	
//	//SI5351A_IIC_Ack();
//	//stts = i2cByteSend(data);
//	SI5351A_IIC_Send_Byte(data);
//	//if (stts != I2C_DATA_ACK) return 4;
//	SI5351A_IIC_Wait_Ack();
//	//SI5351A_IIC_Ack();
//	SI5351A_IIC_Stop();
//	//i2cStop();

//	//return 0;
//}

//u8 i2cReadRegister(u8 reg)
//{
//	u8 temp=0;
//	
//	//stts = i2cStart();
//	SI5351A_IIC_Start();
//	//if (stts != I2C_START) return 1;
//	SI5351A_IIC_Wait_Ack();
//	//stts = i2cByteSend(I2C_WRITE);
//	SI5351A_IIC_Send_Byte(I2C_WRITE);
//	//if (stts != I2C_SLA_W_ACK) return 2;
//	SI5351A_IIC_Wait_Ack();
//	//stts = i2cByteSend(reg);
//	SI5351A_IIC_Send_Byte(reg);
//	//if (stts != I2C_DATA_ACK) return 3;
//	SI5351A_IIC_Wait_Ack();
//	SI5351A_IIC_Stop();
//	//stts = i2cStart();
//	SI5351A_IIC_Start();
//	//if (stts != I2C_START_RPT) return 4;
//	SI5351A_IIC_Send_Byte(I2C_READ);
//	//stts = i2cByteSend(I2C_READ);
//	//if (stts != I2C_SLA_R_ACK) return 5;
//	SI5351A_IIC_Wait_Ack();
//	//*data = i2cByteRead();
//	temp = SI5351A_IIC_Read_Byte(0);
//	SI5351A_IIC_NAck();
//	//i2cStop();
//	SI5351A_IIC_Stop();
//	return temp;
//}
u8 Si5351_Write_Reg(u8 reg, u16 data)
{
	
	IIC_Start();
	IIC_Send_Byte( I2C_WRITE);
	//IIC_Wait_Ack();
	if(IIC_Wait_Ack())return 1;	//等待应答(成功?/失败?) 
	//SI5351A_IIC_Ack();
	IIC_Send_Byte(reg);
	if(IIC_Wait_Ack())return 2;	//等待应答(成功?/失败?) 
	//IIC_Wait_Ack();
	IIC_Send_Byte(data);
	if(IIC_Wait_Ack())return 3;	//等待应答(成功?/失败?)
	
	IIC_Stop();

	return 0;
}

u8 Si5351_Read_Reg(u8 reg)
{
	u8 temp=0;
	
	//stts = i2cStart();
	IIC_Start();
	//if (stts != I2C_START) return 1;
	IIC_Wait_Ack();
	//stts = i2cByteSend(I2C_WRITE);
	IIC_Send_Byte(I2C_WRITE);
	//if (stts != I2C_SLA_W_ACK) return 2;
	IIC_Wait_Ack();
	//stts = i2cByteSend(reg);
	IIC_Send_Byte(reg);
	//if (stts != I2C_DATA_ACK) return 3;
	IIC_Wait_Ack();
	IIC_Stop();
	//stts = i2cStart();
	IIC_Start();
	//if (stts != I2C_START_RPT) return 4;
	IIC_Send_Byte(I2C_READ);
	//stts = i2cByteSend(I2C_READ);
	//if (stts != I2C_SLA_R_ACK) return 5;
	IIC_Wait_Ack();
	//*data = i2cByteRead();
	temp = IIC_Read_Byte(0);
	IIC_NAck();
	//i2cStop();
	IIC_Stop();
	return temp;
}
void SI5351A_Init()
{
	IIC_Init();
	Si5351_Write_Reg(183, 0XC0);
//	i2cSendRegister(3, 0);
	//i2cSendRegister(15, 0);
	//i2cSendRegister(3, 0);
}
void Si5351_I2c_exit(void)
{
	//SI5351A_SDA_IN();//SDA设置为输入
	//SI5351A_SCL_IN();
//	SI5351A_IIC_SCL=0;
//	SI5351A_IIC_SDA=0;
}
// Set up specified PLL with mult, num and denom
// mult is 15..90
// num is 0..1,048,575 (0xFFFFF)
// denom is 0..1,048,575 (0xFFFFF)
//
void setupPLL(uint8_t pll, uint8_t mult, uint32_t num,uint32_t denom)
{
	u32 P1;					// PLL config register P1
	u32 P2;					// PLL config register P2
	u32 P3;					// PLL config register P3

	P1 = (uint32_t)(128 * ((float)num / (float)denom));
	P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
	P2 = (uint32_t)(128 * ((float)num / (float)denom));
	P2 = (uint32_t)(128 * num - denom * P2);
	P3 = denom;

	Si5351_Write_Reg(pll + 0, (P3 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(pll + 1, (P3 & 0x000000FF));
	Si5351_Write_Reg(pll + 2, (P1 & 0x00030000) >> 16);
	Si5351_Write_Reg(pll + 3, (P1 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(pll + 4, (P1 & 0x000000FF));
	Si5351_Write_Reg(pll + 5, ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
	Si5351_Write_Reg(pll + 6, (P2 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(pll + 7, (P2 & 0x000000FF));
}

//
// Set up MultiSynth with integer divider and R divider
// R divider is the bit value which is OR'ed onto the appropriate register, it is a #define in si5351a.h
//
void setupMultisynth(uint8_t synth, uint32_t divider, uint8_t rDiv)
{
	u32 P1;					// Synth config register P1
	u32 P2;					// Synth config register P2
	u32 P3;					// Synth config register P3

	P1 = 128 * divider - 512;
	P2 = 0;							// P2 = 0, P3 = 1 forces an integer value for the divider
	P3 = 1;
	

	Si5351_Write_Reg(synth + 0,   (P3 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(synth + 1,   (P3 & 0x000000FF));
	Si5351_Write_Reg(synth + 2,   ((P1 & 0x00030000) >> 16) | rDiv);
	Si5351_Write_Reg(synth + 3,   (P1 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(synth + 4,   (P1 & 0x000000FF));
	Si5351_Write_Reg(synth + 5,   ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
	Si5351_Write_Reg(synth + 6,   (P2 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(synth + 7,   (P2 & 0x000000FF));
}
/**********************************************
小数除法设置
************************************************/
void setupMultisynth_FFT(uint8_t synth, uint32_t divider, uint8_t rDiv,uint8_t mult, uint32_t num,uint32_t denom)
{
	u32 P1;					// Synth config register P1
	u32 P2;					// Synth config register P2
	u32 P3;					// Synth config register P3

	P1 = (uint32_t)(128 * ((float)num / (float)denom));
	P1 = (uint32_t)(128 * (uint32_t)(mult) + P1 - 512);
	P2 = (uint32_t)(128 * ((float)num / (float)denom));
	P2 = (uint32_t)(128 * num - denom * P2);
	P3 = denom;
	

	Si5351_Write_Reg(synth + 0,   (P3 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(synth + 1,   (P3 & 0x000000FF));
	Si5351_Write_Reg(synth + 2,   ((P1 & 0x00030000) >> 16) | rDiv);
	Si5351_Write_Reg(synth + 3,   (P1 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(synth + 4,   (P1 & 0x000000FF));
	Si5351_Write_Reg(synth + 5,   ((P3 & 0x000F0000) >> 12) | ((P2 & 0x000F0000) >> 16));
	Si5351_Write_Reg(synth + 6,   (P2 & 0x0000FF00) >> 8);
	Si5351_Write_Reg(synth + 7,   (P2 & 0x000000FF));
}
//
// Switches off Si5351a output
// Example: si5351aOutputOff(SI_CLK0_CONTROL);
// will switch off output CLK0
//
void si5351aOutputOff(uint8_t clk)
{
	//i2c_init();
	IIC_Init();
	Si5351_Write_Reg(clk, 0x80);		// Refer to SiLabs AN619 to see bit values - 0x80 turns off the output stage

	//i2c_exit();
}
// 
// Set CLK0 output ON and to the specified frequency
// Frequency is in the range 1MHz to 150MHz
// Example: si5351aSetFrequency(10000000);
// will set output CLK0 to 10MHz
//
// This example sets up PLL A
// and MultiSynth 0
// and produces the output on CLK0
// 
void si5351aSetFrequency_LoA(u32 Xtalfreq,u32 frequency)
{
	u32 pllFreq;
	//u32 xtalFreq = Xtalfreq ;
	u32 l;
	float f;
	u8 mult;
	u32 num;
	u32 denom;
	u32 divider;

	//SI5351A_Init();						// Initialise the I2C

	divider = 900000000 / frequency;// Calculate the division ratio. 900,000,000 is the maximum internal 计算除法比。900000000是内部最大值。
									// PLL frequency: 900MHz
	if (divider % 2) divider--;		// Ensure an even integer division ratio 确保偶数整除比例

	pllFreq = divider * frequency;	// Calculate the pllFrequency: the divider * desired output frequency计算锁相环分频器输出频率

	mult = pllFreq / Xtalfreq;		//Determine the multiplier to get to the required pllFrequency确定锁相环乘数的整数部分
	l = pllFreq % Xtalfreq;			// It has three parts:提取分数部分
	f = l;							// mult is an integer that must be in the range 15..90多是一个整数，必须在范围15到90
	f *= 1048575;					// num and denom are the fractional parts, the numerator and denominatorNum和标准是分数的分子和分母部分，
	f /= Xtalfreq; 	// each is 20 bits (range 0..1048575)每一个都是20位（范围0…1048575）
	//f = f/xtalFreq * 1048575;
	num = f;						// the actual multiplier is  mult + num / denom实际乘数mult+ Num /denom
	denom = 1048575;				// For simplicity we set the denominator to the maximum 1048575为了简单起见，我们将分母设为最大值1048575。

									// Set up PLL A with the calculated multiplication ratio用计算乘法比设置PLL A
	setupPLL(SI_SYNTH_PLL_A, mult, num, denom);
									// Set up MultiSynth divider 0, with the calculated divider.建立的MultiSynth分频器0，与计算的分频器。 
									// The final R division stage can divide by a power of two, from 1..128.最后的R级可以除以两个功率，从1…128。 
									// reprented by constants SI_R_DIV1 to SI_R_DIV128 (see si5351a.h header file)用以si_r_div128 si_r_div1的常数（见si5351a。H头文件）
									// If you want to output frequencies below 1MHz, you have to use the 如果你想输出频率低于1MHz，你必须使用最后R阶段
									// final R division stage
	setupMultisynth(SI_SYNTH_MS_2, divider, SI_R_DIV_1);
									// Reset the PLL. This causes a glitch in the output. For small changes to复位锁相环。这会导致输出出现故障。对小改动 
									// the parameters, you don't need to reset the PLL, and there is no glitch 这些参数，你不需要重置PLL，也没有故障。
	//i2cSendRegister(SI_PLL_RESET, 0XA0);	
									// Finally switch on the CLK0 output (0x4F) 最后打开CLK0输出（0x4f） //设置multisynth0输入是锁相环A
									// and set the MultiSynth0 input to be PLL A
	Si5351_Write_Reg(SI_CLK2_CONTROL, 0X4c | SI_CLK_SRC_PLL_A
	);

//	Si5351_I2c_exit();						// Exit I2C
}
void si5351aSetFrequency_FFT(uint32_t bfo,uint32_t fft)
{
	u32 pllFreq;
//	u32 xtalFreq = XTAL_FREQ;
	u32 l;
	float f;
	u8 mult;
	u32 num;
	u32 denom;
	u32 divider;

//	SI5351A_Init();						// Initialise the I2C
	
	divider = 900000000 /bfo;// Calculate the division ratio. 900,000,000 is the maximum internal 计算除法比。900000000是内部最大值。
									// PLL frequency: 900MHz	
	if (divider % 2) divider--;		// Ensure an even integer division ratio 确保偶数整除比例

	pllFreq = divider * bfo;	// 计算 PLL 分频器输出频率

	mult = pllFreq / fft;		//计算多层分频器的整数部分
	l = pllFreq % fft;			// It has three parts:提取分子部分
	f = l;							// mult is an integer that must be in the range 15..90多是一个整数，必须在范围15到90
//	f *= 1048575;					// num and denom are the fractional parts, the numerator and denominatorNum和标准是分数的分子和分母部分，
//	f /= xtalFreq; 	// each is 20 bits (range 0..1048575)每一个都是20位（范围0…1048575）
	f = f/fft * 1048575;
	num = f;						// the actual multiplier is  mult + num / denom实际乘数mult+ Num /denom
	denom = 1048575;				// For simplicity we set the denominator to the maximum 1048575为了简单起见，我们将分母设为最大值1048575。

									// Set up PLL A with the calculated multiplication ratio用计算乘法比设置PLL A
	//setupPLL(SI_SYNTH_PLL_B, mult, num, denom);
									// Set up MultiSynth divider 0, with the calculated divider.建立的MultiSynth分频器0，与计算的分频器。 
									// The final R division stage can divide by a power of two, from 1..128.最后的R级可以除以两个功率，从1…128。 
									// reprented by constants SI_R_DIV1 to SI_R_DIV128 (see si5351a.h header file)用以si_r_div128 si_r_div1的常数（见si5351a。H头文件）
									// If you want to output frequencies below 1MHz, you have to use the 如果你想输出频率低于1MHz，你必须使用最后R阶段
									// final R division stage
	setupMultisynth_FFT(SI_SYNTH_MS_2, divider, SI_R_DIV_1, mult, num, denom);
									// Reset the PLL. This causes a glitch in the output. For small changes to复位锁相环。这会导致输出出现故障。对小改动 
									// the parameters, you don't need to reset the PLL, and there is no glitch 这些参数，你不需要重置PLL，也没有故障。
	Si5351_Write_Reg(SI_PLL_RESET, 0XA0);	
									// Finally switch on the CLK0 output (0x4F) 最后打开CLK0输出（0x4f） //设置multisynth0输入是锁相环A
									// and set the MultiSynth0 input to be PLL A
	Si5351_Write_Reg(SI_CLK2_CONTROL, 0x0f | SI_CLK_SRC_PLL_B);
	
	
//	Si5351_I2c_exit();						// Exit I2C
}

void si5351aSetFrequency_Bfo(u32 Xtalfreq,uint32_t frequency)
{
	u32 pllFreq;
	//u32 xtalFreq = Xtalfreq ;
	u32 l;
	float f;
	u8 mult;
	u32 num;
	u32 denom;
	u32 divider;

	SI5351A_Init();						// Initialise the I2C

	divider = 900000000 / frequency;// Calculate the division ratio. 900,000,000 is the maximum internal 计算除法比。900000000是内部最大值。
									// PLL frequency: 900MHz
	if (divider % 2) divider--;		// Ensure an even integer division ratio 确保偶数整除比例

	pllFreq = divider * frequency;	// Calculate the pllFrequency: the divider * desired output frequency计算锁相环分频器输出频率

	mult = pllFreq / Xtalfreq;		// Determine the multiplier to get to the required pllFrequency确定乘数得到所需的锁相环
	l = pllFreq % Xtalfreq;			// It has three parts:提取分数部分
	f = l;							// mult is an integer that must be in the range 15..90多是一个整数，必须在范围15到90
	f *= 1048575;					// num and denom are the fractional parts, the numerator and denominatorNum和标准是分数的分子和分母部分，
	f /= Xtalfreq; 	// each is 20 bits (range 0..1048575)每一个都是20位（范围0…1048575）
	//f = f/xtalFreq * 1048575;
	num = f;						// the actual multiplier is  mult + num / denom实际乘数mult+ Num /denom
	denom = 1048575;				// For simplicity we set the denominator to the maximum 1048575为了简单起见，我们将分母设为最大值1048575。

									// Set up PLL A with the calculated multiplication ratio用计算乘法比设置PLL A
	setupPLL(SI_SYNTH_PLL_B, mult, num, denom);
									// Set up MultiSynth divider 0, with the calculated divider.建立的MultiSynth分频器0，与计算的分频器。 
									// The final R division stage can divide by a power of two, from 1..128.最后的R级可以除以两个功率，从1…128。 
									// reprented by constants SI_R_DIV1 to SI_R_DIV128 (see si5351a.h header file)用以si_r_div128 si_r_div1的常数（见si5351a。H头文件）
									// If you want to output frequencies below 1MHz, you have to use the 如果你想输出频率低于1MHz，你必须使用最后R阶段
									// final R division stage
	setupMultisynth(SI_SYNTH_MS_1, divider, SI_R_DIV_1);
									// Reset the PLL. This causes a glitch in the output. For small changes to复位锁相环。这会导致输出出现故障。对小改动 
									// the parameters, you don't need to reset the PLL, and there is no glitch 这些参数，你不需要重置PLL，也没有故障。
	//i2cSendRegister(SI_PLL_RESET, 0XA0);	
									// Finally switch on the CLK0 output (0x4F) 最后打开CLK0输出（0x4f） //设置multisynth0输入是锁相环A
									// and set the MultiSynth0 input to be PLL A
	Si5351_Write_Reg(SI_CLK1_CONTROL, 0X4c | SI_CLK_SRC_PLL_B);

//	Si5351_I2c_exit();						// Exit I2C
}

