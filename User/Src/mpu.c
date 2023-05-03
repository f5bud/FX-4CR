#include "mpu.h" 
#include "usart.h" 
#include "led.h" 
#include "dwt.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//MPU�ڴ汣�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/6/7
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//********************************************************************************
//����˵�� 
//��
//////////////////////////////////////////////////////////////////////////////////	 

//��ֹMPU����
void MPU_Disable(void)
{  
	SCB->SHCSR&=~(1<<16);	//��ֹMemManage 
	MPU->CTRL&=~(1<<0);		//��ֹMPU
}

//����MPU����
void MPU_Enable(void)
{ 
	MPU->CTRL=(1<<2)|(1<<0);//ʹ��PRIVDEFENA,ʹ��MPU 
	SCB->SHCSR|=1<<16;		//ʹ��MemManage
}

//��nbytesת��Ϊ2Ϊ�׵�ָ��.
//NumberOfBytes:�ֽ���.
//����ֵ:��2Ϊ�׵�ָ��ֵ
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

//����ĳ�������MPU����
//baseaddr:MPU��������Ļ�ַ(�׵�ַ)
//size:MPU��������Ĵ�С(������32�ı���,��λΪ�ֽ�)
//rnum:MPU���������,��Χ:0~7,���֧��8����������
//ap:����Ȩ��,���ʹ�ϵ����:
//0,�޷��ʣ���Ȩ&�û������ɷ��ʣ�
//1,��֧����Ȩ��д����
//2,��ֹ�û�д���ʣ���Ȩ�ɶ�д���ʣ�
//3,ȫ���ʣ���Ȩ&�û����ɷ��ʣ�
//4,�޷�Ԥ��(��ֹ����Ϊ4!!!)
//5,��֧����Ȩ������
//6,ֻ������Ȩ&�û���������д��
//���:STM32F7����ֲ�.pdf,4.6��,Table 89.
//sen:�Ƿ�������;0,������;1,����
//cen:�Ƿ�����cache;0,������;1,����
//ben:�Ƿ�������;0,������;1,����
//����ֵ;0,�ɹ�.
//    ����,����.
u8 MPU_Set_Protection(u32 baseaddr,u32 size,u32 rnum,u8 ap,u8 sen,u8 cen,u8 ben)
{
	u32 tempreg=0;
	u8 rnr=0;
	if((size%32)||size==0)return 1;	//��С����32�ı���,����sizeΪ0,˵����������
	rnr=MPU_Convert_Bytes_To_POT(size)-1;//ת��Ϊ2Ϊ�׵�ָ��ֵ
	MPU_Disable();					//����֮ǰ,�Ƚ�ֹMPU����
	MPU->RNR=rnum;					//���ñ�������
	MPU->RBAR=baseaddr;				//���û�ַ
	tempreg|=0<<28;					//����ָ�����(�����ȡָ��)
	tempreg|=((u32)ap)<<24;			//���÷���Ȩ��,
	tempreg|=0<<19;					//����������չ��Ϊlevel0
	tempreg|=((u32)sen)<<18;		//�Ƿ�������
	tempreg|=((u32)cen)<<17;		//�Ƿ�����cache
	tempreg|=((u32)ben)<<16;		//�Ƿ�������
	tempreg|=0<<8;					//��ֹ������
	tempreg|=rnr<<1;				//���ñ��������С
	tempreg|=1<<0;					//ʹ�ܸñ������� 
    MPU->RASR=tempreg;				//����RASR�Ĵ���
	MPU_Enable();					//�������,ʹ��MPU����
	return 0;
}

//������Ҫ�����Ĵ洢��
//����Բ��ִ洢�������MPU����,������ܵ��³��������쳣
//����MCU������ʾ,����ͷ�ɼ����ݳ���ȵ�����...
void MPU_Memory_Protection(void)
{
	MPU_Set_Protection(0x20000000,128*1024,1,MPU_REGION_FULL_ACCESS,0,1,1);		//��������DTCM,��128K�ֽ�,��ֹ����,����cache,������
	
	MPU_Set_Protection(0x24000000,512*1024,2,MPU_REGION_FULL_ACCESS,1,1,1);		//��������AXI SRAM,��512K�ֽ�,��ֹ����,����cache,������
	MPU_Set_Protection(0x30000000,512*1024,3,MPU_REGION_FULL_ACCESS,0,1,0);		//��������SRAM1~SRAM3,��288K�ֽ�,��ֹ����,����cache,������
	MPU_Set_Protection(0x38000000,64*1024,4,MPU_REGION_FULL_ACCESS,0,0,0);		//��������SRAM4,��64K�ֽ�,��ֹ����,����cache,������

	MPU_Set_Protection(0x60000000,64*1024*1024,5,MPU_REGION_FULL_ACCESS,0,0,0);	//����MCU LCD�����ڵ�FMC����,,��64M�ֽ�,��ֹ����,��ֹcache,��ֹ����
	MPU_Set_Protection(0XC0000000,32*1024*1024,6,MPU_REGION_FULL_ACCESS,0,0,0);	//����SDRAM����,��32M�ֽ�,��ֹ����,����cache,������
	MPU_Set_Protection(0X80000000,256*1024*1024,7,MPU_REGION_FULL_ACCESS,1,1,1);//��������NAND FLASH����,��256M�ֽ�,��ֹ����,��ֹcache,��ֹ����
}

//MemManage�������ж�
//������ж��Ժ�,���޷��ָ���������!!
void MemManage_Handler(void)
{ 
	//LED1(0);							//����DS1
	//printf("Mem Access Error!!\r\n"); 	//���������Ϣ
	delay_ms(1000);	
	//printf("Soft Reseting...\r\n");		//��ʾ�������
	delay_ms(1000);	
	Sys_Soft_Reset();					//��λ
}














