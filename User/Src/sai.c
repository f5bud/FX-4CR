#include "sai.h" 
#include "Audio_driver.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//SAI ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/15
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved	
//********************************************************************************
//����˵��
//V1.1 20180817
//1,����SAIB_Init��SAIA_RX_DMA_Init��SAI_Rec_Start��SAI_Rec_Stop�Ⱥ���	
//2,����:sai_ltdc_clkset����,��������LTDCʱ��,ȷ�����޸�pll3n��ʱ��,���ı�LTDC������ʱ��
////////////////////////////////////////////////////////////////////////////////// 	
//extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff0[SAMPLE_SIZE ];
//extern  __attribute__((section (".RAM_D1"))) u16 ADC_Buff1[SAMPLE_SIZE ];
//extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff0[SAMPLE_SIZE];	
//extern  __attribute__((section (".RAM_D1"))) u16 DAC_Buff1[SAMPLE_SIZE];
 
//SAI Block A��ʼ��,I2S,�����ֱ�׼
//mode:����ģʽ,00,��������;01,��������;10,�ӷ�����;11,�ӽ�����
//cpol:0,ʱ���½���ѡͨ;1,ʱ��������ѡͨ
//datalen:���ݴ�С,0/1,δ�õ�,2,8λ;3,10λ;4,16λ;5,20λ;6,24λ;7,32λ.
void SAIA_Init(u8 mode,u8 cpol,u8 datalen)
{ 
	u32 tempreg=0;
	RCC->D2CCIP1R&=~(7<<0);		//SAI1SEL[2:0]����
	RCC->D2CCIP1R|=2<<0;		//SAI1SEL[2:0]=2,ѡ��pll3_p_ck��ΪSAI1��ʱ��Դ
	RCC->APB2ENR|=1<<22;		//ʹ��SAI1ʱ��
	RCC->APB2RSTR|=1<<22;		//��λSAI1
	RCC->APB2RSTR&=~(1<<22);	//������λ  
	tempreg|=mode<<0;			//����SAI1����ģʽ
	tempreg|=0<<2;				//����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
	tempreg|=datalen<<5;		//�������ݴ�С
	tempreg|=0<<8;				//����MSBλ����
	tempreg|=(u16)cpol<<9;		//������ʱ�ӵ�����/�½���ѡͨ
	tempreg|=0<<10;				//��Ƶģ��ͬ��
	tempreg|=0<<12;				//������ģʽ
	tempreg|=1<<13;				//����������Ƶģ�����
	tempreg|=0<<19;				//ʹ����ʱ�ӷ�Ƶ��(MCKDIV)
	SAI1_Block_A->CR1=tempreg;	//����CR1�Ĵ���
	
	tempreg=(64-1)<<0;			//����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
	tempreg|=(32-1)<<8;			//����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
	tempreg|=1<<16;				///FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
	tempreg|=0<<17;				///FS�͵�ƽ��Ч(�½���)
	tempreg|=1<<18;				//��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	 
	SAI1_Block_A->FRCR=tempreg;
	
	tempreg=0<<0;				//slotƫ��(FBOFF)Ϊ0
	tempreg|=2<<6;				//slot��СΪ32λ
	tempreg|=(2-1)<<8;			//slot��Ϊ2��
	tempreg|=(1<<17)|(1<<16);	//ʹ��slot0��slot1
	SAI1_Block_A->SLOTR=tempreg;//����slot 
	
	SAI1_Block_A->CR2=1<<0;		//����FIFO��ֵ:1/4 FIFO  
	SAI1_Block_A->CR2|=1<<3;	//FIFOˢ�� 
} 

//SAI Block B��ʼ��,I2S,�����ֱ�׼,ͬ��ģʽ
//mode:����ģʽ,00,��������;01,��������;10,�ӷ�����;11,�ӽ�����
//cpol:0,ʱ���½���ѡͨ;1,ʱ��������ѡͨ
//datalen:���ݴ�С,0/1,δ�õ�,2,8λ;3,10λ;4,16λ;5,20λ;6,24λ;7,32λ.
void SAIB_Init(u8 mode,u8 cpol,u8 datalen)
{ 
	u32 tempreg=0;  
	tempreg|=mode<<0;			//����SAI1����ģʽ
	tempreg|=0<<2;				//����SAI1Э��Ϊ:����Э��(֧��I2S/LSB/MSB/TDM/PCM/DSP��Э��)
	tempreg|=datalen<<5;		//�������ݴ�С
	tempreg|=0<<8;				//����MSBλ����
	tempreg|=(u16)cpol<<9;		//������ʱ�ӵ�����/�½���ѡͨ
	tempreg|=1<<10;				//ʹ���첽ģʽ
	tempreg|=0<<12;				//������ģʽ
	tempreg|=1<<13;				//����������Ƶģ�����
	SAI1_Block_B->CR1=tempreg;	//����CR1�Ĵ���
	
	tempreg=(64-1)<<0;			//����֡����Ϊ64,��ͨ��32��SCK,��ͨ��32��SCK.
	tempreg|=(32-1)<<8;			//����֡ͬ����Ч��ƽ����,��I2Sģʽ��=1/2֡��.
	tempreg|=1<<16;				///FS�ź�ΪSOF�ź�+ͨ��ʶ���ź�
	tempreg|=0<<17;				///FS�͵�ƽ��Ч(�½���)
	tempreg|=1<<18;				//��slot0�ĵ�һλ��ǰһλʹ��FS,��ƥ������ֱ�׼	 
	SAI1_Block_B->FRCR=tempreg;
	
	tempreg=0<<0;				//slotƫ��(FBOFF)Ϊ0
	tempreg|=2<<6;				//slot��СΪ32λ
	tempreg|=(2-1)<<8;			//slot��Ϊ2��
	tempreg|=(1<<17)|(1<<16);	//ʹ��slot0��slot1
	SAI1_Block_B->SLOTR=tempreg;//����slot 
	
	SAI1_Block_B->CR2=1<<0;		//����FIFO��ֵ:1/4 FIFO  
	SAI1_Block_B->CR2|=1<<3;	//FIFOˢ��  
	SAI1_Block_B->CR1|=1<<17;	//ʹ��DMA
	SAI1_Block_B->CR1|=1<<16;	//ʹ��SAI1 Block B
} 

//SAI Block A����������
//�����ʼ��㹫ʽ(��NOMCK=0,OSR=0Ϊǰ��):
//Fmclk=256*Fs=sai_x_ker_ck/MCKDIV[5:0]
//Fs=sai_x_ker_ck/(256*MCKDIV[5:0])
//Fsck=Fmclk*(FRL[7:0]+1)/256=Fs*(FRL[7:0]+1)
//����:
//sai_x_ker_ck=(HSE/PLL3DIVM)*(PLL3DIVN+1)/(PLL3DIVP+1)
//HSE:һ��Ϊ25Mhz
//PLL3DIVM:1~63,��ʾ1~63��Ƶ
//PLL3DIVN:3~511,��ʾ4~512��Ƶ 
//PLL3DIVP:0~127,��ʾ1~128��Ƶ 
//MCKDIV:0~63,��ʾ1~63��Ƶ,0Ҳ��1��Ƶ,�Ƽ�����Ϊż��
//SAI A��Ƶϵ����@PLL3DIVM=25,HSE=25Mhz,��vco����Ƶ��Ϊ1Mhz 
//���ʽ:
//������|(PLL3DIVN+1)|(PLL3DIVP+1)|MCKDIV
const u16 SAI_PSC_TBL[][4]=
{
	{800 ,256,5,25},	//8Khz������
	{1102,302,107,0},	//11.025Khz������ 
	{1600,426,2,52},	//16Khz������
	{2205,429,38,2},	//22.05Khz������
	{3200,426,1,52},	//32Khz������
	{4410,429,1,38},	//44.1Khz������
	{4800,467,1,38},	//48Khz������
	{8820,429,1,19},	//88.2Khz������
	{9600,467,1,19},	//96Khz������
	{17640,271,1,6},	//176.4Khz������ 
	{19200,295,6,0},	//192Khz������
};
/*
/////////////////////////////////////////////////////////////////////////////////////
#include "ltdc.h"
//�ֱ���:����ʱ�Ӷ�Ӧ��,ǰ��Ϊ����ֱ���,����ΪĿ������ʱ��Ƶ��
const u16 ltdc_clk_table[4][2]=
{
	480,9,
	800,33,
	1024,50,
	1280,50,
};*/
//ר�����LTDC��һ��ʱ�����ú���
//pll3n,��ǰ��pll3��Ƶϵ��.
//void sai_ltdc_clkset(u16 pll3n)
//{
//	u8 i=0;
//	u8 pll3r=0;
//	if(lcdltdc.pwidth==0)return;	//����RGB��,����Ҫ����
//	for(i=0;i<4;i++)				//�����Ƿ��ж�ӦRGB���ķֱ���
//	{
//		if(lcdltdc.pwidth==ltdc_clk_table[i][0])break;	//�ҵ�����ֱ���˳�
//	}
//	if(i==4)return;					//�ұ���Ҳû�ҵ���Ӧ�ķֱ���
//	pll3r=pll3n/ltdc_clk_table[i][1];//�õ�������pll3r��ֵ
//	if(pll3n>(pll3r*ltdc_clk_table[i][1]))pll3r+=1;	
//	//LTDC_Clk_Set(pll3n,25,pll3r);	//��������PLL3��R��Ƶ.
//}
/////////////////////////////////////////////////////////////////////////////////////

//����SAIA�Ĳ�����(@NOMCK=0,OSR=0)
//samplerate:������,��λ:Hz
//����ֵ:0,���óɹ�;1,�޷�����.
u8 SAIA_SampleRate_Set(u32 samplerate)
{ 
	u16 retry=0; 
	u8 i=0; 
	u32 tempreg=0;
	samplerate/=10;//��С10��    
	for(i=0;i<(sizeof(SAI_PSC_TBL)/8);i++)	//�����Ĳ������Ƿ����֧��
	{
		if(samplerate==SAI_PSC_TBL[i][0])break;
	} 
	if(i==(sizeof(SAI_PSC_TBL)/8))return 1;	//�ѱ���Ҳ�Ҳ���
	RCC->CR&=~(1<<28);						//�ر�PLL3ʱ�� 
	while(((RCC->CR&(1<<29)))&&(retry<0X1FFF))retry++;//�ȴ�PLL3ʱ��ʧ��
	RCC->PLLCKSELR&=~(0X3F<<20);			//���DIVM3[5:0]ԭ��������
	RCC->PLLCKSELR|=25<<20;					//DIVM3[5:0]=25,����PLL3��Ԥ��Ƶϵ��
	tempreg=RCC->PLL3DIVR;					//��ȡPLL3DIVR��ֵ
	tempreg&=0XFFFF0000;					//���DIVN��PLL3DIVPԭ��������
	tempreg|=(u32)(SAI_PSC_TBL[i][1]-1)<<0;	//����DIVN[8:0]
	tempreg|=(u32)(SAI_PSC_TBL[i][2]-1)<<9;	//����DIVP[6:0]
	RCC->PLL3DIVR=tempreg;					//����PLL3DIVR�Ĵ���
	RCC->PLLCFGR|=1<<22;					//DIVP3EN=1,ʹ��pll3_p_ck  
	RCC->CR|=1<<28;							//����PLL3
	while((RCC->CR&1<<29)==0);				//�ȴ�PLL3�����ɹ�. 

	tempreg=SAI1_Block_A->CR1;			
	tempreg&=~(0X3F<<20);					//���MCKDIV[5:0]����
	tempreg|=(u32)SAI_PSC_TBL[i][3]<<20;	//����MCKDIV[5:0]
	//tempreg|=1<<17;							//ʹ��DMA
	//tempreg|=1<<16;							//ʹ��SAI1 Block A
	SAI1_Block_A->CR1=tempreg;				//����MCKDIV[5:0],ͬʱʹ��SAI1 Block A 
	SAI1_Block_A->CR1|=1<<17;	//ʹ��DMA
	SAI1_Block_A->CR1|=1<<16;	//ʹ��SAI1 Block B
	//sai_ltdc_clkset(SAI_PSC_TBL[i][1]);		//���RGB��,��Ҫ�����޸�PLL3R��ֵ
	return 0;
}  
//SAIA TX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
//width:λ��(�洢��������,ͬʱ����),0,8λ;1,16λ;2,32λ;
void SAIA_TX_DMA_Init(u32* buf0,u32* buf1,u16 num,u8 width)
{   
	RCC->AHB1ENR|=1<<1;			//DMA2ʱ��ʹ��  
	RCC->D3AMR|=1<<0;			//DMAMUXʱ��ʹ�� 
	while(DMA2_Stream3->CR&0X01);//�ȴ�DMA2_Stream1������
	DMAMUX1_Channel11->CCR=87;	//DMA2_Stream3��ͨ��ѡ��: 87,��SAI1_A��Ӧ��ͨ��
								//���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 119
									
	DMA2->LIFCR|=0X3D<<22;		//���ͨ��3�������жϱ�־
	DMA2_Stream3->FCR=0X0000021;//����ΪĬ��ֵ	
	
	DMA2_Stream3->PAR=(u32)&SAI1_Block_A->DR;//�����ַΪ:SAI1_Block_A->DR
	DMA2_Stream3->M0AR=(u32)buf0;//�ڴ�1��ַ
	DMA2_Stream3->M1AR=(u32)buf1;//�ڴ�2��ַ
	DMA2_Stream3->NDTR=num;		//��ʱ���ó���Ϊ1
	DMA2_Stream3->CR=0;			//��ȫ����λCR�Ĵ���ֵ  
	DMA2_Stream3->CR|=1<<6;		//�洢��������ģʽ 
	DMA2_Stream3->CR|=1<<8;		//ѭ��ģʽ
	DMA2_Stream3->CR|=0<<9;		//���������ģʽ
	DMA2_Stream3->CR|=1<<10;	//�洢������ģʽ
	DMA2_Stream3->CR|=(u16)width<<11;//�������ݳ���:16λ/32λ
	DMA2_Stream3->CR|=(u16)width<<13;//�洢�����ݳ���:16λ/32λ
	DMA2_Stream3->CR|=2<<16;	//�����ȼ�
	DMA2_Stream3->CR|=1<<18;	//˫����ģʽ
	DMA2_Stream3->CR|=0<<21;	//����ͻ�����δ���
	DMA2_Stream3->CR|=0<<23;	//�洢��ͻ�����δ���
	DMA2_Stream3->CR|=0<<25;	//ѡ��ͨ��0 SAI1_Aͨ�� 

	DMA2_Stream3->FCR&=~(1<<2);	//��ʹ��FIFOģʽ
	DMA2_Stream3->FCR&=~(3<<0);	//��FIFO ����
	
	//DMA2_Stream3->CR|=1<<4;		//������������ж�
	//MY_NVIC_Init(5,0,DMA2_Stream3_IRQn,0);	//��ռ1�������ȼ�0����2  
}  

//SAIB RX DMA����
//����Ϊ˫����ģʽ,������DMA��������ж�
//buf0:M0AR��ַ.
//buf1:M1AR��ַ.
//num:ÿ�δ���������
//width:λ��(�洢��������,ͬʱ����),0,8λ;1,16λ;2,32λ;
void SAIB_RX_DMA_Init(u32* buf0,u32* buf1,u16 num,u8 width)
{  

	RCC->AHB1ENR|=1<<1;			//DMA2ʱ��ʹ��  
	RCC->D3AMR|=1<<0;			//DMAMUXʱ��ʹ�� 
	while(DMA2_Stream5->CR&0X01);//�ȴ�DMA2_Stream5������
	DMAMUX1_Channel13->CCR=88;	//DMA2_Stream5��ͨ��ѡ��: 88,��SAI1_B��Ӧ��ͨ��
								//���<<STM32H7xx�ο��ֲ�>>16.3.2��,Table 119
	
	DMA2->HIFCR|=0X3D<<6*1;		//���ͨ��5�������жϱ�־
	DMA2_Stream5->FCR=0X0000021;//����ΪĬ��ֵ	
	
	DMA2_Stream5->PAR=(u32)&SAI1_Block_B->DR;//�����ַΪ:SAI1_Block_B->DR
	DMA2_Stream5->M0AR=(u32)buf0;//�ڴ�1��ַ
	DMA2_Stream5->M1AR=(u32)buf1;//�ڴ�2��ַ
	DMA2_Stream5->NDTR=num;		//��ʱ���ó���Ϊ1
	DMA2_Stream5->CR=0;			//��ȫ����λCR�Ĵ���ֵ  
	DMA2_Stream5->CR|=0<<6;		//���赽�洢��ģʽ 
	DMA2_Stream5->CR|=1<<8;		//ѭ��ģʽ
	DMA2_Stream5->CR|=0<<9;		//���������ģʽ
	DMA2_Stream5->CR|=1<<10;	//�洢������ģʽ
	DMA2_Stream5->CR|=(u16)width<<11;//�������ݳ���:16λ/32λ
	DMA2_Stream5->CR|=(u16)width<<13;//�洢�����ݳ���:16λ/32λ
	DMA2_Stream5->CR|=1<<16;	//�е����ȼ�
	DMA2_Stream5->CR|=1<<18;	//˫����ģʽ
	DMA2_Stream5->CR|=0<<21;	//����ͻ�����δ���
	DMA2_Stream5->CR|=0<<23;	//�洢��ͻ�����δ���
	DMA2_Stream5->CR|=0<<25;	//ѡ��ͨ��0 SAI1_Bͨ�� 

	DMA2_Stream5->FCR&=~(1<<2);	//��ʹ��FIFOģʽ
	DMA2_Stream5->FCR&=~(3<<0);	//��FIFO ����
	
	DMA2_Stream5->CR|=1<<4;		//������������ж�
	MY_NVIC_Init(15,0,DMA2_Stream5_IRQn,4);	//��ռ1�������ȼ�1����2  
} 

//SAI DMA�ص�����ָ��
void (*sai_tx_callback)(void);	//TX�ص����� 
void (*sai_rx_callback)(void);	//RX�ص�����

////DMA2_Stream3�жϷ�����
//void DMA2_Stream3_IRQHandler(void)
//{      
//	if(DMA2->LISR&(1<<27))			//DMA2_Steam3,������ɱ�־
//	{ 
//		DMA2->LIFCR|=1<<27;			//�����������ж�
//      	sai_tx_callback();			//ִ�лص�����,��ȡ���ݵȲ����������洦�� 
//		SCB_CleanInvalidateDCache();//�����Ч��D-Cache 
//	}   											 
//}  
////DMA2_Stream5�жϷ�����
//void DMA2_Stream5_IRQHandler(void)
//{      
//	if(DMA2->HISR&(1<<11))			//DMA2_Steam5,������ɱ�־
//	{ 
//		DMA2->HIFCR|=1<<11;			//�����������ж�
//      	sai_rx_callback();			//ִ�лص�����,��ȡ���ݵȲ����������洦��  
//		SCB_CleanInvalidateDCache();//�����Ч��D-Cache
//	}   											 
//}  
//SAI��ʼ����
void SAI_Play_Start(void)
{   	  
	DMA2_Stream3->CR|=1<<0;		//����DMA TX����  		
}
//�ر�I2S����
void SAI_Play_Stop(void)
{   	 
	DMA2_Stream3->CR&=~(1<<0);	//��������	 	 
} 
//SAI��ʼ¼��
void SAI_Rec_Start(void)
{   	    
	DMA2_Stream5->CR|=1<<0;		//����DMA RX���� 		
}
//�ر�SAI¼��
void SAI_Rec_Stop(void)
{   	  
	DMA2_Stream5->CR&=~(1<<0);	//����¼��		 
}





