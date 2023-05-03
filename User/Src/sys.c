#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//FX-4C(STM32H750VBT6)ר��
//ϵͳʱ�ӳ�ʼ��	
//����ʱ������/�жϹ���/GPIO���õ�
//BG2IXD
//��������:2020/9/8
//�汾��V1.1
//��Ȩ���У�����ؾ���
//********************************************************************************
//�޸�˵��
//V1.1 20180806
//1,�޸�PLL2�ı�Ƶϵ��Ϊ440,pll2_r_ck=220Mhz
//2,������pll2_p_ck�ķ�Ƶ����,2��Ƶ,�õ�220M��ʱ��Ƶ��
//3,�޸�pll1_q_ck��ע��,��Ϊ4��Ƶ,200Mhz��Ƶ��˵��
//////////////////////////////////////////////////////////////////////////////////  


//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����		 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]������
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//ȡ����λ
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//������Ӧ���ȼ����������ȼ�   	    	  				   
} 
//�ⲿ�ж����ú���,ֻ���GPIOA~GPIOK 
//����:
//GPIOx:0~10,����GPIOA~GPIOK
//BITx:0~15,����IO���ű��.
//TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTOFFSET=(BITx%4)*4;  
	RCC->APB4ENR|=1<<1;								//SYSCFGEN=1,ʹ��SYSCFGʱ��  
	SYSCFG->EXTICR[BITx/4]&=~(0x000F<<EXTOFFSET);	//���ԭ�����ã�����
	SYSCFG->EXTICR[BITx/4]|=GPIOx<<EXTOFFSET;		//EXTI.BITxӳ�䵽GPIOx.BITx 
	//�Զ�����
	EXTI_D1->IMR1|=1<<BITx;				//����line BITx�ϵ��ж�(���Ҫ��ֹ�жϣ��򷴲�������)
	if(TRIM&0x01)EXTI->FTSR1|=1<<BITx;	//line BITx���¼��½��ش���
	if(TRIM&0x02)EXTI->RTSR1|=1<<BITx;	//line BITx���¼��������ش���
} 	
//GPIO��������
//GPIOx:GPIOA~GPIOK.
//BITx:0~15,����IO���ű��.
//AFx:0~15,����AF0~AF15.
//AF0~15�������(��������г����õ�,��ϸ�����STM32H743xx�����ֲ�,Table 9~19):
//AF0:MCO/SWD/SWCLK/RTC;   		AF1:TIM1/2/TIM16/17/LPTIM1;		AF2:TIM3~5/TIM12/HRTIM1/SAI1;	AF3:TIM8/LPTIM2~5/HRTIM1/LPUART1;
//AF4:I2C1~I2C4/TIM15/USART1;	AF5:SPI1~SPI6/CEC;         		AF6:SPI3/SAI1~3/UART4/I2C4; 	AF7:SPI2/3/6/USART1~3/6/UART7/SDIO1;
//AF8:USART4/5/8/SPDIF/SAI2/4;	AF9;FDCAN1~2/TIM13/14/LCD/QSPI; AF10:USB_OTG1/2/SAI2/4/QSPI;  	AF11:ETH/UART7/SDIO2/I2C4/COMP1/2;
//AF12:FMC/SDIO1/OTG2/LCD;		AF13:DCIM/DSI/LCD/COMP1/2; 		AF14:LCD/UART5;					AF15:EVENTOUT;
void GPIO_AF_Set(GPIO_TypeDef* GPIOx,u8 BITx,u8 AFx)
{  
	GPIOx->AFR[BITx>>3]&=~(0X0F<<((BITx&0X07)*4));
	GPIOx->AFR[BITx>>3]|=(u32)AFx<<((BITx&0X07)*4);
}   
//GPIOͨ������ 
//GPIOx:GPIOA~GPIOK.
//BITx:0X0000~0XFFFF,λ����,ÿ��λ����һ��IO,��0λ����Px0,��1λ����Px1,��������.����0X0101,����ͬʱ����Px0��Px8.
//MODE:0~3;ģʽѡ��,0,����(ϵͳ��λĬ��״̬);1,��ͨ���;2,���ù���;3,ģ������.
//OTYPE:0/1;�������ѡ��,0,�������;1,��©���.
//OSPEED:0~3;����ٶ�����,0,����;1,����;2,����;3,����. 
//PUPD:0~3:����������,0,����������;1,����;2,����;3,����.
//ע��:������ģʽ(��ͨ����/ģ������)��,OTYPE��OSPEED������Ч!!
void GPIO_Set(GPIO_TypeDef* GPIOx,u32 BITx,u32 MODE,u32 OTYPE,u32 OSPEED,u32 PUPD)
{  
	u32 pinpos=0,pos=0,curpin=0;
	for(pinpos=0;pinpos<16;pinpos++)
	{
		pos=1<<pinpos;	//һ����λ��� 
		curpin=BITx&pos;//��������Ƿ�Ҫ����
		if(curpin==pos)	//��Ҫ����
		{
			GPIOx->MODER&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->MODER|=MODE<<(pinpos*2);	//�����µ�ģʽ 
			if((MODE==0X01)||(MODE==0X02))	//��������ģʽ/���ù���ģʽ
			{  
				GPIOx->OSPEEDR&=~(3<<(pinpos*2));	//���ԭ��������
				GPIOx->OSPEEDR|=(OSPEED<<(pinpos*2));//�����µ��ٶ�ֵ  
				GPIOx->OTYPER&=~(1<<pinpos) ;		//���ԭ��������
				GPIOx->OTYPER|=OTYPE<<pinpos;		//�����µ����ģʽ
			}  
			GPIOx->PUPDR&=~(3<<(pinpos*2));	//�����ԭ��������
			GPIOx->PUPDR|=PUPD<<(pinpos*2);	//�����µ�������
		}
	}
} 
//����GPIOĳ�����ŵ����״̬
//GPIOx:GPIOA~GPIOK.
//pinx:����λ��,��Χ:1<<0 ~ 1<<15
//status:����״̬(�����λ��Ч),0,����͵�ƽ;1,����ߵ�ƽ
void GPIO_Pin_Set(GPIO_TypeDef* GPIOx,u16 pinx,u8 status)
{
	if(status&0X01)GPIOx->BSRR=pinx<<0;	//����GPIOx��pinxΪ1
	else GPIOx->BSRR=pinx<<16;				//����GPIOx��pinxΪ0
}
//��ȡGPIOĳ�����ŵ�״̬
//GPIOx:GPIOA~GPIOK.
//pinx:����λ��,��Χ:1<<0 ~ 1<<15
//����ֵ:����״̬,0,���ŵ͵�ƽ;1,���Ÿߵ�ƽ
u8 GPIO_Pin_Get(GPIO_TypeDef* GPIOx,u16 pinx)
{ 
	if(GPIOx->IDR&pinx)return 1;		//pinx��״̬Ϊ1
	else return 0;						//pinx��״̬Ϊ0
}
//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
void INTX_DISABLE(void)
{
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
void MSR_MSP(u32 addr)
{
	asm("MSR MSP, r0"); 			//set Main Stack value
	asm("BX r14");
}
//�������ģʽ	  
void Sys_Standby(void)
{ 
 	PWR->WKUPEPR|=1<<0;		//WKUPEN0=1,PA0����WKUP����
 	PWR->WKUPEPR&=~(1<<8);	//WKUPP0=0,PA0�ߵ�ƽ����(������)	
 	PWR->WKUPEPR&=~(3<<16);	//���WKUPPUPDԭ��������
	PWR->WKUPEPR|=2<<16;	//WKUPPUPD=10,PA0���� 
	PWR->WKUPCR|=0X3F<<0;	//���WKUP0~5���ѱ�־
	PWR->CPUCR|=7<<0;		//PDDS_D1/D2/D3=1,����D1/D2/D3�������˯��ģʽ(PDDS) 
	SCB->SCR|=1<<2;			//ʹ��SLEEPDEEPλ (SYS->CTRL)	 
	WFI_SET();				//ִ��WFIָ��,�������ģʽ		 
}
//ϵͳ��λ   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 
//ʹ��STM32H7��L1-Cache,ͬʱ����D cache��ǿ��͸д
void Cache_Enable(void)
{
    SCB_EnableICache();	//ʹ��I-Cache,������core_cm7.h���涨��
    SCB_EnableDCache();	//ʹ��D-Cache,������core_cm7.h���涨�� 
	SCB->CACR|=1<<2;	//ǿ��D-Cache͸д,�粻����͸д,ʵ��ʹ���п���������������
}
//ʱ�����ú���
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCOƵ��
//Fsys:ϵͳʱ��Ƶ��,Ҳ��PLL1��p��Ƶ���ʱ��Ƶ��
//Fq:PLL1��q��Ƶ���ʱ��Ƶ��
//Fs:PLL����ʱ��Ƶ��,������HSI,CSI,HSE��. 

//plln:PLL1��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:4~512.
//pllm:PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��,ȡֵ��Χ:2~128.(�ұ�����2�ı���)
//pllq:PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:1~128.

//CPUƵ��(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//pll2_p_ck=(25/25)*440/2)=220Mhz
//pll2_r_ck=FMCʱ��Ƶ��=((25/25)*440/2)=110Mhz

//�ⲿ����Ϊ25M��ʱ��,�Ƽ�ֵ:plln=160,pllm=5,pllp=2,pllq=4.
//�õ�:Fvco=25*(160/5)=800Mhz
//     Fsys=pll1_p_ck=800/2=400Mhz
//     Fq=pll1_q_ck=800/4=200Mhz
//����ֵ:0,�ɹ�;1,ʧ�ܡ�
u8 Sys_Clock_Set(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{ 
	u16 retry=0;
	u8 status=0;
	
	PWR->CR3&=~(1<<2);				//SCUEN=0,����LDOEN��BYPASSλ������
	PWR->D3CR|=3<<14;				//VOS=3,Scale1,1.15~1.26V�ں˵�ѹ,FLASH���ʿ��Եõ��������
	while((PWR->D3CR&(1<<13))==0);	//�ȴ���ѹ�ȶ� 
	RCC->CR|=1<<16;					//HSEON=1,����HSE
	while(((RCC->CR&(1<<17))==0)&&(retry<0X7FFF))retry++;//�ȴ�HSE RDY
	if(retry==0X7FFF)status=1;		//HSE�޷�����
	else   
	{
		RCC->PLLCKSELR|=2<<0;		//PLLSRC[1:0]=2,ѡ��HSE��ΪPLL������ʱ��Դ
		RCC->PLLCKSELR|=pllm<<4;	//DIVM1[5:0]=pllm,����PLL1��Ԥ��Ƶϵ��
		RCC->PLL1DIVR|=(plln-1)<<0;	//DIVN1[8:0]=plln-1,����PLL1�ı�Ƶϵ��,����ֵ���1
		RCC->PLL1DIVR|=(pllp-1)<<9;	//DIVP1[6:0]=pllp-1,����PLL1��p��Ƶϵ��,����ֵ���1
		RCC->PLL1DIVR|=(pllq-1)<<16;//DIVQ1[6:0]=pllq-1,����PLL1��q��Ƶϵ��,����ֵ���1
		RCC->PLL1DIVR|=1<<24;		//DIVR1[6:0]=pllr-1,����PLL1��r��Ƶϵ��,����ֵ���1,r��Ƶ������ʱ��û�õ�
		RCC->PLLCFGR|=2<<2;			//PLL1RGE[1:0]=2,PLL1����ʱ��Ƶ����4~8Mhz֮��(25/5=5Mhz),���޸�pllm,��ȷ�ϴ˲���
		RCC->PLLCFGR|=0<<1;			//PLL1VCOSEL=0,PLL1���VCO��Χ,192~836Mhz
		RCC->PLLCFGR|=3<<16;		//DIVP1EN=1,DIVQ1EN=1,ʹ��pll1_p_ck��pll1_q_ck
		RCC->CR|=1<<24;				//PLL1ON=1,ʹ��PLL1
		while((RCC->CR&(1<<25))==0);//PLL1RDY=1?,�ȴ�PLL1׼����  
	
		//����PLL2��R��Ƶ���,Ϊ220Mhz,������SDRAMʱ��,�ɵõ�110M��SDRAMʱ��Ƶ��
		RCC->PLLCKSELR|=8<<12;		//DIVM2[5:0]=25,����PLL2��Ԥ��Ƶϵ��
		RCC->PLL2DIVR|=(440-1)<<0;	//DIVN2[8:0]=440-1,����PLL2�ı�Ƶϵ��,����ֵ���1
		RCC->PLL2DIVR|=(2-1)<<9;	//DIVP2[6:0]=2-1,����PLL2��p��Ƶϵ��,����ֵ���1
		RCC->PLL2DIVR|=(2-1)<<24;	//DIVR2[6:0]=2-1,����PLL2��r��Ƶϵ��,����ֵ���1
		RCC->PLLCFGR|=0<<6;			//PLL2RGE[1:0]=0,PLL2����ʱ��Ƶ����1~2Mhz֮��(25/25=1Mhz)
		RCC->PLLCFGR|=0<<5;			//PLL2VCOSEL=0,PLL2���VCO��Χ,192~836Mhz
		RCC->PLLCFGR|=1<<19;		//DIVP2EN=1,ʹ��pll2_p_ck
		RCC->PLLCFGR|=1<<21;		//DIVR2EN=1,ʹ��pll2_r_ck
		RCC->D1CCIPR&=~(3<<0);		//���FMCSEL[1:0]ԭ��������
		RCC->D1CCIPR|=2<<0;			//FMCSEL[1:0]=2,FMCʱ��������pll2_r_ck		
		RCC->CR|=1<<26;				//PLL2ON=1,ʹ��PLL2
		while((RCC->CR&(1<<27))==0);//PLL2RDY=1?,�ȴ�PLL2׼����  
	
		RCC->D1CFGR|=8<<0;			//HREF[3:0]=8,rcc_hclk1/2/3/4=sys_d1cpre_ck/2=400/2=200Mhz,��AHB1/2/3/4=200Mhz
		RCC->D1CFGR|=0<<8;			//D1CPRE[2:0]=0,sys_d1cpre_ck=sys_clk/1=400/1=400Mhz,��CPUʱ��=400Mhz
		RCC->CFGR|=3<<0;			//SW[2:0]=3,ϵͳʱ��(sys_clk)ѡ������pll1_p_ck,��400Mhz
		while(1)
		{
			retry=(RCC->CFGR&(7<<3))>>3;	//��ȡSWS[2:0]��״̬,�ж��Ƿ��л��ɹ�
			if(retry==3)break;		//�ɹ���ϵͳʱ��Դ�л�Ϊpll1_p_ck
		}
			
		FLASH->ACR|=3<<0;			//LATENCY[2:0]=2,2��CPU�ȴ�����(@VOS1 Level,maxclock=210Mhz)
		FLASH->ACR|=2<<4;			//WRHIGHFREQ[1:0]=2,flash����Ƶ��<285Mhz
		
		RCC->D1CFGR|=4<<4;			//D1PPRE[2:0]=4,rcc_pclk3=rcc_hclk3/2=100Mhz,��APB3=100Mhz
		RCC->D2CFGR|=4<<4;			//D2PPRE1[2:0]=4,rcc_pclk1=rcc_hclk1/2=100Mhz,��APB1=100Mhz
		RCC->D2CFGR|=4<<8;			//D2PPRE2[2:0]=4,rcc_pclk2=rcc_hclk1/2=100Mhz,��APB2=100Mhz
		RCC->D3CFGR|=4<<4;			//D3PPRE[2:0]=4,rcc_pclk4=rcc_hclk4/2=100Mhz,��APB4=100Mhz
 
		RCC->CR|=1<<7;				//CSION=1,ʹ��CSI,ΪIO������Ԫ�ṩʱ��
		RCC->APB4ENR|=1<<1;			//SYSCFGEN=1,ʹ��SYSCFGʱ��
		SYSCFG->CCCSR|=1<<0;		//EN=1,ʹ��IO������Ԫ 
	} 
	return status;
}  

//ϵͳʱ�ӳ�ʼ������
//plln:PLL1��Ƶϵ��(PLL��Ƶ),ȡֵ��Χ:4~512.
//pllm:PLL1Ԥ��Ƶϵ��(��PLL֮ǰ�ķ�Ƶ),ȡֵ��Χ:2~63.
//pllp:PLL1��p��Ƶϵ��(PLL֮��ķ�Ƶ),��Ƶ����Ϊϵͳʱ��,ȡֵ��Χ:2~128.(�ұ�����2�ı���)
//pllq:PLL1��q��Ƶϵ��(PLL֮��ķ�Ƶ),ȡֵ��Χ:2~128.
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
	RCC->CR=0x00000001;				//����HISON,�����ڲ�����RC�񵴣�����λȫ����
	RCC->CFGR=0x00000000;			//CFGR���� 
	RCC->D1CFGR=0x00000000;			//D1CFGR���� 
	RCC->D2CFGR=0x00000000;			//D2CFGR���� 
	RCC->D3CFGR=0x00000000;			//D3CFGR���� 
	RCC->PLLCKSELR=0x00000000;		//PLLCKSELR���� 
	RCC->PLLCFGR=0x00000000;		//PLLCFGR���� 
	RCC->CIER=0x00000000;			//CIER����,��ֹ����RCC����ж�  
	//AXI_TARG7_FN_MOD�Ĵ���,����û����stm32h743xx.h���涨��,����,ֻ����ֱ��
	//������ַ�ķ�ʽ,���޸�,�üĴ�����<<STM32H7xx�ο��ֲ�>>��119ҳ,table 5
	//*((vu32*)0x51008108)=0x00000001;//����AXI SRAM�ľ����ȡ����Ϊ1 
	Cache_Enable();					//ʹ��L1 Cache
	Sys_Clock_Set(plln,pllm,pllp,pllq);//����ʱ�� 
	//����������				  
#ifdef  VECT_TAB_RAM
	MY_NVIC_SetVectorTable(D1_AXISRAM_BASE,0x0);
#else   
	MY_NVIC_SetVectorTable(FLASH_BANK1_BASE,0x0);
#endif 
}		    




























