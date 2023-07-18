#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//FX-4C(STM32H750VBT6)专用
//系统时钟初始化	
//包括时钟设置/中断管理/GPIO设置等
//BG2IXD
//创建日期:2020/9/8
//版本：V1.1
//版权所有，盗版必究。
//********************************************************************************
//修改说明
//V1.1 20180806
//1,修改PLL2的倍频系数为440,pll2_r_ck=220Mhz
//2,新增对pll2_p_ck的分频设置,2分频,得到220M的时钟频率
//3,修改pll1_q_ck的注释,改为4分频,200Mhz的频率说明
//////////////////////////////////////////////////////////////////////////////////  


//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量		 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}
//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//取低四位
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//设置响应优先级和抢断优先级   	    	  				   
} 
//外部中断配置函数,只针对GPIOA~GPIOK 
//参数:
//GPIOx:0~10,代表GPIOA~GPIOK
//BITx:0~15,代表IO引脚编号.
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTOFFSET=(BITx%4)*4;  
	RCC->APB4ENR|=1<<1;								//SYSCFGEN=1,使能SYSCFG时钟  
	SYSCFG->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);	//清除原来设置！！！
	SYSCFG->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;		//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI_D1->IMR1|=1<<BITx;				//开启line BITx上的中断(如果要禁止中断，则反操作即可)
	if(TRIM&0x01)EXTI->FTSR1|=1<<BITx;	//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR1|=1<<BITx;	//line BITx上事件上升降沿触发
} 	
//GPIO复用设置
//GPIOx:GPIOA~GPIOK.
//BITx:0~15,代表IO引脚编号.
//AFx:0~15,代表AF0~AF15.
//AF0~15设置情况(这里仅是列出常用的,详细的请见STM32H743xx数据手册,Table 9~19):
//AF0:MCO/SWD/SWCLK/RTC;   		AF1:TIM1/2/TIM16/17/LPTIM1;		AF2:TIM3~5/TIM12/HRTIM1/SAI1;	AF3:TIM8/LPTIM2~5/HRTIM1/LPUART1;
//AF4:I2C1~I2C4/TIM15/USART1;	AF5:SPI1~SPI6/CEC;         		AF6:SPI3/SAI1~3/UART4/I2C4; 	AF7:SPI2/3/6/USART1~3/6/UART7/SDIO1;
//AF8:USART4/5/8/SPDIF/SAI2/4;	AF9;FDCAN1~2/TIM13/14/LCD/QSPI; AF10:USB_OTG1/2/SAI2/4/QSPI;  	AF11:ETH/UART7/SDIO2/I2C4/COMP1/2;
//AF12:FMC/SDIO1/OTG2/LCD;		AF13:DCIM/DSI/LCD/COMP1/2; 		AF14:LCD/UART5;					AF15:EVENTOUT;
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
}   
//GPIO通用设置 
//GPIOx:GPIOA~GPIOK.
//BITx:0X0000~0XFFFF,位设置,每个位代表一个IO,第0位代表Px0,第1位代表Px1,依次类推.比如0X0101,代表同时设置Px0和Px8.
//MODE:0~3;模式选择,0,输入(系统复位默认状态);1,普通输出;2,复用功能;3,模拟输入.
//OTYPE:0/1;输出类型选择,0,推挽输出;1,开漏输出.
//OSPEED:0~3;输出速度设置,0,低速;1,中速;2,快速;3,高速. 
//PUPD:0~3:上下拉设置,0,不带上下拉;1,上拉;2,下拉;3,保留.
//注意:在输入模式(普通输入/模拟输入)下,OTYPE和OSPEED参数无效!!
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//一个个位检查 
		curpin=BITx&pos;//检查引脚是否要设置
		if(curpin==pos)	//需要设置
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->MODER|=MODE<<(pinpos*2);	//设置新的模式 
			if((MODE==0X01)||(MODE==0X02))	//如果是输出模式/复用功能模式
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//清除原来的设置
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//设置新的速度值  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//清除原来的设置
				GPIOx->OTYPER|=OTYPE<<pinpos;		//设置新的输出模式
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//先清除原来的设置
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//设置新的上下拉
		}
	}
} 
//设置GPIO某个引脚的输出状态
//GPIOx:GPIOA~GPIOK.
//pinx:引脚位置,范围:1<<0 ~ 1<<15
//status:引脚状态(仅最低位有效),0,输出低电平;1,输出高电平
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status)
{
	if(status&0X01)GPIOx->BSRR=pinx<<0;	//设置GPIOx的pinx为1
	else GPIOx->BSRR=pinx<<16;				//设置GPIOx的pinx为0
}
//读取GPIO某个引脚的状态
//GPIOx:GPIOA~GPIOK.
//pinx:引脚位置,范围:1<<0 ~ 1<<15
//返回值:引脚状态,0,引脚低电平;1,引脚高电平
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx)
{ 
	if(GPIOx->IDR&pinx)return 1;		//pinx的状态为1
	else return 0;						//pinx的状态为0
}
//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//关闭所有中断(但是不包括fault和NMI中断)
void INTX_DISABLE(void)
{
	__ASM volatile("cpsid i");
}
//开启所有中断
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//设置栈顶地址
//addr:栈顶地址
void MSR_MSP(u32 addr)
{
	asm("MSR MSP, r0"); 			//set Main Stack value
	asm("BX r14");
}
#if daniel
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
#endif
//进入待机模式	  
void Sys_Standby(void)
{ 
 	PWR->WKUPEPR|=1<<0;		//WKUPEN0=1,PA0用于WKUP唤醒
 	PWR->WKUPEPR&=~(1<<8);	//WKUPP0=0,PA0高电平唤醒(上升沿)	
 	PWR->WKUPEPR&=~(3<<16);	//清除WKUPPUPD原来的设置
	PWR->WKUPEPR|=2<<16;	//WKUPPUPD=10,PA0下拉 
	PWR->WKUPCR|=0X3F<<0;	//清除WKUP0~5唤醒标志
	PWR->CPUCR|=7<<0;		//PDDS_D1/D2/D3=1,允许D1/D2/D3进入深度睡眠模式(PDDS) 
	SCB->SCR|=1<<2;			//使能SLEEPDEEP位 (SYS->CTRL)	 
	WFI_SET();				//执行WFI指令,进入待机模式		 
}
//系统软复位   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 
//使能STM32H7的L1-Cache,同时开启D cache的强制透写
void Cache_Enable(void)
{
    SCB_EnableICache();	//使能I-Cache,函数在core_cm7.h里面定义
    SCB_EnableDCache();	//使能D-Cache,函数在core_cm7.h里面定义 
	SCB->CACR|=1<<2;	//强制D-Cache透写,如不开启透写,实际使用中可能遇到各种问题
}
//时钟设置函数
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//Fsys:系统时钟频率,也是PLL1的p分频输出时钟频率
//Fq:PLL1的q分频输出时钟频率
//Fs:PLL输入时钟频率,可以是HSI,CSI,HSE等. 

//plln:PLL1倍频系数(PLL倍频),取值范围:4~512.
//pllm:PLL1预分频系数(进PLL之前的分频),取值范围:2~63.
//pllp:PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟,取值范围:2~128.(且必须是2的倍数)
//pllq:PLL1的q分频系数(PLL之后的分频),取值范围:1~128.

//CPU频率(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//pll2_p_ck=(25/25)*440/2)=220Mhz
//pll2_r_ck=FMC时钟频率=((25/25)*440/2)=110Mhz

//外部晶振为25M的时候,推荐值:plln=160,pllm=5,pllp=2,pllq=4.
//得到:Fvco=25*(160/5)=800Mhz
//     Fsys=pll1_p_ck=800/2=400Mhz
//     Fq=pll1_q_ck=800/4=200Mhz
//返回值:0,成功;1,失败。
u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{ 
	u16 retry=0;
	u8 status=0;
	
	PWR->CR3&=~(1<<2);				//SCUEN=0,锁定LDOEN和BYPASS位的设置
	PWR->D3CR|=3<<14;				//VOS=3,Scale1,1.15~1.26V内核电压,FLASH访问可以得到最高性能
	while((PWR->D3CR&(1<<13))==0);	//等待电压稳定 
	RCC->CR|=1<<16;					//HSEON=1,开启HSE
	while(((RCC->CR&(1<<17))==0)&&(retry<0X7FFF))retry++;//等待HSE RDY
	if(retry==0X7FFF)status=1;		//HSE无法就绪
	else   
	{
		RCC->PLLCKSELR|=2<<0;		//PLLSRC[1:0]=2,选择HSE作为PLL的输入时钟源
		RCC->PLLCKSELR|=pllm<<4;	//DIVM1[5:0]=pllm,设置PLL1的预分频系数
		RCC->PLL1DIVR|=(plln-1)<<0;	//DIVN1[8:0]=plln-1,设置PLL1的倍频系数,设置值需减1
		RCC->PLL1DIVR|=(pllp-1)<<9;	//DIVP1[6:0]=pllp-1,设置PLL1的p分频系数,设置值需减1
		RCC->PLL1DIVR|=(pllq-1)<<16;//DIVQ1[6:0]=pllq-1,设置PLL1的q分频系数,设置值需减1
		RCC->PLL1DIVR|=1<<24;		//DIVR1[6:0]=pllr-1,设置PLL1的r分频系数,设置值需减1,r分频出来的时钟没用到
		RCC->PLLCFGR|=2<<2;			//PLL1RGE[1:0]=2,PLL1输入时钟频率在4~8Mhz之间(25/5=5Mhz),如修改pllm,请确认此参数
		RCC->PLLCFGR|=0<<1;			//PLL1VCOSEL=0,PLL1宽的VCO范围,192~836Mhz
		RCC->PLLCFGR|=3<<16;		//DIVP1EN=1,DIVQ1EN=1,使能pll1_p_ck和pll1_q_ck
		RCC->CR|=1<<24;				//PLL1ON=1,使能PLL1
		while((RCC->CR&(1<<25))==0);//PLL1RDY=1?,等待PLL1准备好  
	
		//设置PLL2的R分频输出,为220Mhz,后续做SDRAM时钟,可得到110M的SDRAM时钟频率
		RCC->PLLCKSELR|=8<<12;		//DIVM2[5:0]=25,设置PLL2的预分频系数
		RCC->PLL2DIVR|=(440-1)<<0;	//DIVN2[8:0]=440-1,设置PLL2的倍频系数,设置值需减1
		RCC->PLL2DIVR|=(2-1)<<9;	//DIVP2[6:0]=2-1,设置PLL2的p分频系数,设置值需减1
		RCC->PLL2DIVR|=(2-1)<<24;	//DIVR2[6:0]=2-1,设置PLL2的r分频系数,设置值需减1
		RCC->PLLCFGR|=0<<6;			//PLL2RGE[1:0]=0,PLL2输入时钟频率在1~2Mhz之间(25/25=1Mhz)
		RCC->PLLCFGR|=0<<5;			//PLL2VCOSEL=0,PLL2宽的VCO范围,192~836Mhz
		RCC->PLLCFGR|=1<<19;		//DIVP2EN=1,使能pll2_p_ck
		RCC->PLLCFGR|=1<<21;		//DIVR2EN=1,使能pll2_r_ck
		RCC->D1CCIPR&=~(3<<0);		//清除FMCSEL[1:0]原来的设置
		RCC->D1CCIPR|=2<<0;			//FMCSEL[1:0]=2,FMC时钟来自于pll2_r_ck		
		RCC->CR|=1<<26;				//PLL2ON=1,使能PLL2
		while((RCC->CR&(1<<27))==0);//PLL2RDY=1?,等待PLL2准备好  
	
		RCC->D1CFGR|=8<<0;			//HREF[3:0]=8,rcc_hclk1/2/3/4=sys_d1cpre_ck/2=400/2=200Mhz,即AHB1/2/3/4=200Mhz
		RCC->D1CFGR|=0<<8;			//D1CPRE[2:0]=0,sys_d1cpre_ck=sys_clk/1=400/1=400Mhz,即CPU时钟=400Mhz
		RCC->CFGR|=3<<0;			//SW[2:0]=3,系统时钟(sys_clk)选择来自pll1_p_ck,即400Mhz
		while(1)
		{
			retry=(RCC->CFGR&(7<<3))>>3;	//获取SWS[2:0]的状态,判断是否切换成功
			if(retry==3)break;		//成功将系统时钟源切换为pll1_p_ck
		}
			
		FLASH->ACR|=3<<0;			//LATENCY[2:0]=2,2个CPU等待周期(@VOS1 Level,maxclock=210Mhz)
		FLASH->ACR|=2<<4;			//WRHIGHFREQ[1:0]=2,flash访问频率<285Mhz
		
		RCC->D1CFGR|=4<<4;			//D1PPRE[2:0]=4,rcc_pclk3=rcc_hclk3/2=100Mhz,即APB3=100Mhz
		RCC->D2CFGR|=4<<4;			//D2PPRE1[2:0]=4,rcc_pclk1=rcc_hclk1/2=100Mhz,即APB1=100Mhz
		RCC->D2CFGR|=4<<8;			//D2PPRE2[2:0]=4,rcc_pclk2=rcc_hclk1/2=100Mhz,即APB2=100Mhz
		RCC->D3CFGR|=4<<4;			//D3PPRE[2:0]=4,rcc_pclk4=rcc_hclk4/2=100Mhz,即APB4=100Mhz
 
		RCC->CR|=1<<7;				//CSION=1,使能CSI,为IO补偿单元提供时钟
		RCC->APB4ENR|=1<<1;			//SYSCFGEN=1,使能SYSCFG时钟
		SYSCFG->CCCSR|=1<<0;		//EN=1,使能IO补偿单元 
	} 
	return status;
}  

//系统时钟初始化函数
//plln:PLL1倍频系数(PLL倍频),取值范围:4~512.
//pllm:PLL1预分频系数(进PLL之前的分频),取值范围:2~63.
//pllp:PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟,取值范围:2~128.(且必须是2的倍数)
//pllq:PLL1的q分频系数(PLL之后的分频),取值范围:2~128.
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{  
	/* FPU settings ------------------------------------------------------------*/
//  #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
//    SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
//  #endif
//  /* Reset the RCC clock configuration to the default reset state ------------*/
//  /* Set HSION bit */
//  RCC->CR |= RCC_CR_HSION;
//  
//  /* Reset CFGR register */
//  RCC->CFGR = 0x00000000;

//  /* Reset HSEON, CSSON , CSION,RC48ON, CSIKERON PLL1ON, PLL2ON and PLL3ON bits */
//  RCC->CR &= (uint32_t)0xEAF6ED7F;

//  /* Reset D1CFGR register */
//  RCC->D1CFGR = 0x00000000;

//  /* Reset D2CFGR register */
//  RCC->D2CFGR = 0x00000000;
//  
//  /* Reset D3CFGR register */
//  RCC->D3CFGR = 0x00000000;

//  /* Reset PLLCKSELR register */
//  RCC->PLLCKSELR = 0x00000000;

//  /* Reset PLLCFGR register */
//  RCC->PLLCFGR = 0x00000000;
//  /* Reset PLL1DIVR register */
//  RCC->PLL1DIVR = 0x00000000;
//  /* Reset PLL1FRACR register */
//  RCC->PLL1FRACR = 0x00000000;

//  /* Reset PLL2DIVR register */
//  RCC->PLL2DIVR = 0x00000000;

//  /* Reset PLL2FRACR register */
//  
//  RCC->PLL2FRACR = 0x00000000;
//  /* Reset PLL3DIVR register */
//  RCC->PLL3DIVR = 0x00000000;

//  /* Reset PLL3FRACR register */
//  RCC->PLL3FRACR = 0x00000000;
//  
//  /* Reset HSEBYP bit */
//  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Disable all interrupts */
    RCC->CIER = 0x00000000;
	RCC->CR=0x00000001;				//设置HISON,开启内部高速RC振荡，其他位全清零
	RCC->CFGR=0x00000000;			//CFGR清零 
	RCC->D1CFGR=0x00000000;			//D1CFGR清零 
	RCC->D2CFGR=0x00000000;			//D2CFGR清零 
	RCC->D3CFGR=0x00000000;			//D3CFGR清零 
	RCC->PLLCKSELR=0x00000000;		//PLLCKSELR清零 
	RCC->PLLCFGR=0x00000000;		//PLLCFGR清零 
	RCC->CIER=0x00000000;			//CIER清零,禁止所有RCC相关中断  
	//AXI_TARG7_FN_MOD寄存器,由于没有在stm32h743xx.h里面定义,所以,只能用直接
	//操作地址的方式,来修改,该寄存器在<<STM32H7xx参考手册>>第119页,table 5
	//*((vu32*)0x51008108)=0x00000001;//设置AXI SRAM的矩阵读取能力为1 
	Cache_Enable();					//使能L1 Cache
	Sys_Clock_Set(plln,pllm,pllp,pllq);//设置时钟 
	//配置向量表				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(D1_AXISRAM_BASE,0x0);
#else   
	MY_NVIC_SetVectorTable(FLASH_BANK1_BASE,0x0);
#endif 
}		    




























