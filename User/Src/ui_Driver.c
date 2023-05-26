/*
**************************************************
* FX-4C ui底层驱动 
* 版本 V1.1；
* 作者：CG2IXD； 
* 时间：2020/9/9；
**************************************************
*/
/*
**************************************************
* LCD像素是320*240
* 
**************************************************
*/
#include "ui_Driver.h"
#include "lcd.h"
#include "rtc.h"
#include "arm_math.h" 
//#include <stdio.h>
#include "stdlib.h"
#include "Audio_driver.h"
#include "KEY_Control.h"
#include "gpio_set.h"
#include "adc.h"
#include "timer.h"
#include "dsp_filter.h"
#include "24cxx.h"
#include<math.h>
//
//arm_rfft_fast_instance_f32 Spe_FFT;
//extern __IO At24c_addr_GenInfo	vfo[2];
extern u32  Menu_Values[MENU_END];
extern  u16 Rx_amp_adjust[12];
/*
********************************************************************************
* 顶部显示窗口
* 大小是320*24，划分若干区域，用以显示时间 电源电压等信息
********************************************************************************
*/
	/*
	* 顶部显示区域背景颜色设置函数
	* 显示区域由上至下蓝色逐渐淡化；
	* xStart:0,xEnd:319; yXtart:0,yEnd:23。
	*/
void ui_TopBackground(u16 x,u16 y,u16 width,u16 h)
{
	//static u8 start =0;
	u16 x0,y0;
	//if(start ==0)
	//{
		for(y0=0;y0<h;y0++)	/*高度是24像素*/
		{
			for(x0=0;x0<width;x0++)
			{
				Gui_DrawPoint(x0+x,y0+y,24-y0-y);
			}
		}
	//	start =1;
	//}
}
//
/*	按键锁图形绘制	*/
void key_lock_Display(u16 x)
{
	u8 x0,y0;
	LCD_ShowString(x+1,4,RED,BLACK ,16,0,"0");
	for(y0=13;y0<20;y0++)	/*高度是24像素*/
	{
		for(x0=0;x0<11;x0++)
		{
			Gui_DrawPoint(x0+x,y0,RED);
		}
	}
}
/* ATT显示的坐标和文本宏定义 */
#define UI_ATT_START_X 	50
#define UI_ATT_START_Y 	4
#define UI_ATT_TEXT		"ATT"
	/*
	* ATT衰减控制显示函数
	* k：ATT开关；
	* k=0：ATT关闭OFF;
	* k=1：ATT启用ON；
	*/
void ui_ATT_Display(u8 k)
{
	u16 fc,bc;
	if(k ==2)
	{
		fc =GRAY0;
		bc =RED;
		ui_TopBackground(50,4,32,16);
		LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT2");	
	}
	else
	if(k ==1)
	{
		fc =GRAY0;
		bc =RED;
		ui_TopBackground(50,4,32,16);
		LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT1");
	}
	else
	if(k ==0)	
	{
		fc =GRAY2;
		bc =BLUE2 ;
		ui_TopBackground(50,4,32,16);
		LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT ");
	}
	//LCD_ShowString(UI_ATT_START_X, UI_ATT_START_Y, fc, bc , 16, 1,"ATT");
}

	/*
	* 有小数点的数字显示窗，小数点前显示两位小数点后一位；最大显示99.9；（输入数值是实际数值*10）
	* x:	水平起点；
	* y：	垂直起点；
	* fc：	字体颜色；
	* bc：	字体底色；
	* font：字体大小；
	* num:  输入数值（实际数值*10，num=138显示13.8）
	* text：数值后面显示的文本，比如text='V',13.8V;text='W',5.0W;
	* on：	窗口显示的开关；on=0，窗口关闭；on=1,窗口开启；
	*/
void ui_Decimal_Dispaly(u16 x,u16 y,u16 fc,u16 bc,u8 font,u16 num)
{
	u8 a,b;
	
	a =num/10;/* 计算整数 */
	b =num%10;/* 计算小数 */ 
	LCD_ShowNum(x, y, fc, bc, 2, font, a);				/* 显示整数   */
	LCD_ShowChar(x+(font/2)*2, y, fc, bc,font,0,'.');	/* 显示小数点 */
	LCD_ShowNum(x+(font/2)*3, y, fc, bc, 1, font, b);	/* 显示小数   */
	//LCD_ShowChar(x+(font/2)*4, y, fc, bc,font,0,text);	/* 显示文字   */	
}

	/*
	* 时间显示窗口;函数中调用rtc.c中的RTC_Get_Time(&hour,&min,&sec,&ampm)函数，得到时间并显示在设定的位置
	* x:	水平起点；
	* y：	垂直起点；
	* fc：	字体颜色；
	* bc：	字体底色；
	* font：字体大小；
	* on：	窗口显示的开关；on=0，窗口关闭；on=1,窗口开启；
	*/
void ui_Tim_Dispaly(u16 x,u16 y,u16 fc,u16 bc,char font,u8 on)
{
	u8 hour,min,sec,ampm;
	static u8 start =0;
	static u8 hour_0;
	static u8 min_0;
	static u8 sec_0;
	/* 获取RTC时间 */
	RTC_Get_Time(&hour,&min,&sec,&ampm);/* 此函数在rtc.c中定义 */
	if(start ==0 || hour_0 != hour)
	{
		ui_TopBackground(x,y,12,12);
		LCD_ShowxNum(x, y, fc, bc, 2, font, 0x81, hour);	/* 显示小时 */
		start =1;
		hour_0 =hour;
	}
	if(start ==0 || min_0!= min )
	{
		ui_TopBackground(x+font/2*2,y,18,12);
		if(on==1)
		{
			LCD_ShowChar(x+font/2*2, y, fc, bc,font,1,':');
		}
		else
			LCD_ShowChar(x+font/2*2, y, fc, bc,font,1,' ');
		LCD_ShowxNum(x+font/2*3, y, fc, bc, 2, font, 0x80, min);/* 显示分钟 */
		start =1;
		min_0 = min;
	}
	if(start ==0 || sec_0 != sec )
	{
		ui_TopBackground(x+font/2*5,y,18,12);
		LCD_ShowChar(x+font/2*5, y, fc, bc, font,1,':');
		LCD_ShowxNum(x+font/2*6, y, fc, bc, 2, font, 0x81, sec);/* 显示秒 */	
		start =1;
		sec_0 =sec;
	}
}
/*
*	静音显示图标
*	
*/
void ui_Mute_icon(u16 x,u16 y,u16 bc,u8 spk)
{
	u16 x0,y0;
	if(ks.Spk_key==0)
	{
		ui_TopBackground(x-2,0,15,20);
		for(y0=0;y0<8;y0++)
		{
			for(x0=0;x0<3;x0+=1)Gui_DrawPoint(x+x0+1,y+y0+4,YELLOW);
		}
		for(y0=0;y0<16;y0+=1)
		{
			Gui_DrawPoint(x+7,y+y0,YELLOW);
		}
		Gui_DrawLine(x+4, y+4,x+7, y,YELLOW);
		Gui_DrawLine(x+4, y+11,x+7, y+15,YELLOW);
	}
	else
	{
		ui_TopBackground(x-2,0,15,20);
		 Gui_Circle(x+4,y+6,5,YELLOW) ;
		ui_TopBackground(x+2,y+9,5,3);
		
		for(y0=0;y0<5;y0++)
		{
			for(x0=0;x0<2;x0+=1)Gui_DrawPoint(x+x0+1,y+y0+8,YELLOW);
		}
		for(y0=0;y0<5;y0++)
		{
			for(x0=0;x0<2;x0+=1)Gui_DrawPoint(x+x0+6,y+y0+8,YELLOW);
		}
	}
	
	Gui_DrawLine(x+12, y+3,x+18, y+12,bc);
	Gui_DrawLine(x+12, y+12,x+18, y+3,bc);
}
/*
********************************************************************************
* 模式 频率 滤波器等显示区域
* X起点：0；X终点：319
* Y起点：24;Y终点：80；
********************************************************************************
*/
	/*
	* 此区域左侧x(0,59);y(24,80)为多功能显示窗口，包括机器工作模式，当前频率的模式，VFO/CH的选择等
	* 字体采用ASCII 12;
	*/
/* 用于工作模式和VFO显示窗口的坐标和大小的宏定义 */
#define UI_MODE_VFO_X		1	/* mode X起点*/
#define UI_MODE_VFO_Y		26	/* mode Y起点*/
#define UI_MODE_VFO_W		46	/* mode W宽度*/
#define UI_MODE_VFO_H		28	/* mode H高度*/
//typedef struct
//{
//	char *Mode[6];/* 模式显示文本数组 */
//	char *V_CH[2];/* VFO/CH转换显示文本数组 */
//	char *Vfo[2]; /* VFO A/B转换显示文本数组 */
//	char *RT[2];  /* TX/RX文本数组 */
//}TWXT_tybedef;
const  TWXT_tybedef text =
{
	/* 模式显示文本数组 */
	{
		" CW L "," CW U "," LSB  "," LSB  "," USB  "," USB  "," AM   "," FM   "
	},
	/* VFO/CH转换显示文本数组 */
	{
		" VFO  "," CH   ",
	},
	/* VFO转换显示文本数组 */
	{
		"A ","B ",
	},
	/* TX/RX文本数组 */
	{
		"T=R","T=R","RX","RX"
	}
};
   /* 
	* 多功能窗口显示函数
	* rt_key： 收发转换
	* modeidx：模式mode选择；
	* vfo_reg;  
	* 		    位0：VFO A/B选择，0：VFO_A,1: VFO_B;
	*		    位1：信道CH/VFO选择，0：VFO，1：CH;
	*		    位2：电台的工作模式，0：收发模式，1：只接收模式；
	*			位3-位5：保留；
	*			位6：同频/异频收发选择，0：收发同频，1：收发异频；
	*			位7：收发转换，0：接收RX，1：发射TX；
	* ch_num： 信道数值；
	*/
void ui_Vfo_Display(u8 ch_num)
{
	static u8 vfo_0;
	u16 fc;
	static u8 clk =0;
	static u8 rt_rx0;
	static u8 ab_0;
	static u8 tr_0;
	static u8 sim_dif0;
	static u8 ch_vfo0;
	static u8 ch_num0;
	//static u8 mode_0;
	if( ab_0 != ks.A_B || ch_vfo0 != ks.ch_vfo || ch_num0 != ch_num || tr_0 != TR_READ || sim_dif0 != ks.sim_dif || rt_rx0 != ks.rt_rx || clk ==0)
	{
		if(ks.rt_rx ==1 )/* 接收机模式 */
		{
			ui_TopBackground(12,4,16,16);
			fc =GRAY0;
		}
		else/* 收发信机模式 */
		{
			ui_TopBackground(12,4,16,16);
			fc = RED;
		}
		
		if(TR_READ ==CONTROL_TX && ks.sim_dif ==1)/* 异频发射TX */
		{
			vfo_0 = ~ks.A_B &0x01;/* 只在异频发射状态VFO_A/VFO_B或者CH_A/CH_B调换 */
			
		}
		else/* 同频发射及接收状态 */
		{
			vfo_0 = ks.A_B &0x01;/* VFO_A/VFO_B或者CH_A/CH_B不调换 */
		}		
		
		/* 电台工作模式显示 */
		//LCD_RectangleWindow(2, 2, 36, 20, GRAY1);/* 窗口边框 */
		ui_TopBackground(12,4,24,16);
		LCD_ShowString(12, 4, fc, BLUE  , 16, 1,text.RT [ks.rt_rx<<1 | ks.sim_dif]);/* 0:显示“RX”, 1:显示“TX” */
		if(ks.rt_rx==0 && ks.sim_dif ==1 )LCD_ShowString(20, 4, GRAY0 , BLUE  , 16, 1,"/");
		
		/* MODE显示 */
//		if(mode_0 != vfo[VFO_A].Mode)
//		{
//			LCD_RectangleWindow(1, 30, 52, 20, GRAY1);/* 窗口边框 */
//			
//			LCD_ShowString(3, 32, GRAY0, RED , 16, text.Mode [ vfo[VFO_A].Mode ] );
//			mode_0 = vfo[VFO_A].Mode;
//		}
		/* VFO显示窗口的矩形边线 */
		LCD_RectangleWindow(1, 47, 52, 19, GRAY1);
		/* VFO/CH显示 */
		LCD_ShowString(3, 48,YELLOW, GRAY4  , 16, 0,text.V_CH [ks.ch_vfo]);
		
		//LCD_ShowString(35, 56, GRAY0, GRAY4 , 0,16, text.Vfo [ks.A_B]);
		if(ks.ch_vfo==0)
		{
			LCD_ShowString(35, 48, GRAY0 , GRAY4  , 16,1, text.A_B[vfo_0 ]);
		}
		else
		{
			LCD_ShowNum(35, 48, GRAY0, GRAY4 , 2, 16, ch_num-2);
		}
		//mode_0 = vfo[VFO_A].Mode;
		ab_0 = ks.A_B; 
		ch_vfo0 = ks.ch_vfo;
		ch_num0 = ch_num;
		tr_0 = TR_READ;
		sim_dif0 = ks.sim_dif;
		rt_rx0 = ks.rt_rx;
		clk =1;
		
	}
	/*
	*/
	/************************************************************************************************/
//		/* 副频率MODE/VFO显示窗口的矩形边线 */
//		LCD_RectangleWindow(UI_MODE_VFO_X, UI_MODE_VFO_Y+37, UI_MODE_VFO_W, 16, GRAY2);
//		/* 副频率VFO显示 */
//		LCD_ShowString(UI_MODE_VFO_X+2, UI_MODE_VFO_Y+39,YELLOW, GRAY4 , 12, V_CH_text[(vfo_reg>>1) &0x01]);
//		if(((vfo_reg>>1) &0x01)==0)
//		{
//			LCD_ShowString(UI_MODE_VFO_X+26, UI_MODE_VFO_Y+39, GRAY1, GRAY4 , 12, Vfo_text[~vfo&0x01]);
//		}
//		else
//		{
//			LCD_ShowNum(UI_MODE_VFO_X+26, UI_MODE_VFO_Y+39, GRAY1, GRAY4 , 2, 12, ch_num);
//		}
	/* 副频率MODE显示 */
	//LCD_ShowString(UI_MODE_VFO_X+2, UI_MODE_VFO_Y+14+29, GRAY0, bc , 12, Mode_text[modeidx]);
}
/*
********************************************************************************
* 滤波器显示区域，在频率显示右侧x(219,319)*y(24,60)区域；
* 有随带宽可变的图形以及带宽文本显示；
********************************************************************************
*/
// typedef struct
//{
//	char *text;
//	u8 width;
//	u8 spe_width;
//}Filter_parameters;
const  Filter_parameters Fil_p[] =
{
	{0,0,0},
	{" 50Hz  ",	0,	2},/* 0 */
	{" 100Hz ",	2,	3},/* 1 */
	{" 200Hz ",	4,	4},/* 2 */
	{" 300Hz ",	6,	5},/* 3 */
	{" 500Hz ",	8,	6},/* 4 */
	{" 800Hz ",	10,	7},/* 5 */
	
	{" 1.5k  ",	12, 12},/* 6 */
	{" 1.8k  ",	14, 14},/* 7 */
	{" 2.1k  ",	16, 17},/* 8 */
	{" 2.4k  ",	18, 19},/* 9 */
	{" 2.7k  ",	20, 22},/* 10 */
	{" 3.0k  ",	22, 24},/* 11 */
	
	{" 6.0k  ",	24, 48},/* 12 */
	{" 9.0k  ",	26, 72},/* 13 */
	{" 5.0k  ",	28, 40},/* 14 */
	{"  10k  ",	30, 80},/* 15 */
	//{" 5.0k  ",	26},/* 14 */
	//{"  10k  ",	28},/* 15 */
};
	/*
	*	滤波器带宽显示窗口
	*	***入口函数***
	*	x y	 	坐标起点
	*	fc 		字体颜色	Color
	*	bc		背景颜色
	*	k 		显示宽度 width
	*	mode	模式显示； -1：下边带LSB,0: AM/FM,1：上边带；
	*	****出口函数 无****
	*/
void ui_Filter_Display(u16 x,u16 y,u16 fc,u16 bc,u8 idx,int16_t mode)
{
	
	int16_t  x0,m;
  	int16_t  y0;
	u8 k;

	k = Fil_p[idx].width ; 
	Lcd_Color(x-32,y,x+32,y+32,bc);//
	LCD_RectangleWindow(x-32, y, 64, 34, GRAY2);/* 窗口边框 */
	
	if(mode==-1)m=-k/2-5;//LSB
	else
	if(mode==0)m=0;	//AM&FM
	else 	
	if(mode==1)m=k/2+5;//USB
	for(y0=0;y0<12;y0+=1)
	{		
		for(x0=(-y0/4-k/2-1);x0<(k/2+y0/4+2);x0+=1)
		{	
			/*if(y0<1||x0<(-y0/4-k/2)||x0>(k/2+y0/4)) */Gui_DrawPoint(x+x0-m,y+y0+6,fc);
			//else			
			//	Gui_DrawPoint(x+x0,y+y0+4,bc);
		}
	}					
	for(x0=-32;x0<32;x0+=1)Gui_DrawPoint(x+x0,y+18,GRAY2);//水平坐标
	
	for(y0=0;y0<16;y0+=1)Gui_DrawPoint(x,y+y0+3, GRAY2);//垂直坐标
	
	LCD_ShowString(x-21,y+20,YELLOW,BLACK,12,0,Fil_p[idx].text);
	
 }
/*
 *******************************************************************************
 * 频谱带宽指示
 *******************************************************************************
*/
void ui_Spe_Filter_Display(u16 fil_idx,int8_t mode,u8 clk,u16 bc,u8 h_mode)
{
	u16 x0,x1;
	static u16 spe_k;
	static int8_t mode0;
	static u16 bfo;
	//static u8 clk0;
	u8 h;
	if(h_mode)h=182;
	else h=239;
	/* 频谱带宽显示 */
	if(spe_k != Fil_p[fil_idx].spe_width || mode0 != mode || clk>0 || bfo != sd.Dsp_Bfo )
	{
		x0=sd.Dsp_Bfo*0.008f + Fil_p[fil_idx].spe_width/2;
		x1=sd.Dsp_Bfo*0.008f - Fil_p[fil_idx].spe_width/2;
		Lcd_Color(159-53,130,159+53,h,BLACK);
		
		if(mode==1)Lcd_Color(159-x0,130,159-x1,h,bc);//LSB
		else
		if(mode==0)Lcd_Color(159-Fil_p[fil_idx].spe_width/2,130,159+Fil_p[fil_idx].spe_width/2,h,bc);//AM&FM
		else 	
		if(mode==-1)Lcd_Color(159+x1,130,159+x0,h,bc);//USB
		spe_k = Fil_p[fil_idx].spe_width;
		mode0 = mode;
		bfo = sd.Dsp_Bfo;
		//clk0 = clk;
		//if(--clk<1)clk=0;
		//f0=f;
	}
}
/*
 *******************************************************************************
 * 调谐步进标识指示
 *******************************************************************************
*/
void set_pos(u16 x,u16 y,u16 fc)
{
	 u8 x0,y0;
	for(y0=0;y0<7;y0+=1)
	{
		for(x0=y0;x0<7-y0;x0+=1)
		{
			Gui_DrawPoint(x+x0,y+y0,fc);
		}
	}
//	for(y0=0;y0<2;y0+=1)
//	{
//		for(x0=0;x0<16;x0+=1)
//		{
//			Gui_DrawPoint(x+x0,y+y0,fc);
//		}
//	}
}
/*
********************************************************************************
* 步进显示
********************************************************************************
*/
/*
 * StepsDisplay struct
*/
 typedef struct
{
    uint16_t x;
     char *text;
}StepsDisplayGenInfo;
// StepsDisplay
const StepsDisplayGenInfo Steps_Info[] =
{
	{160,  "   1Hz"},
	{144, "  10Hz"},
	{128, " 100Hz"},
	{ 96, "  1KHz"},
	{ 80, " 10KHz"},
	{ 64, "100KHz"},
	{ 32, "   MHz"},
	{ 96, "   5KHz"},
	{ 0,  "  9KHz"}
	//{ 0,  "   MHz"}
};
	/*
	* 步进显示函数
	*/
void ui_Step_Display(u16 x,u16 y,int8_t idx)
{
	static u8 idx_0;
	//static u8 x0;
//	LCD_RectangleWindow(252, 62, 64, 16, GRAY2);/* 窗口边框 */
//	LCD_ShowString(254,64,YELLOW,GRAY5 ,12,0,"STEP");
//	LCD_ShowString(278,64,GRAY0 ,BLACK,12,0,Steps_Info[idx].text );
	
	if(idx >=0 && idx <7 )
	{
		set_pos( x+Steps_Info[idx_0].x, y, BLACK );
		set_pos( x+Steps_Info[idx].x, y, YELLOW );
		//set_pos( x+Steps_Info[idx_0].x, y, BLACK );
		idx_0 = idx;
	}
	
//	if(idx_0 != idx)
//	{
//		set_pos( x+Steps_Info[idx_0].x, y, BLACK );
//		idx_0 = idx;
//	}
}
/*
********************************************************************************
* RIT 频率微调显示
********************************************************************************
*/
void ui_RIT_Display(int16_t data )
{
	u16 fc,zi_fc;
	static u8 clk =0;
	static u8 rtt_key0;
	static int8_t data_0;
	static u8 rit_0;
	if(sd.rit_delay)sd.rit_delay--;
	if(sd.rit_delay<1){sd.rit_delay=0;ks.RIT_key =0;}
	if(data_0 != data || rtt_key0 != ks.Enc0_Use  || clk==0||rit_0!=ks.RIT_key)
	{
		if(ks.RIT_key ==1)
		{
			fc = RED;
			zi_fc = GRAY0;
		}
		else 
		{
			fc = GRAY2 ;
			if(data==0)zi_fc = GRAY2;
			else zi_fc = GRAY0;
		}
		//if(ks.RIT_key ==0)data =0;
		LCD_RectangleWindow(252, 61, 64, 14, fc);/* 窗口边框 */
		LCD_ShowString(254,62,zi_fc,GRAY5 ,12, 0," RIT ");
		if(data<0)
		{
			LCD_ShowString(284,62,zi_fc,BLACK ,12,0,"-");/* 小于0显示负号 */
			data = 0-data;/* 计算绝对值；（0减去负数的结果就是这个负数的绝对值）*/
		}
		else 
			LCD_ShowString(284,62,zi_fc,BLACK ,12,0,"+"); /* 大于0显示正号 */
		LCD_ShowNum(290, 62, zi_fc, BLACK , 4, 12, data);/* 显示数值 */
		
		data_0 = data;
		rtt_key0 = ks.Enc0_Use;
		clk =1;
		rit_0 =ks.RIT_key;
	}
}
/*
********************************************************************************
* 场强显示及功率显示 驻波显示区域 
********************************************************************************
*/
/*
*
*/

typedef struct
{
	u16 tx_bc;
	u16 rx_bc;
	char *text;/* 显示的文本 */
	//char *tx_text;/* 显示的文本 */
}RxTx_Window_parameters;
const RxTx_Window_parameters RxTx_wp[] =
{
	{RED,BLACK," T ",},
	{BLACK,GREEN2, " R "}
};
void ui_Signal_scale(u16 x,u8 y,int16_t m,u16 fc)
{
	u8 x0,y0,i;
	if(m<0)m *=-1;
	if(m>160)m=160;
	
	for(x0=0;x0<4;x0+=1)
	{
		for(y0=0;y0<7;y0+=1)
		{
			for(i=0;i<165;i++)
			{
				if(m>i)Gui_DrawPoint(44+x+x0+i,16+y0+y,fc);
				else Gui_DrawPoint(48+x+x0+m,16+y0+y,BLACK );
			}
		}
		
	}
}
	/*
	* 	信号显示窗口
	* 
	*/
void ui_Signal_intensity_Display(u16 x,u16 y,u16 fc,u16 bc,u16 riss,u8 RT_key)
{
	static u8 sql_timer;
	static u8 start =0;
	static u8 start_ptt =0;
	static u8 RT_key_0;
	static float32_t pow_0;
	static float32_t riss_0;
	//static float32_t meon_pow;
	static u8 p;
	//static float32_t riss_1[32];
	u16 x0,y0,i;
	//u16 Signal_table;
	//u16 fwd[10],ref[10];
	
	if(start ==0)/* 开机调用只执行一次 */
	{
		/*场强强度指示 */	
		
		//LCD_ShowString(x+32,y,YELLOW,BLACK,16,0,"S");
		LCD_ShowString(x+55,y,YELLOW,BLACK,12,0,"1  3  5  7  9");
			
		LCD_ShowString(149+x,y,RED,BLACK,12,0,"20  40 60dB");	
		/*S 场强表刻度线 */
		for(i=0;i<81;i+=9)	//RX刻度线S0～S9
		{
			for(y0=0;y0<2;y0+=1)
			{
				for(x0=0;x0<2;x0+=1)
				{
					Gui_DrawPoint(48+x+x0+i,12+y0+y,YELLOW);
				}
			}
		}
		for(i=81;i<155;i+=12)	//RX刻度线S9～S9+60db
		{
			for(y0=0;y0<2;y0+=1)
			{
				for(x0=0;x0<2;x0+=1)
				{
					Gui_DrawPoint(48+x0+i+x,12+y0+y,YELLOW);
				}
			}
		}
		/*S 场强表动态显示上下边框线 */	
		for(y0=0;y0<11;y0+=10)	
		{
			for(x0=0;x0<165;x0+=1)
			{
				Gui_DrawPoint(x0+48+x,y0+14+y,GRAY0);	
			}
		}
		/*发射功率刻度线 */
		for(i=0;i<155;i+=15)	//TX刻度线
		{
			for(y0=0;y0<2;y0+=1)
			{
				for(x0=0;x0<2;x0+=1)
				{
					Gui_DrawPoint(48+x0+i+x,25+y0+y,YELLOW);
				}
			}
		}
		//LCD_ShowString(x+32,23+y,YELLOW,BLACK,16,0,"Po");
		LCD_ShowString(62+x,27+y,YELLOW,BLACK,12,0,"2");
		LCD_ShowString(116+x,27+y,YELLOW,BLACK,12,0,"10");//发射功率刻度指示
		LCD_ShowString(191+x,27+y,YELLOW,BLACK,12,0,"20w");//发射功率刻度指示
		
		//ads.cw_alc_gain[vfo[VFO_A].Band_id] = AT24CXX_ReadLenByte(ADDR_CW_POW_GAIN(vfo[VFO_A].Band_id),2);//CW功率增益系数读取
		
		start =1;/* 执行完一次锁定，避免重复执行； */
	}
	
	/* 收发状态显示 */
	if(start_ptt <1 || RT_key_0 != RT_key)
	{
		
		/* 收发状态显示 */
		LCD_RectangleWindow(x+21, y, 26, 18, RxTx_wp[RT_key].rx_bc);/* 窗口边框 */
		LCD_RectangleWindow(x+21, y+22, 26, 18, RxTx_wp[RT_key].tx_bc);/* 窗口边框 */
		//LCD_ShowString(x-29, y,  GRAY0,RxTx_wp[RT_key].bc , 16,0, RxTx_wp[RT_key].text );
		//LCD_ShowString(x, y, GRAY0,RxTx_wp[RT_key].tx_bc , 16,0, RxTx_wp[RT_key].text );
		LCD_ShowString(x+22,y+1,YELLOW,RxTx_wp[RT_key].rx_bc,16,0," S ");
		LCD_ShowString(x+22,23+y,YELLOW,RxTx_wp[RT_key].tx_bc,16,0," Po");
		/* TX驻波SWR显示窗口清屏 */
		//Lcd_Color(252,98,319,180,BLACK);
		if(RT_key ==CONTROL_TX)
		{
//			if( RT_key_0 != RT_key)
//			{ 
//				start_ptt =0;
//				RT_key_0 = RT_key;
//			}
			//LCD_RectangleWindow(x-1, y-1, 26, 18, BLACK);/* 窗口边框 */
			//LCD_RectangleWindow(x-1, y-1, 26, 18, RED);/* 窗口边框 */
			Lcd_Color(160,132,160,180,BLACK);
			Lcd_Color(5,130,70,180,BLACK);
			LCD_RectangleWindow(5, 131, 64, 20, RED);/* 窗口边框 */
			LCD_ShowString(55,133,YELLOW,GRAY5 ,16, 1, "W");
			LCD_RectangleWindow(5, 153, 64, 20, RED);/* 窗口边框 */
			LCD_ShowString(9,155,YELLOW ,BLACK , 16, 0, "SWR "); /* 大于0显示正号 */
				
			//start_ptt++;
		}
		else 
		{
			Lcd_Color(160,132,160,180,BLACK);
			Lcd_Color(5,130,70,180,BLACK);
		}
		if( RT_key_0 != RT_key)
		{ 
			start_ptt =0;
			RT_key_0 = RT_key;
		}
		//RT_key_0 = RT_key;
		//start_ptt =1;
		if( RT_key_0 == RT_key)
		{ 
			start_ptt =1;
		}
//		if( start_ptt>2)start_ptt =2;
	}
	
	/* TX驻波SWR显示 */
	if(RT_key ==CONTROL_TX)
	{
//		for(i=0;i<5;i+=1)	//TX刻度线
//		{
//			ads.fwd+=Get_Adc1(ADC_CH11);/* 正向检测 */
//			ads.ref+= Get_Adc1(ADC_CH10);/* 反射检测 */
//		}
		//ads.fwd = Get_Adc1(ADC_CH11);/* 正向检测 */
		//ads.ref = Get_Adc1(ADC_CH10);/* 反射检测 */
		//arm_mean_f32((float32_t *)fwd ,10, (float32_t *)&ads.fwd );
		//arm_mean_f32((float32_t *)ref ,10, (float32_t *)&ads.ref );
		//ads.fwd/=5;
		//ads.ref/=5;
		/*
		*	P=V*V/R
		*	功率计算公式 pow = ((fwd*2.5f/65535) * (fwd*2.5f/65535))/50*100;
		*	公式末尾之所以乘以100是因为功率检测部分的衰减是20db;
		*	我们把它简化，把已知部分先行计算；并把结果放大10倍；
		*/
		//ads.pow = ads.fwd * 0.00038f * ads.fwd *0.00038f * 0.02f;/* 16位ADC */
		//ads.pow = ads.fwd * 0.012207f * ads.fwd *0.012207f * 0.0975f;/* 12位ADC */
		//if(vfo[VFO_A].Mode >= DEMOD_LSB )ads.pow = ads.fwd * 0.012207f * ads.fwd *0.012207f * 0.15f;/* 12位ADC */
		//ads.pow = ads.fwd * 0.0118f * ads.fwd *0.0118f * 0.2f;/* 12位ADC */
		//ads.pow = ads.fwd * 0.00078f * ads.fwd *0.00078f * 0.2f;/* 16位ADC */
		//if(ads.pow <0)ads.pow*=-1;
		
		//if(ads.pow>1)ads.swr = (ads.fwd*ads.fwd + ads.ref*ads.ref*14.0f)*10
		//						/(ads.fwd*ads.fwd - ads.ref*ads.ref*14.0f);/* 驻波计算 */
		
		//LCD_RectangleWindow(252, 98, 64, 20, RED);/* 窗口边框 */
		//LCD_ShowString(256,100,YELLOW,GRAY5 ,16,"SWR");
		if(ads.swr<20)
		{	
			ui_Decimal_Dispaly(34,155,GRAY0,BLACK,16, ads.swr);/* 驻波小于2数值显示底色为黑色 */
			//sd.Pow = AT24CXX_ReadLenByte(ADDRPOW_MM+vfo[VFO_A].Band_id, 1);
		}
		else
		if(ads.swr>=20 && ads.swr <30)
		{
			ui_Decimal_Dispaly(34,155,GRAY0,YELLOW2 ,16, ads.swr);/* 驻波大于2与小于3数值底色为黄色 */
			//if(sd.Pow >100)sd.Pow =100;
		}
		else
		if(ads.swr>=30)
		{
			//if(sd.Pow >100)sd.Pow =100;
			//if(ads.swr>=99)ads.swr=99;
			ui_Decimal_Dispaly(34,155,GRAY0,RED ,16, ads.swr);/* 驻波大于3数值底色为红色 */
		}
		if(++p>5)p=0;
		
		if(ads.pow > pow_0)pow_0 =ads.pow;
		if(ads.pow <= pow_0)
		{
			if(ads.pow >=3)
			{
				if(ads.pow < pow_0-3)pow_0 -=pow_0*0.1f;
			}
			else pow_0 -=pow_0*0.1f;
		}
		ads.Signal_table = (u8)(pow_0 * 0.75f);
			
		if(p==0)ui_Decimal_Dispaly(9,133,GRAY0,BLACK  , 16, (u16)pow_0);/* 显示数值 */
		//if(sd.menu_page ==1)sd.Pow =50;
		if(sd.Pow ==50&&vfo[VFO_A].Mode <  DEMOD_LSB)
		{			
			if(ads.pow>50&&ads.pow<53)AT24CXX_WriteLenByte(vfo[VFO_A].Band_id*2 + ADDRPOWER_GAIN , ads.pow_gain[vfo[VFO_A].Band_id],2);
			if(sd.menu_page ==1)LCD_ShowNum(160,132,GRAY1,BLACK, 4,16,ads.pow_gain[vfo[VFO_A].Band_id ]);
		}
	}
	else
	{
		
		//ads.amp_adjust_num = (u8)((riss -78)*0.2f);
		//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
		riss_0 = riss ;
//		if(riss > riss_0)riss_0 = riss ;
//		if( vfo[VFO_A].Mode >= DEMOD_LSB && vfo[VFO_A].Mode <= DEMOD_USB_DIG)
//		{
//			riss_0 -= 20;
//		}
		
//		//if(riss < (u16)riss_0)
//		{
//			riss_0 -= riss_0 * 0.01f;
//		}
		
		//riss_0 = riss;
		
		if((u16)riss_0 <=54)ads.Signal_table = (u16)(riss_0*1.5f);
		else
		if((u16)riss_0 >54 )ads.Signal_table = (u16)(81 + (riss_0 -54)*1.2f );
		
		/* 静噪控制 */
		if(sd.Sql>1 || ads.tx_delay >10)
		{
			if(riss>sd.Sql+1)
			{
				ads.Af_mute =1;/* 音频开启 */
				ui_TopBackground(232,4,8,16);
				sql_timer =50;
			}
			else
			if(riss<sd.Sql )
			{
				if(ks.Enc2_Use ==1||vfo[VFO_A].Mode > DEMOD_USB_DIG)
				{
					ads.Af_mute =0;/* 音频关闭 */	
					ui_Mute_icon(220,4,RED,ks.Spk_key);
				}
				else
				{
					sql_timer--;
					if(sql_timer<1)
					{
						sql_timer =0;
						ads.Af_mute =0;/* 音频关闭 */	
						ui_Mute_icon(220,4,RED,ks.Spk_key);
					}
				}
			}
		}
		else
		{
			ads.Af_mute =1;/* 音频开启 */
			ui_TopBackground(232,4,8,16);
		}
	}
	
	/* RX信号强度和TX发射功率动态显示 */
	if(ads.Signal_table >155)ads.Signal_table =155;/* 限定最大值，避免超出显示范围 */
	
	for(y0=0;y0<7;y0+=1)/* 单位动态显示符的高度 7个像素 */
	{
		if(RT_key ==CONTROL_RX)
		{
			for(i=0;i<155;i+=1)/* 单位动态显示符宽度 3个像素 */					
			{
				if(i<ads.Signal_table)
				{
					Gui_DrawPoint(x+48+0+i,y+16+y0,fc);/* 颜色显示 */
				}
				else if(i>ads.Signal_table)
					Gui_DrawPoint(x+48+0+i,y+16+y0,bc);/* 消隐 */
			}
		}
		else
		{
			for(i=0;i<155;i+=1)/* 单位动态显示符宽度 3个像素 */					
			{
				if(i<ads.Signal_table)
				{
					Gui_DrawPoint(x+48+i,y+16+y0,RED);/* 高于S9显示红色 */
				}
				else if(i>ads.Signal_table)
					Gui_DrawPoint(x+48+i,y+16+y0,bc);/* 消隐 */
			}
		}
	}
	
//	if(pow_1<ads.Signal_table)
//	{
//		pow_1=ads.Signal_table;
//	}
//	if(++k ==11) k =0;
//	if(pow_1>=ads.Signal_table)
//	{
//		
//		if(k>5)pow_1 -=pow_1*0.1f;
//		
//	}
//	if(RT_key ==CONTROL_RX)ui_Signal_scale(x, y,pow_1,GREEN);
//	else ui_Signal_scale(x, y,pow_1,RED);
}
/*
********************************************************************************
* 波段显示界面
********************************************************************************
*/
/* 波段显示信息结构体 */
typedef struct
{
	u16 x;/* 起点X*/
	u16 y;/* 起点Y*/
	//u8 font;
	char *text;/* 显示的文本 */
}UI_BandDisplay_Information,UI_RxBandDisplay_Information;
/* 收发功能模式波段显示的信息 */
const  UI_BandDisplay_Information BandDispaly_path[] =
{
	//{0,0,0,},
	/* 第一行 */
	{ 50,	130,	" 80m "  },
	{120,	130,	" 60m "  },
	{190,	130,	" 40m "  },
	/* 第二行 */
	{ 50,	158,	" 30m "  },
	{120,	158,	" 20m "  },
	{190,	158,	" 17m "  },
	/* 第三行 */
	{ 50,	186,	" 15m "  },
	{120,	186,	" 12m "  },
	{190,	186,	" 10m "  },
	//{ 90,	186,	"  6m "  }
	//{160,100,24," 40m "  },
	//{230,100,24," 40m "  },
};
/* 接收机模式波段显示参数 */
const  UI_RxBandDisplay_Information Rx_BandDispaly_path[] =
{
	//{0,0,0,},
	/* 第一行 */
	{ 15,	130,	"  MW "  },
	{ 65,	130,	"120M "  },
	{115,	130,	" 90m "  },
	{165,	130,	" 75m "  },
	{215,	130,	" 60m "  },
	{265,	130,	" 49m "  },
	/* 第二行 */
	{ 15,	150,	" 41m "  },
	{ 65,	150,	" 31m "  },
	{115,	150,	" 25m "  },
	{165,	150,	" 22m "  },
	{215,	150,	" 19m "  },
	{265,	150,	" 16m "  },
	/* 第三行 */
	{ 15,	170,	" 15m "  },
	{ 65,	170,	" 13m "  },
	{115,	170,	" 11m "  },
	{165,	170,	"  6m "  },
	{215,	170,	"  2m "  },
	{265,	170,	" 2m5 "  }
};
	/*
	* 收发信机的波段显示
	* mode: 工作模式；0：收发模式波段显示；1：接收机模式波段显示；
	* font: ASCII字符字体；
	* band_idx: 波段选择信息；
	*/
void ui_BandDisplay(u8 mode,u8 band_idx)
{
	u8 i;
	u16 fc;
	
	//Lcd_Color(0,130,319,210,BLACK);/* 显示区域清屏 */
	
	if(mode ==0)/* 收发模式波段显示 */
	{
		Lcd_Color(0,130,319,210,BLACK);/* 显示区域清屏 */
		for(i=0;i<9;i++)
		{
			if(i &band_idx)fc =RED;/* 选中的波段字体为红色 */
			else
				fc =GRAY0;
			LCD_ShowString( BandDispaly_path[i].x ,  BandDispaly_path[i].y , fc, GRAY4 , 24, 0, BandDispaly_path[i].text );
			//LCD_RectangleWindow(u8 x, u8 y, u8 w, u8 h, u16 fc);
		}
	}
	//else
	if(mode ==1)/* 收发模式波段显示 */
	{
		Lcd_Color(0,130,319,210,BLACK);/* 显示区域清屏 */
		for(i=0;i<18;i++)/*接收机模式波段显示 */
		{
			if(i &band_idx)fc =RED;/* 选中的波段字体为红色 */
			else
				fc =GRAY0;
			LCD_ShowString( Rx_BandDispaly_path[i].x , Rx_BandDispaly_path[i].y , fc, GRAY4 , 16,0, Rx_BandDispaly_path[i].text );
			//LCD_RectangleWindow(u8 x, u8 y, u8 w, u8 h, u16 fc);
		}
	}
}
///*
//********************************************************************************
//* 频谱及瀑布图显示
//********************************************************************************
//*/
//typedef struct
//{
//	u16 x;/* 起点X*/
//	u16 y;/* 起点Y*/
//	//u8 font;
//	char *text;/* 显示的文本 */
//}SPE_Text_typedef;
//const SPE_Text_typedef	Spe_text[] =
//{
//	{  10,222,"5"},
//	{  40,222,"4"},
//	{  70,222,"3"},
//	{ 100,222,"2"},
//	{ 130,222,"1"},
//	{ 160,222,"0"},
//	{ 190,222,"1"},
//	{ 220,222,"2"},
//	{ 250,222,"3"},
//	{ 280,222,"4"},
//	{ 310,222,"5"},
//};
/*
* 频谱显示函数；
* 实现频谱以及瀑布图显示。
* 入口参数：
* x y为起始坐标，其中y是底部坐标由下向上显示，高度是spe_h+falls_h；
* width：宽度；
* spe_h: 频谱高度；
* falls_h：瀑布图高度；
* mode: 保留；
*/
//float32_t FFT_in[FFT_LENGTH*2];
/*__attribute__((section (".RAM_D1")))*/ //float32_t input[512];
//void ui_Spectrum_Dispaly(u16 x,u16 y,u16 width,u16 spe_h,u16 falls_h)
//{
//	static u16 clk =0;
//	 u16 x0,y0,fc;
//	static u16 k;
//	static float32_t input[512];
//	static  uint16_t  falls[20][320];
//	static u16 h;
//	 u16 i;
//	static float32_t FFT_in[FFT_LENGTH*2];
//	//static float32_t FFT_out[FFT_LENGTH];
//	if(clk ==0)
//	{
//		/* */
//		for(x0=0;x0<width;x0++)
//		{
//			Gui_DrawPoint(x0+x,y0+y,GRAY1);
//		}
//		for(x0=10;x0<width;x0+=30)
//		{
//			for(y0=0;y0<2;y0+=1)
//				Gui_DrawPoint(x0+x,y0+y+1,YELLOW );
//		}
//		for(i=0;i<11;i++)
//		{
//			LCD_ShowString(Spe_text[i].x-2 ,Spe_text[i].y ,YELLOW ,BLACK , 12, 0, Spe_text[i].text ); /* 大于0显示正号 */
//		}
//		clk =1;
//	}
//	if(spe.fft_flang ==1)
//	{			
//		for(i=0;i<FFT_LENGTH;i++)
//		{		
//			 FFT_in[i*2] = spe.fft_buf [i];
//			 FFT_in[i*2+1] =0;
//		}
//		//arm_cfft_f32(&arm_cfft_sR_f32_len1024, spe.spe_buf, 0, 1);
//		arm_rfft_fast_f32(&Spe_FFT ,  FFT_in,   FFT_in,0 );	//FFT计算（基4）
//		/***取出FFT模值***/
//		arm_cmplx_mag_f32(  FFT_in, FFT_in,512);
//			
//		for(k=0;k<width;k++)
//		{
//			/* 旧波形清楚 */
//			
//			for(i=input[k+1];i>0;i--)
//			//{
//				Gui_DrawPoint(k+1+x,y-i-20,BLACK);	
//			//}
//			/* 新波形数据提取 */
//			input[k] = FFT_in[k-1]*0.025 +  FFT_in[k]*0.05 +  FFT_in[k+1]*0.025;
//			falls[h][k] = (u16)input[k];
//			//input[k]=log2(input[k]);
//			arm_sqrt_f32( input[k]*0.0015625f,&input[k]);
//			//input[k]*=0.015625f;
//			/* 波形高度限制 */
//			//input[k] = FFT_out[k];
//			if(input[k]>falls_h)input[k]  = 60;
//			
//			if(input[k]<falls_h)input[k] = (u16)input[k]&0x2f;
//			
//			/* 绘制波形 */	
//			if(k>0)
//			{
//				/* 中线标识显示 */
//				if(k==width*0.5f)fc=YELLOW;
//				else
//				fc=BLUE;
//				/* 频谱显示 */
//				
//				for(i = input[k];i>0;i--)
//				{
//					Gui_DrawPoint(k,y-i-20,fc);
//					
//				}
//				
//				/* 瀑布图显示 */
//				for(i=0;i<falls_h;i++)
//				{
//					if(i<=h)
//					Gui_DrawPoint(k+x,y+i-20, (falls[h-i][k]<<2));
//					if(i>h)
//					Gui_DrawPoint(k+x,y+i-20, (falls[20+h-i][k]<<2));
//				}
//			}
//			/* 瀑布图更新时钟 */
//			if(k==0)
//			{
//				if(++h >falls_h-1)h =0;
//			}
//		}
//		spe.fft_flang =0;
//	}
//}
/*
********************************************************************************
*	频谱显示（未使用）
********************************************************************************
*/
void ui_Spectrum_Dispaly_2(u16 x,u16 y,u16 width,u8 spe_h,u8 falls_h,u16 *pSrc)
{
	u16 i;
	static u8 h;
	static u16 k;
	static u16 *Pin;
	static u16 **fall;
	
	h++;if(h >19)h=0;
	for(k=0;k<width;k++)
	{
		//*Pin =*pSrc;
		for(i=*(Pin+k+1);i>0;i--)
		{
			Gui_DrawPoint(k+1+x,y-i-20,BLACK);
		}
		*(Pin+k) =(*(pSrc+k-1)>>2)+((*pSrc+k)>>1)+(*(pSrc+k+1)>>2);
		
		*(*(fall+k)+h) = *(Pin+k);
		
		if(*(Pin+k) >spe_h)*(Pin+k) = spe_h;
		if(k>0)
		{
			for(i = *(Pin+k);i>0;i--)
			{
				Gui_DrawPoint(k,y-i-20,GRAY0);	
			}
		}
		for(i=0;i<falls_h;i++)
		{
			if(i<=h)
			Gui_DrawPoint(k+x,y+i-19, *(*(fall+k)+h-i));
			if(i>h)
			Gui_DrawPoint(k+x,y+i-19, *(*(fall+k)+h-i+20));
		}
		//Pin ++;
		//pSrc ++;
		
	}		
}
/*
********************************************************************************
	菜单显示窗口
********************************************************************************
*/
/*
*	x/y: 起点
*	h：  高度
*	fc: 划线颜色
*	bc: 背景颜色
*/
void ui_Meun_Window(u16 x, u16 y,u16 h, u16 fc, u16 bc)
{
	u16 x0,y0;
	Lcd_Color(x, y,319,h,bc) ;
	for(y0=0;y0<81;y0+=80)
	{
		for(x0=0;x0<280;x0+=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
	for(x0=0;x0<280;x0+=279)
	{
		for(y0=0;y0<81;y0+=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
}
/*
********************************************************************************
*	功能：显示音量编码器调整的参数选项名称 
********************************************************************************
*/
/*
*	左侧中部显示窗（左侧边功能显示窗）
*/
const  SideFeatureText_tybedef Function_L_text[]=
{
	{"VOL",	2,	 90},
	{"SQL",	2,	107},
};
//
void ui_SideFunction_Display()
{
	u8 i;
	u16 fc,fc0;
	static u8 clk;
	//static u8 enc2_key0;
	//static u8 enc2_long0;
	static u8 vol0;
	static u8 spl0;
	static u8 enc2_use0;
	if(enc2_use0 != ks.Enc2_Use)
	{
		enc2_use0 = ks.Enc2_Use;
		clk=0;
	}
	if(clk ==0 || vol0!=sd.Vol || spl0 !=sd.Sql)
	{
		for(i=0;i<2;i++)
		{
			if(i==ks.Enc2_Use && ks.EC1_12 ==0)
			{
				fc =RED;
				fc0 =GRAY0;
			}
			else 
			{
				fc =GRAY2;
				fc0 = GRAY2;
			}				
			LCD_RectangleWindow(Function_L_text[i].x, Function_L_text[i].y, 46, 14, fc);/* 窗口边框 */
			LCD_ShowString(Function_L_text[i].x+6,Function_L_text[i].y+1,fc0 ,BLACK , 12, 1, Function_L_text[i].name );
		}
		if(vol0!=sd.Vol || clk ==0)LCD_ShowNum(Function_L_text[0].x+30,Function_L_text[0].y+1,GRAY1,BLACK, 2,12,sd.Vol);
		if(spl0!=sd.Sql || clk ==0)LCD_ShowNum(Function_L_text[1].x+30,Function_L_text[1].y+1,GRAY1,BLACK, 2,12,sd.Sql);
		
		vol0=sd.Vol;
		spl0=sd.Sql;
		clk =1;
	}		
}
/*
*********************************************************************************************
	
*********************************************************************************************
*/
/*
*	dit/dah嘀字符显示函数
*/
void cw_sign_dit_dah(u16 x,u16 y,u16 fc,u8 mode)
{
	u8 x0,y0;
	if(mode >0)
	{
		for(x0=0;x0<6;x0++)//dah
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
		for(x0=0;x0<2;x0++)//dit
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+10+x,y0+y,fc);
		}
	}
	else
	{
		for(x0=0;x0<2;x0++)//dah
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
		for(x0=0;x0<6;x0++)//dit
		{
			for(y0=0;y0<2;y0++)
			Gui_DrawPoint(x0+6+x,y0+y,fc);
		}
	}
}
/*
*	右侧边显示窗
*/
//

const  SideFeatureText_tybedef  KeySwitch_MA_text[]=
{
	{"CW A",	3,	71},
	{"CW M",	3,	71},
	{"E",		43,	71}
	//{" - .",	278,	77},
	//{" . -",	278,	77},
};
//
const  SideFeatureText_tybedef  DSP_noise_text[]=
{
	{"NR",	6,	71},
	{"NB",	33,	71}
};
//
const  SideFeatureText_tybedef Function_CW_text[]=
{
	{"SIVOL",	254,	78},
	{"SITON",	254,	93},
	{"KEYSP",	254,	108},
};
//
const  SideFeatureText_tybedef Function_SSB_text[]=
{
	{" MIC ",	254,	78},
	{" DSP ",	254,	93},
	{" LEN ",	254,	108}
};
//
void ui_SideFunction_R_Display(u8 rt_key)
{
	int8_t  i;
	u16 fc,fc0;
	//static u8 rtkey0;
	static u8 vfo_a_mode0;
	static u8 clk;
	static u8 menukey0;
	//static u8 f_key0;
	static u8 enc2_key0;
	static u8 enc2_long0;
	static u8 mode0;
	static u8 nr_key0;
	static u8 not_key0;
	static u8 cw_keysw0;
	static u8 cw_Exercise0;
	static u8 cw_ab0;
	static u16 Menu_Values0[3];
	static u16 Menu_Values1[3]; 
	static u16 cw_Values[3]; 
	static u16 nr_Values[3];
	
	//if(rt_key !=CONTROL_RX)clk =0;
	nr_Values[0] = sd.mic_gain;
	nr_Values[1] = nr.dsp_nr_strength;
	nr_Values[2] = nr.dsp_nr_delaybuf_len;
	
	cw_Values[0] = sd.CW_vol;
	cw_Values[1] = sd.CW_Sidetone;
	cw_Values[2] = sd.CW_Speed ;
	for(i=0;i<3;i++)
	{
		if(Menu_Values0[i] != cw_Values[i] || Menu_Values1[i] != nr_Values[i])enc2_key0 = ~ks.EC1_12 *0x01;
	}
	if(menukey0 != ks.FunctionWindow_Use )enc2_long0 = ~ks.EC1_12_LONG &0x01;
	
	if(rt_key ==CONTROL_RX )//&& ks.FunctionWindow_Use ==0)//多功能窗口已解除占用)
	{
		if(clk ==0 || enc2_key0 != ks.EC1_12 || enc2_long0 != ks.EC1_12_LONG || mode0 != vfo[VFO_A].Mode || nr_key0 != ks.NR_key 
			|| not_key0 != ks.NOT_key || cw_keysw0 != ks.CW_keySW || cw_Exercise0 != ks.CW_Exercise || cw_ab0 != ks.CW_AB )
		{
			if(clk ==0|| vfo_a_mode0!=vfo[VFO_A].Mode)
			{
				Lcd_Color(252, 77,319,127,BLACK ) ;/* 显示区域清屏 */
				clk =1;
				
				vfo_a_mode0=vfo[VFO_A].Mode;
			}
			if(vfo[VFO_A].Mode < DEMOD_LSB )
			{
				Lcd_Color(0, 68,54,86,BLACK ) ;/* 显示区域清屏 */
				//LCD_ShowString(KeySwitch_MA_text[ks.CW_AB+3].x+1,KeySwitch_MA_text[ks.CW_AB+3].y,GRAY0 ,BLACK , 12, 0, KeySwitch_MA_text[ks.CW_AB+3].name );
				cw_sign_dit_dah(30,77,YELLOW,ks.CW_AB);
				if(ks.CW_Exercise ==1)
				{
					fc =RED;
					fc0 =GRAY0;
					//fc1 =YELLOW;
				}
				else 
				if(ks.CW_Exercise ==0)	
				{
					fc =GRAY2;
					fc0 = GRAY2;
					//fc1 = GRAY2;
				}
				if(sd.menu_page==1)ks.CW_keySW =1;
				//cw_sign_dit_dah(285,105,fc1,ks.CW_AB);
				LCD_RectangleWindow(KeySwitch_MA_text[0].x-2, KeySwitch_MA_text[2].y-1, 52, 14, RED);/* 窗口边框 */
				//LCD_ShowString(KeySwitch_MA_text[i].x+1,KeySwitch_MA_text[i].y,BLACK ,BLACK , 12, 0, "   ");
				LCD_ShowString(KeySwitch_MA_text[2].x+1,KeySwitch_MA_text[2].y,fc0 ,BLACK , 12, 0, KeySwitch_MA_text[2].name );
				//LCD_RectangleWindow(KeySwitch_MA_text[ks.CW_keySW].x-2, KeySwitch_MA_text[ks.CW_keySW].y-1, 31, 14, RED);/* 窗口边框 */
				//LCD_ShowString(KeySwitch_MA_text[i].x+1,KeySwitch_MA_text[i].y,BLACK ,BLACK , 12, 0, "   ");
				LCD_ShowString(KeySwitch_MA_text[ks.CW_keySW].x+1,KeySwitch_MA_text[ks.CW_keySW].y,GRAY0 ,BLACK , 12, 0, KeySwitch_MA_text[ks.CW_keySW].name );
				
			}
			else
			{
				Lcd_Color(0, 68,54,86,BLACK ) ;/* 显示区域清屏 */
				for(i=0;i<2;i++)
				{
					if(i==0 && ks.NR_key ==1)
					{
						fc =RED;
						fc0 =GRAY0;
					}
					else 
					if(i==0 && ks.NR_key ==0)	
					{
						fc =GRAY2;
						fc0 = GRAY2;
					}
					//
					if(i==1 && ks.NOT_key ==1)
					{
						fc =RED;
						fc0 =GRAY0;
					}
					else 
					if(i==1 && ks.NOT_key ==0)	
					{
						fc =GRAY2;
						fc0 = GRAY2;
					}
					LCD_RectangleWindow(DSP_noise_text[i].x-5, DSP_noise_text[i].y-1, 25, 14, fc);/* 窗口边框 */
					//LCD_ShowString(DSP_noise_text[i].x+1,DSP_noise_text[i].y,BLACK ,BLACK , 12, 0, "   ");
					LCD_ShowString(DSP_noise_text[i].x+1,DSP_noise_text[i].y,fc0 ,BLACK , 12, 0, DSP_noise_text[i].name );
				}
			}
			//
			for(i=0;i<3;i++)
			{
				if(i==ks.EC1_12-1 && ks.EC1_12_LONG ==0)
				{
					fc =RED;
					fc0 =GRAY0;
				}
				else 
				if(i!=ks.EC1_12-1 && ks.EC1_12_LONG ==0)	
				{
					fc =GRAY2;
					fc0 = GRAY2;
				}
				//if(i<2)
				//{
				//	LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 31, 19, fc);/* 窗口边框 */
				//}
				//else 
				//LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* 窗口边框 */
				
				if(vfo[VFO_A].Mode < DEMOD_LSB)
				{
					LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* 窗口边框 */
					LCD_ShowString(Function_CW_text[2-i].x+2,Function_CW_text[2-i].y,fc0 ,BLACK , 12, 0, Function_CW_text[2-i].name );
					LCD_ShowNum(Function_CW_text[2-i].x+42,Function_CW_text[2-i].y,fc0 ,BLACK , 3,12,cw_Values[2-i]);
				}
				else
				{
					LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 64, 14, fc);/* 窗口边框 */
					LCD_ShowString(Function_SSB_text[i].x+2,Function_SSB_text[i].y,fc0 ,BLACK , 12, 0, Function_SSB_text[i].name );
					LCD_ShowNum(Function_SSB_text[i].x+42,Function_SSB_text[i].y,fc0 ,BLACK , 3,12,nr_Values[i]);
				}
			}
			//
			enc2_key0 = ks.EC1_12; 
			enc2_long0 = ks.EC1_12_LONG;
			mode0 = vfo[VFO_A].Mode;
			nr_key0 = ks.NR_key;
			not_key0 = ks.NOT_key;
			cw_keysw0 = ks.CW_keySW;
			menukey0 = ks.FunctionWindow_Use;
			cw_Exercise0 = ks.CW_Exercise;
			cw_ab0 = ks.CW_AB;
			//clk =1;
			for(i=0;i<3;i++)
			{
				Menu_Values0[i] = cw_Values[i];
				Menu_Values1[i] = nr_Values[i];
			}
		}
//		/**/
//		if( spe.fft_max >39)
//		{
//			for(i=0;i<3;i++)
//			{
//				if(i==ks.EC1_12-1 && ks.EC1_12_LONG ==0)
//				{
//					fc =RED;
//					fc0 =GRAY0;
//				}
//				else 
//				if(i!=ks.EC1_12-1 && ks.EC1_12_LONG ==0)	
//				{
//					fc =GRAY2;
//					fc0 = GRAY2;
//				}
//				//if(i<2)
//				//{
//				//	LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 31, 19, fc);/* 窗口边框 */
//				//}
//				//else 
//				//LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* 窗口边框 */
//				
//				if(vfo[VFO_A].Mode < DEMOD_LSB)
//				{
//					LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* 窗口边框 */
//					LCD_ShowString(Function_CW_text[2-i].x+2,Function_CW_text[2-i].y,fc0 ,BLACK , 12, 0, Function_CW_text[2-i].name );
//					LCD_ShowNum(Function_CW_text[2-i].x+42,Function_CW_text[2-i].y,fc0 ,BLACK , 3,12,Menu_Values[7-i]);
//				}
//				else
//				{
//					LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 64, 14, fc);/* 窗口边框 */
//					LCD_ShowString(Function_SSB_text[i].x+2,Function_SSB_text[i].y,fc0 ,BLACK , 12, 0, Function_SSB_text[i].name );
//					LCD_ShowNum(Function_SSB_text[i].x+42,Function_SSB_text[i].y,fc0 ,BLACK , 3,12,Menu_Values[i+13]);
//				}
//			}
//			spe.fft_max =0;
//		}
	}
	else
	{
		clk =0;	
	}
	
	if( spe.fft_max >39)
		{
			for(i=0;i<3;i++)
			{
				if(i==ks.EC1_12-1 && ks.EC1_12_LONG ==0)
				{
					fc =RED;
					fc0 =GRAY0;
				}
				else 
				if(i!=ks.EC1_12-1 && ks.EC1_12_LONG ==0)	
				{
					fc =GRAY2;
					fc0 = GRAY2;
				}
				//if(i<2)
				//{
				//	LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 31, 19, fc);/* 窗口边框 */
				//}
				//else 
				//LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* 窗口边框 */
				
				if(vfo[VFO_A].Mode < DEMOD_LSB)
				{
					LCD_RectangleWindow(Function_CW_text[2-i].x-2, Function_CW_text[2-i].y-1, 64, 14, fc);/* 窗口边框 */
					LCD_ShowString(Function_CW_text[2-i].x+2,Function_CW_text[2-i].y,fc0 ,BLACK , 12, 0, Function_CW_text[2-i].name );
					LCD_ShowNum(Function_CW_text[2-i].x+42,Function_CW_text[2-i].y,fc0 ,BLACK , 3,12,cw_Values[2-i]);
				}
				else
				{
					LCD_RectangleWindow(Function_CW_text[i].x-2, Function_CW_text[i].y-1, 64, 14, fc);/* 窗口边框 */
					LCD_ShowString(Function_SSB_text[i].x+2,Function_SSB_text[i].y,fc0 ,BLACK , 12, 0, Function_SSB_text[i].name );
					LCD_ShowNum(Function_SSB_text[i].x+42,Function_SSB_text[i].y,fc0 ,BLACK , 3,12,nr_Values[i]);
				}
			}
			spe.fft_max =0;
		}
}
/*
********************************************************************************
*	条形显示窗 
********************************************************************************
*/
void ui_LongStrip_DisplayWindow(u16 x,u16 y,u8 var)
{
	 u8 x0,y0,i;
	static u8 var_0 ;
	
	if(var_0 != var)
	{
		var/=2;
		if(var >50)var =50;
		for(x0=var_0;x0<3+var_0;x0++)
		{
			for(y0=0;y0<14;y0++)
			{
				Gui_DrawPoint(x0+x,y0+y-6,BLACK);
			}
		}
	
		for(x0=var;x0<3+var;x0++)
		{
			for(y0=0;y0<14;y0++)
			{
				Gui_DrawPoint(x0+x,y0+y-6,RED);
			}
		}
	
		for(i=0;i<52;i++)
		{
			for(y0=0;y0<2;y0++)
			{
				if(i<var+3)Gui_DrawPoint(x+i,y0+y,RED);
				else
					Gui_DrawPoint(x+i,y0+y,GRAY2);
			}
		}
		var_0 = var;
	}
}
/*
********************************************************************************
*	音量显示 
********************************************************************************
*/
void ui_VOL_DisPlay(u16 VOL_van,char *zi,u8 num)
{
	//u16 fc;
	static u8 num0;
	static u8 clk =0;
	//static u16 vol_van0;
	//static u16 sdvol0;
	//static u8 mark_vol0;
	if(ks.Enc2_Use ==num )
	{
		if(num0 != num){clk=0;num0 = num;}
		if(sd.Enc2_delay >0)
		{
			sd.Enc2_delay -=1;
			ads.vol_timer =VOL_WINDOW_DELAY;
			//ks.agc_m_adjust =0;
			//if(ads.vol_timer<1)ads.vol_timer =0;	
			if(clk ==0)
			{
				//Lcd_Color(125, 140,80,164,BLACK ) ;/* 显示区域清屏 */
				Lcd_Color(60, 131,234,156,GRAY4 ) ;/* 显示区域清屏 */
				if(ks.Enc2_Use ==2)Lcd_Color(117, 136,168,151,BLACK );
				else Lcd_Color(134, 136,185,151,BLACK );
				clk =1;
			}
			
			LCD_RectangleWindow(59, 130, 177, 28, GRAY4);/* 窗口边框 */
			LCD_RectangleWindow(60, 131, 175, 26, GRAY4);/* 窗口边框 */
			LCD_ShowString(67,132,YELLOW ,GRAY4 , 24, 0, zi); /* 大于0显示正号 */
			if(ks.Enc2_Use ==2)
			{
				if(TR_READ ==CONTROL_TX)sd.Enc2_delay =VOL_WINDOW_DELAY;
				ui_Decimal_Dispaly(180,132,GRAY0,GRAY4 ,24,VOL_van);/* 显示数值 */
				ui_LongStrip_DisplayWindow(117,143,VOL_van/2);/* 音量指示条显示 */
			}
			else if(ks.Enc2_Use ==6)
			{
				LCD_Showvol(180, 132, GRAY0, GRAY4 , 4, 24, VOL_van);/* 显示数值 */
			}
			else
			{
				LCD_Showvol(193, 132, GRAY0, GRAY4 , 3, 24, VOL_van);/* 显示数值 */
				//VOL_van/=2;
				ui_LongStrip_DisplayWindow(134,143,VOL_van);/* 音量指示条显示 */
			}
		}
	}
	//else
	if(clk ==1)
	{
		if(sd.Enc2_delay<1)
		{
			ks.Enc2_Use =0;
			ks.EC1_12 =0;
			ks.EC1_12_LONG =0;
			ads.vol_timer =0;
			ks.agc_m_adjust =0;
			ks.IF_shift =0;
			Lcd_Color(59, 130,240,158,BLACK ) ;/* 显示区域清屏 */
			sd.spe_fil_time = SPE_FIL_TIME;//频谱带宽显示回复计时标志
			clk =0;
		}
	}
}
/*
********************************************************************************
*	功率显示 
********************************************************************************
*/
void ui_Power_Display(u8 data)
{
	//u16 fc;
	static u8 clk =0;
	//static u8 data_0;
	//static u8 rt_key0;
	//static u8 Enc2_Use0;
	
	if(ks.Enc2_Use ==2|| (clk>>1)&0x01)
	{
		if(sd.Enc2_delay >0)
		{
			sd.Enc2_delay -=1;
			if((clk&0x01) ==0)
			{
				Lcd_Color(180, 130,245,159,BLACK ) ;/* 显示区域清屏 */
				clk |=1<<0;
				clk |=1<<1;
			}
			//if(vfo[VFO_A].Mode ==DEMOD_AM)data /=2;
			LCD_RectangleWindow(180, 131, 64, 27, RED);/* 窗口边框 */
			LCD_ShowString(230,132,YELLOW ,BLACK , 24,0, "W"); /* 大于0显示正号 */
			ui_Decimal_Dispaly(182,132,GRAY0,BLACK  ,24,data);/* 显示数值 */
			//data_0 = data;
		}
		else
		{
			if(clk&0x01)
			{
				//ks.Enc2_Use =0;
				ks.PWR_key =0;
				Lcd_Color(180, 131,245,159,BLACK ) ;/* 显示区域清屏 */
				//LCD_RectangleWindow(252, 98, 64, 27, GRAY2);/* 窗口边框 */
				//LCD_ShowString(302,99,GRAY2 ,BLACK , 24,0, "W"); /* 大于0显示正号 */
				//ui_Decimal_Dispaly(254,99,GRAY2,BLACK  ,24,data);/* 显示数值 */
				//clk |=~(1<<0);
				sd.spe_fil_time = SPE_FIL_TIME;//频谱带宽显示回复计时标志
				clk =0;
			}
		}
	}
}
/*
********************************************************************************
*	DSP_NR 降噪显示 
********************************************************************************
*/
void ui_dspNR_Display(u8 nr_key,u8 nr_select)
{
	static u8 clk=0;
	//static u8 nr_key0;
	//static u8 nr_select0;
	//u8 fc,bc;
	if(clk==0 || nr_key !=0)
	{
		Lcd_Color(252, 98,319,200,BLACK ) ;/* 显示区域清屏 */
		LCD_RectangleWindow(252, 98, 64, 63, RED);/* 窗口边框 */
		Gui_DrawLine(284, 99,284, 115,GRAY2);
		//Gui_DrawLine(252, 98,316, 98,GRAY2);
		Gui_DrawLine(253, 116,314, 116,GRAY2);
		Gui_DrawLine(253, 131,314, 131,GRAY2);
		Gui_DrawLine(253, 146,314, 146,GRAY2);
		//Gui_DrawLine(252, 161,316, 161,GRAY2);
		
		//Gui_DrawLine(252, 98,252, 161,GRAY2);
		//Gui_DrawLine(316, 98,316, 161,GRAY2);
		clk =1;
	}
}









