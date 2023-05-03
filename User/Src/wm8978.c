#include "wm8978.h"
#include "myiic.h"
#include "dwt.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//WM8978 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//WM8978�Ĵ���ֵ������(�ܹ�58���Ĵ���,0~57),ռ��116�ֽ��ڴ�
//��ΪWM8978��IIC������֧�ֶ�����,�����ڱ��ر������мĴ���ֵ
//дWM8978�Ĵ���ʱ,ͬ�����µ����ؼĴ���ֵ,���Ĵ���ʱ,ֱ�ӷ��ر��ر���ļĴ���ֵ.
//ע��:WM8978�ļĴ���ֵ��9λ��,����Ҫ��u16���洢. 
static u16 WM8978_REGVAL_TBL[58]=
{
	0X0000,0X0000,0X0000,0X0000,0X0050,0X0000,0X0140,0X0000,
	0X0000,0X0000,0X0000,0X00FF,0X00FF,0X0000,0X0100,0X00FF,
	0X00FF,0X0000,0X012C,0X002C,0X002C,0X002C,0X002C,0X0000,
	0X0032,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,0X0000,
	0X0038,0X000B,0X0032,0X0000,0X0008,0X000C,0X0093,0X00E9,
	0X0000,0X0000,0X0000,0X0000,0X0003,0X0010,0X0010,0X0100,
	0X0100,0X0002,0X0001,0X0001,0X0039,0X0039,0X0039,0X0039,
	0X0001,0X0001
}; 
//WM8978��ʼ��
//����ֵ:0,��ʼ������
//    ����,�������
u8 WM8978_Init(void)
{
	u8 res;
	u16 regval;
 	RCC->AHB4ENR|=1<<4;			//ʹ������PORTEʱ��   
 	GPIO_Set(GPIOE,PIN2|PIN3|PIN4|PIN5|PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);	//PE2~6 ���ù������
 	GPIO_AF_Set(GPIOE,2,6);		//PE2,AF6  SAI_MCLKA
	GPIO_AF_Set(GPIOE,3,6);		//PE3,AF6  SAI_SDB    WM8978 I2S ADC���������
	GPIO_AF_Set(GPIOE,4,6);		//PE4,AF6  SAI_FSA
	GPIO_AF_Set(GPIOE,5,6);		//PE5,AF6  SAI_SCKA
 	GPIO_AF_Set(GPIOE,6,6);		//PE6,AF6  SAI_SDA  
	
	IIC_Init();//��ʼ��IIC�ӿ�
	res=WM8978_Write_Reg(0,0);	//��λWM8978
	delay_ms(200);
	if(res)return 1;			//����ָ��ʧ��,WM8978�쳣
	//����Ϊͨ������
	WM8978_Write_Reg(1,0X1CF);	//R1,MICEN����Ϊ1(MICʹ��),BIASEN����Ϊ1(ģ��������),VMIDSEL[1:0]����Ϊ:11(5K)
	WM8978_Write_Reg(2,0X1BF);	//R2,ROUT1,LOUT1���ʹ��(�������Թ���),BOOSTENR,BOOSTENLʹ��
	WM8978_Write_Reg(3,0X1EF);	//R3,LOUT2,ROUT2���ʹ��(���ȹ���),RMIX,LMIXʹ��	LOUT3,ROUT4���ʹ��(TX I Q����),
	WM8978_Write_Reg(6,0);		//R6,MCLK���ⲿ�ṩ
	WM8978_Write_Reg(43,1<<4);	//R43,INVROUT2����,��������
	
	regval =0;
	regval &=~(1<<8);
	//regval |= 1<<8;
	WM8978_Write_Reg(47,regval);	//R47����,PGABOOSTL,��ͨ��MIC���20������
	WM8978_Write_Reg(48,regval);	//R48����,PGABOOSTR,��ͨ��MIC���20������
	regval =0;
	regval |= 1<<1;//R49,TSDEN,�������ȱ���
	regval |= 1<<2;//R49,SPEAKER BOOST,1.5x 
	regval |= 1<<3;
	regval |= 1<<4;
	WM8978_Write_Reg(49,regval);	//R49,TSDEN,�������ȱ��� 
	//WM8978_Write_Reg(49,1<<2);	//R49,SPEAKER BOOST,1.5x 
	WM8978_Write_Reg(10,1<<3);	//R10,SOFTMUTE�ر�,128x����,���SNR 
	WM8978_Write_Reg(14,1<<3);	//R14,ADC 128x������
	//WM8978_Write_Reg(24,1<<8);	//�����޷���ʹ�� 
	//WM8978_Write_Reg(25,2<<0);	//�����޷�����ʼ
	
	WM8978_out34_Cfg(1); 
	WM8978_ADC_Gain(255);
	WM8978_DAC_Gain(255);
	WM8978_I2S_Cfg(2,2);		//�����ֱ�׼,32λ���ݳ���
	// WM8978_HPread_Cfg();
	WM8978_AUXIN_Cfg(7);
	return 0;
} 
//WM8978д�Ĵ���
//reg:�Ĵ�����ַ
//val:Ҫд��Ĵ�����ֵ 
//����ֵ:0,�ɹ�;
//    ����,�������
u8 WM8978_Write_Reg(u8 reg,u16 val)
{ 
	IIC_Start(); 
	IIC_Send_Byte((WM8978_ADDR<<1)|0);//����������ַ+д����	 
	if(IIC_Wait_Ack())return 1;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC_Send_Byte((reg<<1)|((val>>8)&0X01));//д�Ĵ�����ַ+���ݵ����λ
	if(IIC_Wait_Ack())return 2;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
	IIC_Send_Byte(val&0XFF);	//��������
	if(IIC_Wait_Ack())return 3;	//�ȴ�Ӧ��(�ɹ�?/ʧ��?) 
    IIC_Stop();
	WM8978_REGVAL_TBL[reg]=val;	//����Ĵ���ֵ������
	return 0;	
}  
//WM8978���Ĵ���
//���Ƕ�ȡ���ؼĴ���ֵ�������ڵĶ�Ӧֵ
//reg:�Ĵ�����ַ 
//����ֵ:�Ĵ���ֵ
u16 WM8978_Read_Reg(u8 reg)
{  
	return WM8978_REGVAL_TBL[reg];	
}
//
//WM8978�������
//
void WM8978_HPread_Cfg(void)
{
	u8 regval;
//	regval &=~(0x1ff);
//	regval |= 0<<0;
//	WM8978_Write_Reg(8,regval);//
	regval=WM8978_Read_Reg(9);	
	regval &=~(0x1ff);
	//regval &=~(1<<6);
	//regval |= 1<<6;
	//regval |= 3<<7;
	WM8978_Write_Reg(9,regval);//
	regval=WM8978_Read_Reg(13);
	regval &=~(0x1ff);
	regval |= 1<<0;
	regval |= 3<<2;
	//regval |= 1<<5;
	regval |= 3<<6;
	WM8978_Write_Reg(13,0X0ED);//
}
//WM8978 AUX_IN ����
//auxr>0��ͨ 0~7 -15db~6db 
//auxl>0��ͨ 0~7  
void WM8978_AUXIN_Cfg(u8 auxr)
{
	u8 regval;
	
	if(auxr>0)
	{
		regval=WM8978_Read_Reg(51);	//��ȡR51
		regval &=~(0XF<<5);
		//regval |=  1<<5;
		
		regval |=  auxr<<6|1<<5;
		//regval |=  auxr<<2|1<<1;
		WM8978_Write_Reg(51,regval);//AUX_R
	}
	else
	{
		regval=WM8978_Read_Reg(51);	//��ȡR3
		regval &=~(0XF<<0);
		WM8978_Write_Reg(51,regval);//AUX_R
	}
}
//WM8978 OUT3/4 ����
void WM8978_out34_Cfg(u8 outen)
{
	u8 regval;
	
	regval=WM8978_Read_Reg(56);	//��ȡR3
	regval &=~(1<<0);
	if(outen)regval |=  1<<0;
	WM8978_Write_Reg(56,regval);//L_DAC����OUT3
	//
	regval=WM8978_Read_Reg(57);	//��ȡR3
	regval &=~(1<<0);
	if(outen)regval |=  1<<0;
	WM8978_Write_Reg(57,regval);//R_DAC����OUT4	
}
//WM8978 DAC/ADC����
//adcen:adcʹ��(1)/�ر�(0)
//dacen:dacʹ��(1)/�ر�(0)
void WM8978_ADDA_Cfg(u8 dacen,u8 adcen)
{
	u16 regval;
	regval=WM8978_Read_Reg(3);	//��ȡR3
	regval&=~(3<<0);
	if(dacen)regval|=3<<0;		//R3���2��λ����Ϊ1,����DACR&DACL
	else regval&=~(3<<0);		//R3���2��λ����,�ر�DACR&DACL.
	WM8978_Write_Reg(3,regval);	//����R3
	regval=WM8978_Read_Reg(2);	//��ȡR2
	regval&=~(3<<0);
	if(adcen)regval|=3<<0;		//R2���2��λ����Ϊ1,����ADCR&ADCL
	else regval&=~(3<<0);		//R2���2��λ����,�ر�ADCR&ADCL.
	WM8978_Write_Reg(2,regval);	//����R2
	//WM8978_Write_Reg(14,0x100);
	WM8978_Input_Cfg(0,0);//����ͨ��(MIC&LINE IN)
	WM8978_Output_Cfg(0,0);		//����BYPASS��� 
	WM8978_MIC_Gain(63);//MIC��������,�������ALC���棬��������ָ���������
}
//WM8978 ����ʱ����ͨ������ 
//micen:MIC����(1)/�ر�(0)
//lineinen:Line In����(1)/�ر�(0)
//auxen:aux����(1)/�ر�(0) 
void WM8978_Input_Cfg(u8 micen_l,u8 micen_r)
{
	//static u8 clk;
	u16 regval;
	//if(clk==0)
	//{		
		//regval=WM8978_Read_Reg(44);	//��ȡR44
		regval&=~((7<<4)|(7<<0));
		if(micen_l==1)regval|=(3<<0);	//����LIN1INPPGA,LIP1INPGA,RIN1INPPGA,RIP1INPGA.
		else regval|=(3<<1);	//����LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
		
		if(micen_r==1)regval|=3<<4;
		else regval|=3<<5;	//�ر�LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
		WM8978_Write_Reg(44,regval);//����R44 
	//}
}

//WM8978 DAC���� 
//dacen:DAC���(����)����(1)/�ر�(0)
void WM8978_Output_Cfg(u8 dac_l,u8 dac_r)
{
	static u8 regval=0;
	regval=WM8978_Read_Reg(50);
	regval &=~(1<<0);
	if(dac_l==0)regval |=1<<0;
	else regval &=~(1<<0);
	WM8978_Write_Reg(50,regval);//R50����
	regval=WM8978_Read_Reg(51);
	regval &=~(1<<0);
	if(dac_r==0)regval |=1<<0;
	else regval &=~(1<<0);
	WM8978_Write_Reg(51,regval);//R50����
}
//WM8978 MIC��������(����BOOST��20dB,MIC-->ADC���벿�ֵ�����)
//gain:0~89,��Ӧ-12dB~55.25dB,0.75dB/Step
void WM8978_MIC_Gain(u8 gain)
{
	static u8 clk;
	u16 regval;
	if(clk==0)
	{
		//gain&=0X3F;
		//regval=WM8978_Read_Reg(45);
		regval &=~(0x3F<<0);
		regval |= gain<<0;
		WM8978_Write_Reg(45,regval<<0|1<<8);	//R45,��ͨ��PGA����
		//WM8978_Write_Reg(45,1<<8);
		//regval=WM8978_Read_Reg(46);
		regval &=~(0x3F<<0);
		regval |= gain<<0;
		WM8978_Write_Reg(46,regval<<0|1<<8);	//R46,��ͨ��PGA����
		//WM8978_Write_Reg(46,1<<8);
	}
}
//WM8978_ALC����
//
void WM8978_ALC_Cfg(u8 sel,u8 gain)
{
	u16 alc;
	//alc=WM8978_Read_Reg(32);
	alc=0;
	if(sel==0)alc&=~(3<<7);//0:ALC_OFF,1:ALC_L=ON,2:ALC_R=ON;3:L/R=ON
	else alc |=sel<<7;
	alc|=7<<3;
	alc&=~(7<<0);
	
	WM8978_Write_Reg(32,alc);
	
	//alc=WM8978_Read_Reg(33);
	alc=0;
	alc|=gain<<0;//ALCĿ�ꡪ��ADC�����źŵı�׼0000=-28.5dB FS0001=-27.0dB FS��������Ϊ 1.5dB��1110=-7.5dB FS1111=-6dB FS
	alc|=7<<4;//��������ǰ�� ALC ����ʱ��0000=0ms0001=2.67ms0010=5.33ms����ÿһ������ʱ��ӱ���
	alc|=1<<8;
	WM8978_Write_Reg(33,alc);
	
	//alc=WM8978_Read_Reg(34);
	alc=0;
	alc|=7<<4;//
	//alc|=0<<0;//
	WM8978_Write_Reg(34,alc);
	//WM8978_Write_Reg(70,1<<8);
}
//WM8978 ADC��������(DAC����������)
//gain:0~255,��Ӧ-127dB~0dB,0.5dB/Step
void WM8978_ADC_Gain(u8 gain)
{
	//gain&=0Xff;
	
 	WM8978_Write_Reg(15,gain);//����R11
 	WM8978_Write_Reg(16,gain);//����R12
}
//WM8978 DAC��������(DAC����������)
//gain:0~255,��Ӧ-127dB~0dB,0.5dB/Step
void WM8978_DAC_Gain(u8 gain)
{
	gain&=0Xff;
 	WM8978_Write_Reg(11,gain);//����R11
 	WM8978_Write_Reg(12,gain|1<<8);//����R12
}
//WM8978 L2/R2(Ҳ����Line In)��������(L2/R2-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_LINEIN_Gain(u8 gain)
{
	u16 regval;
	gain&=0X07;
	regval=WM8978_Read_Reg(47);	//��ȡR47
	regval&=~(7<<4);			//���ԭ��������
	regval |= gain<<4;	
 	WM8978_Write_Reg(47,regval);//����R47
	regval=WM8978_Read_Reg(48);	//��ȡR48
	regval&=~(7<<4);	//���ԭ�������� 
	regval |= gain<<4;
 	WM8978_Write_Reg(48,regval);//����R48
} 
//WM8978 AUXR,AUXL(PWM��Ƶ����)��������(AUXR/L-->ADC���벿�ֵ�����)
//gain:0~7,0��ʾͨ����ֹ,1~7,��Ӧ-12dB~6dB,3dB/Step
void WM8978_AUX_Gain(u8 aux_l,u8 aux_r)
{
	u16 regval;
	
	regval=WM8978_Read_Reg(47);	//��ȡR47
	regval&=~(7<<0);	//���ԭ��������
	regval |= aux_l<<0;
 	WM8978_Write_Reg(47,regval);//����R47
	regval=WM8978_Read_Reg(48);	//��ȡR48
	regval&=~(7<<0);	//���ԭ��������
	regval |= aux_r<<0;
 	WM8978_Write_Reg(48,regval);//����R48
}  
//����I2S����ģʽ
//fmt:0,LSB(�Ҷ���);1,MSB(�����);2,�����ֱ�׼I2S;3,PCM/DSP;
//len:0,16λ;1,20λ;2,24λ;3,32λ;  
void WM8978_I2S_Cfg(u8 fmt,u8 len)
{
	fmt&=0X03;
	len&=0X03;//�޶���Χ
	WM8978_Write_Reg(4,(fmt<<3)|(len<<5));	//R4,WM8978����ģʽ����	
}	

//���ö���������������
//voll:����������(0~63)
//volr:����������(0~63)
void WM8978_HPvol_Set(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//�޶���Χ
	if(voll==0)voll|=1<<6;//����Ϊ0ʱ,ֱ��mute
	if(volr==0)volr|=1<<6;//����Ϊ0ʱ,ֱ��mute 
	WM8978_Write_Reg(52,voll|1<<8);			//R52,������������������
	WM8978_Write_Reg(53,volr|1<<8);	//R53,������������������,ͬ������(HPVU=1)
}
//������������
//voll:����������(0~63) 
void WM8978_SPKvol_Set(u8 volx)
{ 
	volx&=0X3F;//�޶���Χ
	if(volx==0)volx|=1<<6;//����Ϊ0ʱ,ֱ��mute 
 	WM8978_Write_Reg(54,volx|1<<8);			//R54,������������������
	WM8978_Write_Reg(55,volx|1<<8);	//R55,������������������,ͬ������(SPKVU=1)	
}
void WM8978_Inpga_muter(u8 l_mute,u8 r_mute)
{
	u16 regval;
	regval &=~(1<<6);
	if(l_mute)regval |=1<<6;
	else regval &=~(1<<6);
	WM8978_Write_Reg(45,regval);
	
	regval &=~(1<<6);
	if(r_mute)regval |=1<<6;
	else regval &=~(1<<6);
	WM8978_Write_Reg(46,regval);
}
//RX/TX�շ�����
//0: RX
//1: TX_CW
//2: TX_SSB/AM/FM
void WM8978_TR_set(u8 trvolue,u8 gain,u8 alc)
{
	if(trvolue ==0)
	{
		WM8978_ALC_Cfg(0,15);
		WM8978_AUXIN_Cfg(7);
		WM8978_Input_Cfg(1,1);//����ͨ��(MIC&LINE IN)
		WM8978_Output_Cfg(0,0);		//����BYPASS��� 
		WM8978_MIC_Gain(gain&0x3f);//MIC��������,�������ALC���棬��������ָ���������
		WM8978_Inpga_muter(0,0);
		WM8978_AUX_Gain(0,0);
	}
	if(trvolue ==1)
	{
		//WM8978_MIC_Gain(15);
		//WM8978_Output_Cfg(1,1);
		//WM8978_AUXIN_Cfg(0);
	}
	if(trvolue ==2)
	{
		WM8978_MIC_Gain(gain&0x3f);
		WM8978_Input_Cfg(0,0);//����ͨ��(MIC&LINE IN)
		WM8978_ALC_Cfg(3,alc);
		WM8978_Output_Cfg(1,1);		//����BYPASS��� 
	}
}












