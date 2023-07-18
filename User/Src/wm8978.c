#include "wm8978.h"
#include "myiic.h"
#include "dwt.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//WM8978 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/15
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//WM8978寄存器值缓存区(总共58个寄存器,0~57),占用116字节内存
//因为WM8978的IIC操作不支持读操作,所以在本地保存所有寄存器值
//写WM8978寄存器时,同步更新到本地寄存器值,读寄存器时,直接返回本地保存的寄存器值.
//注意:WM8978的寄存器值是9位的,所以要用u16来存储. 
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
//WM8978初始化
//返回值:0,初始化正常
//    其他,错误代码
u8 WM8978_Init(void)
{
	u8 res;
	u16 regval;
 	RCC->AHB4ENR|=1<<4;			//使能外设PORTE时钟   
 	GPIO_Set(GPIOE,PIN2|PIN3|PIN4|PIN5|PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);	//PE2~6 复用功能输出
 	GPIO_AF_Set(GPIOE,2,6);		//PE2,AF6  SAI_MCLKA
	GPIO_AF_Set(GPIOE,3,6);		//PE3,AF6  SAI_SDB    WM8978 I2S ADC数据输出脚
	GPIO_AF_Set(GPIOE,4,6);		//PE4,AF6  SAI_FSA
	GPIO_AF_Set(GPIOE,5,6);		//PE5,AF6  SAI_SCKA
 	GPIO_AF_Set(GPIOE,6,6);		//PE6,AF6  SAI_SDA  
	
	IIC_Init();//初始化IIC接口
	res=WM8978_Write_Reg(0,0);	//软复位WM8978
	delay_ms(200);
	if(res)return 1;			//发送指令失败,WM8978异常
	//以下为通用设置
	WM8978_Write_Reg(1,0X1CF);	//R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)
	WM8978_Write_Reg(2,0X1BF);	//R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
	WM8978_Write_Reg(3,0X1EF);	//R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能	LOUT3,ROUT4输出使能(TX I Q工作),
	WM8978_Write_Reg(6,0);		//R6,MCLK由外部提供
	WM8978_Write_Reg(43,1<<4);	//R43,INVROUT2反向,驱动喇叭
	
	regval =0;
	regval &=~(1<<8);
	//regval |= 1<<8;
	WM8978_Write_Reg(47,regval);	//R47设置,PGABOOSTL,左通道MIC获得20倍增益
	WM8978_Write_Reg(48,regval);	//R48设置,PGABOOSTR,右通道MIC获得20倍增益
	regval =0;
	regval |= 1<<1;//R49,TSDEN,开启过热保护
	regval |= 1<<2;//R49,SPEAKER BOOST,1.5x 
	regval |= 1<<3;
	regval |= 1<<4;
	WM8978_Write_Reg(49,regval);	//R49,TSDEN,开启过热保护 
	//WM8978_Write_Reg(49,1<<2);	//R49,SPEAKER BOOST,1.5x 
	WM8978_Write_Reg(10,1<<3);	//R10,SOFTMUTE关闭,128x采样,最佳SNR 
	WM8978_Write_Reg(14,1<<3);	//R14,ADC 128x采样率
	//WM8978_Write_Reg(24,1<<8);	//数字限幅器使能 
	//WM8978_Write_Reg(25,2<<0);	//数字限幅器起始
	
	WM8978_out34_Cfg(1); 
	WM8978_ADC_Gain(255);
	WM8978_DAC_Gain(255);
	WM8978_I2S_Cfg(2,2);		//飞利浦标准,32位数据长度
	// WM8978_HPread_Cfg();
	WM8978_AUXIN_Cfg(7);
	return 0;
} 
//WM8978写寄存器
//reg:寄存器地址
//val:要写入寄存器的值 
//返回值:0,成功;
//    其他,错误代码
u8 WM8978_Write_Reg(u8 reg,u16 val)
{ 
	IIC_Start(); 
	IIC_Send_Byte((WM8978_ADDR<<1)|0);//发送器件地址+写命令	 
	if(IIC_Wait_Ack())return 1;	//等待应答(成功?/失败?) 
    IIC_Send_Byte((reg<<1)|((val>>8)&0X01));//写寄存器地址+数据的最高位
	if(IIC_Wait_Ack())return 2;	//等待应答(成功?/失败?) 
	IIC_Send_Byte(val&0XFF);	//发送数据
	if(IIC_Wait_Ack())return 3;	//等待应答(成功?/失败?) 
    IIC_Stop();
	WM8978_REGVAL_TBL[reg]=val;	//保存寄存器值到本地
	return 0;	
}  
//WM8978读寄存器
//就是读取本地寄存器值缓冲区内的对应值
//reg:寄存器地址 
//返回值:寄存器值
u16 WM8978_Read_Reg(u8 reg)
{  
	return WM8978_REGVAL_TBL[reg];	
}
//
//WM8978耳机检测
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
//WM8978 AUX_IN 配置
//auxr>0接通 0~7 -15db~6db 
//auxl>0接通 0~7  
void WM8978_AUXIN_Cfg(u8 auxr)
{
	u8 regval;
	
	if(auxr>0)
	{
		regval=WM8978_Read_Reg(51);	//读取R51
		regval &=~(0XF<<5);
		//regval |=  1<<5;
		
		regval |=  auxr<<6|1<<5;
		//regval |=  auxr<<2|1<<1;
		WM8978_Write_Reg(51,regval);//AUX_R
	}
	else
	{
		regval=WM8978_Read_Reg(51);	//读取R3
		regval &=~(0XF<<0);
		WM8978_Write_Reg(51,regval);//AUX_R
	}
}
//WM8978 OUT3/4 配置
void WM8978_out34_Cfg(u8 outen)
{
	u8 regval;
	
	regval=WM8978_Read_Reg(56);	//读取R3
	regval &=~(1<<0);
	if(outen)regval |=  1<<0;
	WM8978_Write_Reg(56,regval);//L_DAC连接OUT3
	//
	regval=WM8978_Read_Reg(57);	//读取R3
	regval &=~(1<<0);
	if(outen)regval |=  1<<0;
	WM8978_Write_Reg(57,regval);//R_DAC连接OUT4	
}
//WM8978 DAC/ADC配置
//adcen:adc使能(1)/关闭(0)
//dacen:dac使能(1)/关闭(0)
void WM8978_ADDA_Cfg(u8 dacen,u8 adcen)
{
	u16 regval;
	regval=WM8978_Read_Reg(3);	//读取R3
	regval&=~(3<<0);
	if(dacen)regval|=3<<0;		//R3最低2个位设置为1,开启DACR&DACL
	else regval&=~(3<<0);		//R3最低2个位清零,关闭DACR&DACL.
	WM8978_Write_Reg(3,regval);	//设置R3
	regval=WM8978_Read_Reg(2);	//读取R2
	regval&=~(3<<0);
	if(adcen)regval|=3<<0;		//R2最低2个位设置为1,开启ADCR&ADCL
	else regval&=~(3<<0);		//R2最低2个位清零,关闭ADCR&ADCL.
	WM8978_Write_Reg(2,regval);	//设置R2
	//WM8978_Write_Reg(14,0x100);
	WM8978_Input_Cfg(0,0);//输入通道(MIC&LINE IN)
	WM8978_Output_Cfg(0,0);		//开启BYPASS输出 
	WM8978_MIC_Gain(63);//MIC增益设置,必须放在ALC后面，否则增益恢复不起作用
}
//WM8978 发射时输入通道配置 
//micen:MIC开启(1)/关闭(0)
//lineinen:Line In开启(1)/关闭(0)
//auxen:aux开启(1)/关闭(0) 
void WM8978_Input_Cfg(u8 micen_l,u8 micen_r)
{
	//static u8 clk;
	u16 regval;
	//if(clk==0)
	//{		
		//regval=WM8978_Read_Reg(44);	//读取R44
		regval&=~((7<<4)|(7<<0));
		if(micen_l==2)regval&=~(7<<0);
		else
		if(micen_l==1)regval|=(3<<0);	//开启LIN1INPPGA,LIP1INPGA,RIN1INPPGA,RIP1INPGA.
		else 
		if(micen_l==0)regval|=(3<<1);	//开启LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
		
		if(micen_r==2)regval&=~(7<<4);
		else
		if(micen_r==1)regval|=3<<4;
		else 
		if(micen_r==0)regval|=3<<5;	//关闭LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
		WM8978_Write_Reg(44,regval);//设置R44 
	//}
}

//WM8978 DAC配置 
//dacen:DAC输出(放音)开启(1)/关闭(0)
void WM8978_Output_Cfg(u8 dac_l,u8 dac_r)
{
	static u8 regval=0;
	regval=WM8978_Read_Reg(50);
	regval &=~(1<<0);
	if(dac_l==0)regval |=1<<0;
	else regval &=~(1<<0);
	WM8978_Write_Reg(50,regval);//R50设置
	regval=WM8978_Read_Reg(51);
	regval &=~(1<<0);
	if(dac_r==0)regval |=1<<0;
	else regval &=~(1<<0);
	WM8978_Write_Reg(51,regval);//R50设置
}
//WM8978 MIC增益设置(包括BOOST的20dB,MIC-->ADC输入部分的增益)
//gain:0~89,对应-12dB~55.25dB,0.75dB/Step
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
		WM8978_Write_Reg(45,regval<<0|1<<8);	//R45,左通道PGA设置
		//WM8978_Write_Reg(45,1<<8);
		//regval=WM8978_Read_Reg(46);
		regval &=~(0x3F<<0);
		regval |= gain<<0;
		WM8978_Write_Reg(46,regval<<0|1<<8);	//R46,右通道PGA设置
		//WM8978_Write_Reg(46,1<<8);
	}
}
//WM8978_ALC配置
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
	alc|=gain<<0;//ALC目标——ADC输入信号的标准0000=-28.5dB FS0001=-27.0dB FS…（步调为 1.5dB）1110=-7.5dB FS1111=-6dB FS
	alc|=7<<4;//增益增加前的 ALC 保持时间0000=0ms0001=2.67ms0010=5.33ms…（每一个步调时间加倍）
	alc|=1<<8;
	WM8978_Write_Reg(33,alc);
	
	//alc=WM8978_Read_Reg(34);
	alc=0;
	alc|=7<<4;//
	//alc|=0<<0;//
	WM8978_Write_Reg(34,alc);
	//WM8978_Write_Reg(70,1<<8);
}
//WM8978 ADC增益设置(DAC的数字增益)
//gain:0~255,对应-127dB~0dB,0.5dB/Step
void WM8978_ADC_Gain(u8 gain)
{
	//gain&=0Xff;
	
 	WM8978_Write_Reg(15,gain);//设置R11
 	WM8978_Write_Reg(16,gain);//设置R12
}
//WM8978 DAC增益设置(DAC的数字增益)
//gain:0~255,对应-127dB~0dB,0.5dB/Step
void WM8978_DAC_Gain(u8 gain)
{
	gain&=0Xff;
 	WM8978_Write_Reg(11,gain);//设置R11
 	WM8978_Write_Reg(12,gain|1<<8);//设置R12
}
//WM8978 L2/R2(也就是Line In)增益设置(L2/R2-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void WM8978_LINEIN_Gain(u8 l_gain,u8 r_gain)
{
	u16 regval;
	l_gain&=0X07;
	regval=WM8978_Read_Reg(47);	//读取R47
	regval&=~(7<<4);			//清除原来的设置
	regval |= l_gain<<4;	
 	WM8978_Write_Reg(47,regval);//设置R47
	r_gain&=0X07;
	regval=WM8978_Read_Reg(48);	//读取R48
	regval&=~(7<<4);	//清除原来的设置 
	regval |= r_gain<<4;
 	WM8978_Write_Reg(48,regval);//设置R48
} 
//WM8978 AUXR,AUXL(PWM音频部分)增益设置(AUXR/L-->ADC输入部分的增益)
//gain:0~7,0表示通道禁止,1~7,对应-12dB~6dB,3dB/Step
void WM8978_AUX_Gain(u8 aux_l,u8 aux_r)
{
	u16 regval;
	
	regval=WM8978_Read_Reg(47);	//读取R47
	regval&=~(7<<0);	//清除原来的设置
	regval |= aux_l<<0;
 	WM8978_Write_Reg(47,regval);//设置R47
	regval=WM8978_Read_Reg(48);	//读取R48
	regval&=~(7<<0);	//清除原来的设置
	regval |= aux_r<<0;
 	WM8978_Write_Reg(48,regval);//设置R48
}  
//设置I2S工作模式
//fmt:0,LSB(右对齐);1,MSB(左对齐);2,飞利浦标准I2S;3,PCM/DSP;
//len:0,16位;1,20位;2,24位;3,32位;  
void WM8978_I2S_Cfg(u8 fmt,u8 len)
{
	fmt&=0X03;
	len&=0X03;//限定范围
	WM8978_Write_Reg(4,(fmt<<3)|(len<<5));	//R4,WM8978工作模式设置	
}	

//设置耳机左右声道音量
//voll:左声道音量(0~63)
//volr:右声道音量(0~63)
void WM8978_HPvol_Set(u8 voll,u8 volr)
{
	voll&=0X3F;
	volr&=0X3F;//限定范围
	if(voll==0)voll|=1<<6;//音量为0时,直接mute
	if(volr==0)volr|=1<<6;//音量为0时,直接mute 
	WM8978_Write_Reg(52,voll|1<<8);			//R52,耳机左声道音量设置
	WM8978_Write_Reg(53,volr|1<<8);	//R53,耳机右声道音量设置,同步更新(HPVU=1)
}
//设置喇叭音量
//voll:左声道音量(0~63) 
void WM8978_SPKvol_Set(u8 volx)
{ 
	volx&=0X3F;//限定范围
	if(volx==0)volx|=1<<6;//音量为0时,直接mute 
 	WM8978_Write_Reg(54,volx|1<<8);			//R54,喇叭左声道音量设置
	WM8978_Write_Reg(55,volx|1<<8);	//R55,喇叭右声道音量设置,同步更新(SPKVU=1)	
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
//RX/TX收发控制
//0: RX
//1: TX_CW
//2: TX_SSB/AM/FM
void WM8978_TR_set(u8 trvolue,u8 gain,u8 alc)
{
	if(trvolue ==0)
	{
		WM8978_ALC_Cfg(0,15);
		WM8978_AUXIN_Cfg(7);
		WM8978_Input_Cfg(1,1);//输入通道(MIC&LINE IN)
		WM8978_Output_Cfg(0,0);		//开启BYPASS输出 
		WM8978_MIC_Gain(gain&0x3f);//MIC增益设置,必须放在ALC后面，否则增益恢复不起作用
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
		WM8978_Input_Cfg(0,0);//输入通道(MIC&LINE IN)
		WM8978_ALC_Cfg(3,alc);
		WM8978_Output_Cfg(1,1);		//开启BYPASS输出 
	}
}












