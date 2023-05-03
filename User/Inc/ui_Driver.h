/*
*******************************************************
*
*
*******************************************************
*/
#ifndef __uiDriver_H
#define __uiDriver_H		
#include "sys.h"	 
#include "stdlib.h"
#include "arm_math.h"

typedef struct
{
	char *text;
	u8 width;
	u8 spe_width;
}Filter_parameters;
extern const  Filter_parameters Fil_p[];
//
typedef struct
{
	char *Mode[8];/* 模式显示文本数组 */
	char *V_CH[2];/* VFO/CH转换显示文本数组 */
	char *A_B[2]; /* VFO A/B转换显示文本数组 */
	char *RT[4];  /* TX/RX文本数组 */
}TWXT_tybedef;
extern  const  TWXT_tybedef text;

typedef struct
{
	char *name;/* 模式显示文本数组 */
	u16  x;
	u16  y;
}SideFeatureText_tybedef;
extern  const  SideFeatureText_tybedef Function_L_text[];
/* 顶部显示区域 */
void ui_Mute_icon(u16 x,u16 y,u16 bc,u8 spk);
void ui_Spe_Filter_Display(u16 fil_idx,int8_t mode,u8 clk,u16 bc,u8 h_mode);
void ui_TopBackground(u16 x,u16 y,u16 width,u16 h);/* 顶部区域底色 */
void key_lock_Display(u16 x);//按键锁
void ui_ATT_Display(u8 k);  /* ATT显示     */
void ui_Decimal_Dispaly(u16 x,u16 y,u16 fc,u16 bc,u8 font,u16 num);/*小数显示*/
void ui_Tim_Dispaly(u16 x,u16 y,u16 fc,u16 bc,char font,u8 on);
void ui_Vfo_Display(u8 ch_num);
void ui_Filter_Display(u16 x,u16 y,u16 fc,u16 bc,u8 idx,int16_t mode);
void ui_Step_Display(u16 x,u16 y,int8_t idx);
void ui_RIT_Display(int16_t data );
void ui_Signal_intensity_Display(u16 x,u16 y,u16 fc,u16 bc,u16 irss_po,u8 RT_key);
void ui_BandDisplay(u8 mode,u8 band_idx);

void ui_Spectrum_Dispaly(u16 x,u16 y,u16 width,u16 spe_h,u16 falls_h);
void ui_Spectrum_Dispaly_2(u16 x,u16 y,u16 width,u8 spe_h,u8 falls_h,u16 *pSrc);
void ui_Meun_Window(u16 x, u16 y,u16 h, u16 fc, u16 bc);
void ui_LongStrip_DisplayWindow(u16 x,u16 y,u8 var);
void ui_VOL_DisPlay(u16 VOL_van,char *zi,u8 num);
void ui_Power_Display(u8 data );
void ui_dspNR_Display(u8 nr_key,u8 nr_select);
void ui_SideFunction_Display(void);
void ui_SideFunction_R_Display(u8 rt_key);
#endif
