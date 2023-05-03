#include "adc.h"
#include "dwt.h"		 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//FX-4C
//ADC 驱动代码	   
//BG2IXD
//创建日期:2020/9/18
//版本：V1.0
//版权所有，盗版必究。								  
////////////////////////////////////////////////////////////////////////////////// 	 


//初始化ADC1
//这里我们使用规则通道2(PA2)作为FFT输入
//我们默认仅开启ADC1_CH2P
//16位转换时间位:采样周期+8.5个ADC周期
void  Adc2_Init(void)
{    
	//先初始化IO口
 	RCC->AHB1ENR|=1<<5;    		//使能ADC1/2时钟 
	RCC->AHB4ENR|=1<<1;    		//使能PORTA时钟	 
	RCC->AHB4ENR|=1<<2;    		//使能PORTA时钟	  
	GPIO_Set(GPIOB,PIN1,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PB2,模拟输入,不带上下拉   
	GPIO_Set(GPIOC,PIN0|PIN1,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PC0/PC1,模拟输入,不带上下拉   
//	GPIO_Set(GPIOC,PIN2|PIN3,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PC0/PC1,模拟输入,不带上下拉 
	//SYSCFG->PMCR =0;
//	SYSCFG->PMCR |=1<<27;
//	SYSCFG->PMCR |=1<<26;
	RCC->AHB1RSTR|=1<<5;   		//ADC1/2复位
	RCC->AHB1RSTR&=~(1<<5);		//复位结束	
	RCC->D3CCIPR&=~(3<<16);		//ADCSEL[1:0]清零
	RCC->D3CCIPR|=0<<16;		//ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz
	ADC3_COMMON->CCR|=3<<16;
	ADC12_COMMON->CCR|=1<<18;	//PRESC[3:0]=1,输入时钟2分频,即adc_ker_ck=per_ck/2=64/2=32Mhz(不能超过36Mhz)
	
	ADC2->CR=0;   				//CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒.
	
	ADC2->CR|=1<<28;  			//ADVREGEN=1,使能ADC稳压器
	delay_ms(10);				//等待稳压器启动完成,约10us,这里延时大一点,没关系.
	ADC2->CR|=1<<8;  			//BOOST=1,ADC工作在boost模式(ADC频率大于20M的时候,必须设置boost位)	
	ADC2->CFGR&=~(1<<13);		//CONT=0,单次转换模式
	ADC2->CFGR|=1<<12;			//OVRMOD=1,复写模式(DR寄存器可被复写)	
	ADC2->CFGR|=0<<10;			//EXTEN[1:0]=0,硬件触发
	ADC2->CFGR|=14<<5;			//EXTEN[9:5]=13,定时器6RGO触发
	ADC2->CFGR&=~(7<<2);		//RES[2:0]位清零
	ADC2->CFGR|=2<2;			//RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.)
	ADC2->CFGR|=3<<0;			//01：选择 DMA 循环模式

	ADC2->CFGR2&=~((u32)15<<28);//LSHIFT[3:0]=0,不左移,数据右对齐.
	ADC2->CFGR2&=~((u32)0X3FF<<16);//OSR[9:0]=0,不使用过采样
	
	
	ADC2->CR&=~((u32)1<<30);	//ADCALDIF=0,校准单端转换通道
	ADC2->CR|=0<<16;			//ADCALLIN=1,线性校准
	ADC2->CR|=(u32)1<<31;		//开启校准
	while(ADC2->CR&((u32)1<<31));//等待校准完成
	 
	ADC2->JSQR&=~(0XFFFFFFFF);		//L[31:0]清零
	ADC2->JSQR|= 5<<21;
	ADC2->JSQR|=11<<15;
	ADC2->JSQR|=10<<9;
	ADC2->JSQR|= 1<<7;
	ADC2->JSQR|=14<<2;
	ADC2->JSQR|=2<<0;     		//L[3:0]=2,3个转换在规则序列中 也就是只转换规则序列1
	
	/* ADC转换通道选择 */
	//ADC1->PCSEL|=1<<14;			//ADC转换通道预选择2 
	//ADC1->SQR1&=~(0X1F<<6*1);	//规则序列1通道清零
	//ADC1->SQR1|=2<<6*1;			//设置规则序列1的转换通道为ch 
	//ADC1->CR|=1<<2;       		//启动规则转换通道 
	//设置通道2的采样时间
	//ADC1->SMPR2&=~(7<<(3*4));	//通道14采样时间清空	  
 	//ADC1->SMPR2|=3<<(3*4); 		//通道14 16.5个周期,提高采样时间可以提高精确度	
		//设置通道9的采样时间
	ADC2->SMPR1&=~(7<<(3*5));	//通道5采样时间清空	  
 	ADC2->SMPR1|=0<<(3*5); 		//通道5 16.5个周期,提高采样时间可以提高精确度
	//设置通道10的采样时间
	ADC2->SMPR2&=~(7<<(3*0));	//通道10采样时间清空	  
 	ADC2->SMPR2|=0<<(3*0); 		//通道10 16.5个周期,提高采样时间可以提高精确度
	//设置通道11的采样时间
	ADC2->SMPR2&=~(7<<(3*1));	//通道11采样时间清空	  
 	ADC2->SMPR2|=0<<(3*1); 		//通道11 16.5个周期,提高采样时间可以提高精确度  
}				  
//获得ADC值
//ch:通道值 0~19
//返回值:转换结果
u16 Get_Adc1(u8 ch)   
{
	ADC1->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC1->SQR1&=~(0X1F<<6*1);	//规则序列1通道清零
	ADC1->SQR1|=ch<<6*1;		//设置规则序列1的转换通道为ch 
	ADC1->CR|=1<<2;       		//启动规则转换通道 
	while(!(ADC1->ISR&1<<2));	//等待转换结束	 	   
	return ADC1->DR;			//返回adc值	
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc1_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc1(ch);
		delay_ms(5);
	}
	return temp_val/times;
}  

//初始化ADC2
//使用规则通道9(PB1)作为电源电压监测；
//使用规则通道10(PC0)作为REF驻波检测；
//使用规则通道11(PC1)作为PWD功率检测；
//我们默认仅开启ADC1_CH2P
//16位转换时间位:采样周期+8.5个ADC周期
void  Adc1_Init(void)
{    
	//先初始化IO口
 	RCC->AHB1ENR|=1<<5;    		//使能ADC1/2时钟 
	RCC->AHB4ENR|=1<<0;    		//使能PORTA时钟
	RCC->AHB4ENR|=1<<2;    		//使能PORTC时钟
	GPIO_Set(GPIOA,PIN0|PIN1,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PB2,模拟输入,不带上下拉   
	GPIO_Set(GPIOC,PIN0|PIN3,GPIO_MODE_AIN,0,0,GPIO_PUPD_NONE);//PC0/PC1,模拟输入,不带上下拉 
	//SYSCFG->PMCR =0;
	//SYSCFG->PMCR |=1<<27;
	//SYSCFG->PMCR |=1<<26;
	RCC->AHB1RSTR|=1<<5;   		//ADC1/2复位
	RCC->AHB1RSTR&=~(1<<5);		//复位结束	
	RCC->D3CCIPR&=~(3<<16);		//ADCSEL[1:0]清零
	RCC->D3CCIPR|=0<<16;		//ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz
	ADC12_COMMON->CCR|=3<<16;
	ADC12_COMMON->CCR|=3<<18;	//PRESC[3:0]=1,输入时钟2分频,即adc_ker_ck=per_ck/2=64/2=32Mhz(不能超过36Mhz)
	
	ADC1->CR=0;   				//CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒.
	ADC1->CR|=1<<28;  			//ADVREGEN=1,使能ADC稳压器
	delay_ms(100);				//等待稳压器启动完成,约10us,这里延时大一点,没关系.
	ADC1->CR|=1<<8;  			//BOOST=1,ADC工作在boost模式(ADC频率大于20M的时候,必须设置boost位)	
	ADC1->CFGR&=~(1<<13);		//CONT=0,单次转换模式
	ADC1->CFGR|=1<<12;			//OVRMOD=1,复写模式(DR寄存器可被复写)	
	ADC1->CFGR|=1<<10;			//EXTEN[1:0]=0,硬件触发
	ADC1->CFGR|=13<<5;			//EXTEN[9:5]=13,定时器6 TRGO触发
	ADC1->CFGR&=~(7<<2);		//RES[2:0]位清零
	ADC1->CFGR|=0<<2;			//RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.)
	ADC1->CFGR|=3<<0;			//02：选择 DMA 单次模式
	
	ADC1->CFGR2&=~((u32)15<<28);//LSHIFT[3:0]=0,不左移,数据右对齐.
	ADC1->CFGR2&=~((u32)0X3FF<<16);//OSR[9:0]=0,不使用过采样
	ADC1->CFGR2|= ((u32)15<<16);	//8倍过采样率（相当于20位ADC）
	ADC1->CFGR2|= ((u32)4<<5);		//右移3位
	ADC1->CFGR2|= ((u32)1<<0);		//过采样使能
	
	ADC1->CR&=~((u32)1<<30);	//ADCALDIF=0,校准单端转换通道
	ADC1->CR|=0<<16;			//ADCALLIN=1,线性校准
	ADC1->CR|=(u32)1<<31;		//开启校准
	while(ADC1->CR&((u32)1<<31));//等待校准完成
	 
	ADC1->SQR1&=~(0XF<<0);		//L[3:0]清零
	ADC1->SQR1|=3<<0;     		//L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1 
	/* ADC转换通道选择 */
	ADC1->PCSEL|=1<<16;			//ADC转换通道预选择16 VOX声控
	ADC1->PCSEL|=1<<17;			//ADC转换通道预选择17 电压检测
	ADC1->PCSEL|=1<<10;			//ADC转换通道预选择10 FWD
	ADC1->PCSEL|=1<<13;			//ADC转换通道预选择13 SWR
	ADC1->SQR1&=~(0X1F<<6*1);	//规则序列1通道清零
	ADC1->SQR1|=16<<6*1;			//设置规则序列1的转换通道为ch16 
	ADC1->SQR1|=17<<6*2;			//设置规则序列2的转换通道为ch17
	ADC1->SQR1|=10<<6*3;			//设置规则序列3的转换通道为ch10
	ADC1->SQR1|=13<<6*4;			//设置规则序列4的转换通道为ch13
	ADC1->CR|=1<<2;       		//启动规则转换通道 
	//设置通道16的采样时间
	ADC1->SMPR2&=~(7<<(3*6));	//PA0:通道16采样时间清空	  
 	ADC1->SMPR2|=3<<(3*6); 		//通道5 16.5个周期,提高采样时间可以提高精确度
	//设置通道17的采样时间
	ADC1->SMPR2&=~(7<<(3*7));	//PA1通道17采样时间清空	  
 	ADC1->SMPR2|=3<<(3*7); 		//通道5 16.5个周期,提高采样时间可以提高精确度
	//设置通道10的采样时间
	ADC1->SMPR2&=~(7<<(3*0));	//PC0:通道10采样时间清空	  
 	ADC1->SMPR2|=3<<(3*0); 		//通道10 16.5个周期,提高采样时间可以提高精确度
	//设置通道13的采样时间
	ADC1->SMPR2&=~(7<<(3*3));	//PC3:通道13采样时间清空	  
 	ADC1->SMPR2|=3<<(3*3); 		//通道13 16.5个周期,提高采样时间可以提高精确度
 	//ADC2->CR|=1<<0;	   			//开启AD转换器	  
}
//ADC通道选择
//ch:通道值 0~19
void Adc1_Channel_Selection(u8 ch)   
{
	//ADC3->CR|=1<<0;
	//while(!(ADC3->ISR&1<<0));
	ADC1->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC1->SQR1&=~(0X1F<<(6*1));	//规则序列1通道清零
	ADC1->SQR1|=ch<<(6*1);		//设置规则序列1的转换通道为ch 
	ADC1->CR|=1<<2;       		//启动规则转换通道 
	//while(!(ADC2->ISR&1<<2));	//等待转换结束	 	   
	//return ADC2->DR;			//返回adc值	
}
//获得ADC值
//ch:通道值 0~19
//返回值:转换结果
u16 Get_Adc2(u8 ch)   
{
	ADC2->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC2->SQR1&=~(0X1F<<(6*1));	//规则序列1通道清零
	ADC2->SQR1|=ch<<(6*1);		//设置规则序列1的转换通道为ch 
	ADC2->CR|=1<<2;       		//启动规则转换通道 
	while(!(ADC2->ISR&1<<2));	//等待转换结束	 	   
	return ADC2->DR;			//返回adc值	
}
//获取通道ch的转换值，取times次,然后平均 
//ch:通道编号
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u16 Get_Adc2_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc2(ch);
		delay_ms(5);
	}
	return temp_val/times;
}  

//初始化ADC3
//使用规则通道12(PC2)作为TX发射话音输入；
//使用规则通道13(PC1)作为RX接收中频信号输入；
//我们默认仅开启ADC3_CH12P ADC3_CH13P
//16位转换时间位:采样周期+8.5个ADC周期
void  Adc3_Init(void)
{    
	//先初始化IO口
 	RCC->AHB4ENR|=1<<24;    		//使能ADC3时钟 
	RCC->AHB4ENR|=1<<2;    		//使能PORTC时钟
	GPIO_Set(GPIOC,PIN2|PIN3,GPIO_MODE_AIN,0,0,GPIO_PUPD_PD);//PC0/PC1,模拟输入,不带上下拉
	//SYSCFG->PMCR =0;
	//SYSCFG->PMCR |=1<<27;
	//SYSCFG->PMCR |=1<<26;
	RCC->AHB4RSTR|=1<<24;   		//ADC3复位
	RCC->AHB4RSTR&=~(1<<24);		//复位结束	
	//RCC->D3CCIPR&=~(3<<16);		//ADCSEL[1:0]清零
	//RCC->D3CCIPR|=0<<16;		//ADCSEL[1:0]=2,per_ck作为ADC时钟源,默认选择hsi_ker_ck作为per_ck,频率:64Mhz
	ADC3_COMMON->CCR|=3<<16;
	ADC3_COMMON->CCR|=3<<18;	//PRESC[3:0]=1,输入时钟2分频,即adc_ker_ck=per_ck/2=64/2=32Mhz(不能超过36Mhz)
	
	
	ADC3->CR=0;   				//CR寄存器清零,DEEPPWD清零,从深度睡眠唤醒.
	ADC3->CR|=1<<28;  			//ADVREGEN=1,使能ADC稳压器
	delay_ms(100);				//等待稳压器启动完成,约10us,这里延时大一点,没关系.
	ADC3->CR|=1<<8;  			//BOOST=1,ADC工作在boost模式(ADC频率大于20M的时候,必须设置boost位)
	ADC3->CFGR =0;
	ADC3->CFGR&=~(1<<16);
	ADC3->CFGR&=~(1<<13);		//CONT=0,单次转换模式
	ADC3->CFGR|=1<<12;			//OVRMOD=1,复写模式(DR寄存器可被复写)	
	ADC3->CFGR|=1<<10;			//EXTEN[1:0]=0,硬件触发
	ADC3->CFGR|=13<<5;			//EXTEN[9:5]=13,定时器6 TRGO触发
	ADC3->CFGR&=~(7<<2);		//RES[2:0]位清零
	ADC3->CFGR|=0<<2;			//RES[2:0]=0,16位分辨率(0,16位;1,14位;2,12位;3,10位;4,8位.)
	ADC3->CFGR|=3<<0;			//02：选择 DMA 单次模式
	
	ADC3->CFGR2&=~((u32)15<<28);//LSHIFT[3:0]=0,不左移,数据右对齐.
	//ADC3->CFGR2|= ((u32)2<<28);
	ADC3->CFGR2&=~((u32)0X3FF<<16);//OSR[9:0]=0,不使用过采样
	ADC3->CFGR2|= ((u32)15<<16);	//32倍过采样率（相当于21位ADC）
	ADC3->CFGR2|= ((u32)4<<5);		//右移5位
	ADC3->CFGR2|= ((u32)1<<0);		//过采样使能
	//ADC3->CFGR2|= ((u32)1<<10);
	//ADC3->CFGR2|= ((u32)1<<9);
	
	ADC3->CR&=~((u32)1<<30);	//ADCALDIF=0,校准单端转换通道
	ADC3->CR|=0<<16;			//ADCALLIN=1,线性校准
	ADC3->CR|=(u32)1<<31;		//开启校准
	while(ADC3->CR&((u32)1<<31));//等待校准完成
	 
	ADC3->SQR1&=~(0XF<<0);		//L[3:0]清零
	ADC3->SQR1|=0<<0;     		//L[3:0]=0,1个转换在规则序列中 也就是只转换规则序列1 
	/* ADC转换通道选择 */
//	ADC1->PCSEL|=1<<13;			//ADC转换通道预选择2 
//	ADC1->SQR1&=~(0X1F<<6*1);	//规则序列1通道清零
//	ADC1->SQR1|=13<<(6*1);			//设置规则序列1的转换通道为ch 
//	ADC1->CR|=1<<2;       		//启动规则转换通道 
	
	//设置通道12的采样时间
	ADC3->SMPR1&=~(7<<(3*0));	//通道12采样时间清空	  
 	ADC3->SMPR1|= 0<<(3*0); 		//通道12 16.5个周期,提高采样时间可以提高精确度
	//设置通道13的采样时间
	ADC3->SMPR1&=~(7<<(3*1));	//通道13采样时间清空	  
 	ADC3->SMPR1|= 0<(3*1); 		//通道13 16.5个周期,提高采样时间可以提高精确度
 	//ADC3->CR|=1<<0;	   			//开启AD转换器	  
}				  
//ADC通道选择
//ch:通道值 0~19
void Adc3_Channel_Selection(u8 ch)   
{
	//ADC3->CR|=1<<0;
	//while(!(ADC3->ISR&1<<0));
	ADC3->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC3->SQR1&=~(0X1F<<(6*1));	//规则序列1通道清零
	ADC3->SQR1|=ch<<(6*1);		//设置规则序列1的转换通道为ch 
	ADC3->CR|=1<<2;       		//启动规则转换通道 
	//while(!(ADC2->ISR&1<<2));	//等待转换结束	 	   
	//return ADC2->DR;			//返回adc值	
}
void Adc2_Channel_Selection(u8 ch)   
{
	//ADC1->CR|=1<<0;	
	//while(!(ADC1->ISR&1<<0));
	ADC2->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC2->SQR1&=~(0X1F<<(6*1));	//规则序列1通道清零
	ADC2->SQR1|=ch<<(6*1);		//设置规则序列1的转换通道为ch 
	ADC2->CR|=1<<2;       		//启动规则转换通道 
	//SYSCFG->PMCR =0;
	//SYSCFG->PMCR |=1<<27;
	//while(!(ADC2->ISR&1<<2));	//等待转换结束	 	   
	//return ADC2->DR;			//返回adc值	
}
//获得ADC值
//ch:通道值 0~19
//返回值:转换结果
u16 Get_Adc3(u8 ch)   
{
	ADC3->PCSEL|=1<<ch;			//ADC转换通道预选择 
	//设置转换序列	  		 
	ADC3->SQR1&=~(0X1F<<(6*1));	//规则序列1通道清零
	ADC3->SQR1|=ch<<(6*1);		//设置规则序列1的转换通道为ch 
	ADC3->CR|=1<<2;       		//启动规则转换通道 
	while(!(ADC3->ISR&1<<2));	//等待转换结束	 	   
	return ADC3->DR;			//返回adc值	
}
