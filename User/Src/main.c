#include "KEY_Control.h"
#include "sys.h"
#include "rtc.h"
#include "dwt.h"
#include "led.h"  
#include "mpu.h"   
#include "lcd.h" 
#include "timer.h" 
#include "arm_math.h" 
#include "key.h" 
#include "adc.h"
#include "ui_Driver.h"
#include "dsp_filter.h"
#include "gpio_set.h"
#include "Audio_driver.h"
#include "encode.h"
#include "24cxx.h"
#include "Si5351AI2C.h"
#include "malloc.h"
#include "exti.h"
#include "usart.h"
#include "wm8978.h"
//ALIENTEK 阿波罗STM32H7开发板 实验52_2
// DSP FFT测试 实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 


#define FX_4CR  			"FX-4CR"
#define EXAMPLE_DATE	"2023/04/15"
#define DEMO_VER 		"V_1.2"
extern u32 m;
extern u32 n;
u8 Uart_tx[]={"FX_4C DMA 串口实验"};
extern u16  Rx_amp_user[10];
void tx_band_no()//设置发射禁止波段
{
	sd.txband_no = AT24CXX_ReadLenByte( ADDR_TX_BAND_NO,1 )&0x0F;
	if(sd.txband_no >10)sd.txband_no =0;
	if(PIN_K2==0)
	{
		sd.txband_no =~sd.txband_no&0x01;
		AT24CXX_WriteLenByte( ADDR_TX_BAND_NO,sd.txband_no, 1);
	}
	while(PIN_K2==0);
}
int main(void)
{ 
	//u8 t;
	//u8 len;
	//u16 times;
	static u8 time,spk_time,spk_vol;//hp_vol;
	static u8 Dormant =0;
	static u8 lock,kslock;
	static u8 dir;
	static u8 menu_key;
	//u16 i;
	//Stm32_Clock_Init(200,2,2,4);	//设置时钟,400Mhz
	Stm32_Clock_Init(192,5,2,4);	//设置时钟,480Mhz;25M
    bsp_InitDWT();					//延时初始化  
	MPU_Memory_Protection();		//保护相关存储区域 
	GPIO_Init();
	TR_CONTROL(CONTROL_RX);/* 发射开启 */
	//AF_SQL(1);
	bsp_InitKey();					//初始化按键 
	Lcd_Init();		//初始化LCD
	//KEY_BL1(1);
	Lcd_Color(0,0,319,239,BLACK);
	/******************************************************************************************/
	LCD_ShowString(120,60,GRAY1,BLACK ,32,0,FX_4CR);
	LCD_ShowString(106,120,YELLOW1,BLACK ,24,0,EXAMPLE_DATE);
	LCD_ShowString(130,170,GRAY2,BLACK ,32,0,DEMO_VER);
	
	delay_ms(2000);
	//Data_init();
	Lcd_Color(0,0,319,239,BLACK);
	ui_TopBackground(0,0,320,24);
	//RTC_Init();	//初始化RTC
	AT24CXX_Init();
	dir = AT24CXX_ReadLenByte( ADDR_ENC_DIR,1 );
	if(dir>1)dir=0;
	dir=0;
	//
	if( PIN_K13==0)
	{
		dir =~dir&0x01;
		AT24CXX_WriteOneByte(ADDR_ENC_DIR, dir);
		while(PIN_K13==0);
	}
	tx_band_no();
	if( PIN_K9==0)//开发者菜单显示
	{
		sd.menu_end = MENU_HIDD_END;
		sd.menu_page =1;
		 menu_key =1;
		ks.CW_keySW = 1;
		ks.rx_adjuset_key =0;
		sd.amp_phase =0;
		LCD_ShowString(50,130,YELLOW1,BLACK ,24,0,"Development mode");
		while(PIN_K9==0);
	}
	else 
	{
		sd.menu_end = MENU_END;
		sd.menu_page =0;
		menu_key =0;
	}
	if( PIN_K7==0)//开发者菜单显示
	{
		//sd.menu_end = 20;
		ks.rx_adjuset_key =1;
		LCD_ShowString(50,130,YELLOW1,BLACK ,24,0,"RX mirror frequency");
		LCD_ShowString(50,155,YELLOW1,BLACK ,24,0,"suppression");
		while(PIN_K7==0);
	}
	
	//ui_TopBackground(0,0,320,24);
	//GPIO_Init();
	SI5351A_Init();
	Encode_Init(0);
	
	Audio_hardware_init();
	Data_init();
	uart1_init(120,115200);		//串口初始化为115200
	uart3_init(120,115200);		//串口初始化为115200
	SysTick_init(SYSTEM_CLK,1);
	bsp_StartAutoTimer(0, 25); /* 启动1个100ms的自动重装的定时器 */
	sd.key_BL_delay = 200;//按键背光延迟10秒关闭；
	delay_ms(1000);
	POWER_SET_ON;//接通电源
	//EXTIX_Init();//PTT中断开启后。发射时会有干扰，所以禁用
	//VOX_OUT(0);
	//
    while(1)
	{
		PowerControl_off();		//POWER_OFF电源关机
		//ui_SideFunction_R_Display(TR_READ);
		//bsp_KeyScan();	/* 每隔10ms调用一次此函数，此函数在 key.c */
		//audio_DAC_wirte();
		/* 判断定时器超时时间 */
		if (bsp_CheckTimer(0))	
		{
			if(sd.key_BL_delay >0)
			{
				sd.key_BL_delay--;
				if(sd.key_BL_delay <=0 && ks.PWR_LONG ==0)
				{
					KEY_BL(0);
				}
				else
				{
					KEY_BL(1);
				}
			}
			if( Dormant ==0)
			{
				/* 每隔100ms 进来一次 */ 
				key_voltage_Display();
				//ui_Tim_Dispaly(260,12,GRAY1,BLACK,12,1);			
				if(TR_READ ==CONTROL_RX || (TR_READ ==CONTROL_TX && sd.TX_spe==1 ))
				key_Spectrum_Control();
				
				if(ks.key_lock ==1)
				{
					kslock =0;
					if(++time>=5)/* 50*10=500ms */
					{
						lock = ~lock;
						time =0;
					}
					/* 按键锁定状态，LOCK字符在屏幕LCD顶部闪动 */
					if(lock ==0 )
					{
						ui_TopBackground(250,4,12,16);
						//LCD_ShowString(228,4,GRAY0,BLUE2 ,16,0,"LOCK");	
						key_lock_Display(250);
					}
					else
					{
						ui_TopBackground(250,4,12,16);
						LCD_ShowString(250,4,BLUE2,BLUE2 ,12,1," ");
					}
				}
				else 
				if(ks.key_lock ==0 && kslock ==0)	
				{
					LCD_ShowString(250,4,BLUE2,BLUE2 ,16,1," ");
					ui_TopBackground(250,4,12,16);
					kslock =1;
				}
				Dormant =1;
			}
		}
		else Dormant =0;
		KeyCode_lock_settings();
		Key_Management_Set();
		key_ATT_Control();
		//key_ampset();
		RXandTX_Control_Management();
		Enc0_UseManagement();
		Enc2_UseManagement();
		//key_ATT_Control();
		//RXandTX_Control_Management();
		Menus_Set();
		//key_Spectrum_Control();
		//ui_Signal_intensity_Display(10,87,GRAY0,BLACK ,32768,0);
		Key_AGC_RecoveryTime_set();
		ui_RIT_Display(sd.rit);
		
		//ui_Power_Display(sd.Pow );
		
		ui_Signal_intensity_Display(30,87,GREEN,BLACK ,ads.Riss ,TR_READ);
		ui_SideFunction_R_Display(TR_READ);
		key_DSP_NoiseProcessing();
		key_bluetooth_control();
		
		if(spk_time<63 )
		{
			if(++spk_time>63)spk_time =63;
			if(vfo[VFO_A].Mode == DEMOD_LSB_DIG || vfo[VFO_A].Mode == DEMOD_USB_DIG)ads.spk_vol =0;
			else ads.spk_vol =spk_time;
			if(spk_time<56 )ads.hp_vol =spk_time;
		}
		
		if(spk_vol != ads.spk_vol+ads.hp_vol)//&& ads.tx_delay<10 )
		{
			WM8978_SPKvol_Set(ads.spk_vol);
			WM8978_HPvol_Set(ads.hp_vol,ads.hp_vol);
			spk_vol = ads.spk_vol+ads.hp_vol;
			//hp_vol = ads.hp_vol;
		}
		ui_VOL_DisPlay(sd.Pow,"POW",2);
		ui_VOL_DisPlay(sd.Vol,"VOL",0);
		ui_VOL_DisPlay(sd.Sql ,"SQL",1);
		ui_VOL_DisPlay(ads.IF_gain,"MGC",5); 
		ui_VOL_DisPlay(sd.mic_gain ,"MIC",10);
		ui_VOL_DisPlay( nr.dsp_nr_strength ,"DSP",11);
		ui_VOL_DisPlay(nr.dsp_nr_delaybuf_len,"LEN",12);
		ui_VOL_DisPlay(sd.CW_vol ,"SIVOL",13);
		ui_VOL_DisPlay(sd.CW_Sidetone  ,"SITON",14);
		ui_VOL_DisPlay(sd.CW_Speed  ,"KEYSP",15);
		//ui_VOL_DisPlay(Rx_amp_user[ads.rx_amp_band],"IQ_BL",6);
		
		if(menu_key>0)
		{
			ks.menu_key= 1;
			if(ks.rx_adjuset_key ==1)Lcd_Color(0, 182,319,239,BLACK);// 刷新页面
			Lcd_Color(0, 130,319,239,BLACK);// 刷新页面
			ks.StepSelected_idx = T_STEP_1HZ_IDX;//波段步进默认设置为1kHz;
			ads.tx_delay =0;
			sd.spe_fil_time=SPE_FIL_TIME;//频谱带宽显示回复计时标志BAL
			if( ks.CW_keySW<1)
			{
				ks.CW_keySW = 1;
			}
			
			menu_key =0;
		}
		//WM8978_HPread_Cfg();
		//UART3_Task();
		//UART_Task();
		//UART_DMAEnd();
		//LCD_ShowNum(150,150,GRAY1,BLACK, 10,16,PTT_RT);
		//LCD_ShowNum(100,150,GRAY1,BLACK, 4,24,spk_vol);
//		if(USART_RX_STA&0x8000)
//		{					   
//			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
//			printf("\r\n您发送的消息为:\r\n");
//			for(t=0;t<len;t++)
//			{
//				USART2->TDR=USART_RX_BUF[t];
//				while((USART2->ISR&0X40)==0);//等待发送结束
//			}
//			printf("\r\n\r\n");//插入换行
//			USART_RX_STA=0; 
//		}else
//		{
//			times++;
//			if(times%200==0)
//			{
//				printf("\r\nFX_4C 串口实验\r\n");
//				printf("BG2IXD\r\n\r\n\r\n");
//			}
//			if(times%100==0)printf("输入数据,以回车键结束\r\n");  
//		}
//		times++;
//		if(times%100==0)
//		{
//			//printf("\r\nFX_4C 串口实验\r\n");
//			UartTx(Uart_tx, 64);
//		}
	}
	//return 0;
}

		 
 



