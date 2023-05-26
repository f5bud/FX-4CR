#ifndef __KEY_CONTROL_H
#define __KEY_CONTROL_H
#include "sys.h"
#include "arm_math.h"
/*
**************************************************************************************************************
*
*	
*	MATCH_BOX 按键口线分配：
*		K1 键       : PB7   (低电平表示按下)  LNA
*		K2 键       : PB6    (低电平表示按下)	SPE
*		K3 键       : PB14    (低电平表示按下)	MODE
*		K4 键   	: PC6    (低电平表示按下)	BAND
*		K5 键 		: PC7    (低电平表示按下)	SQL/OK
*		K6 键 		: PC8   (低电平表示按下)	SET
*		K7 键		: PC9   (低电平表示按下)	FIL
*		K8 键   	: PA8   (低电平表示按下)	A/B
*		K9 键		: PA11	 VFO
*		K10键		: PA13	 MEUN
*		DOW键			
*		UP 键
*
**************************************************************************************************************
*/
#define KEYLOCK_ON		0	/* 键盘解锁 */
#define KEYLOCK_OFF		1	/* 键盘锁定 */
/*
 *	定义按键代寄存器
 */
typedef struct 
{
	u32 DOWN;
	u32 UP;
	u32 LONG;
}Key_Reg_type;
extern __IO Key_Reg_type	*KEY;
/*
 *	定义按键代码名称
 */
typedef struct 
{
	__IO uint8_t  key_lock; 	//按键锁；F键长按转换
	__IO uint8_t  F_11; 		//键盘功能转换，0：功能键；1：波段转换。
	__IO uint8_t  A_B;
	__IO uint8_t  A_B_vfo;
	__IO uint8_t  ch_vfo;
	__IO uint8_t  rt_rx;
	__IO uint8_t  sim_dif;
	__IO uint8_t  Band_key;
	__IO int8_t  Band_id;	//收发模式波段ID
	__IO int8_t  RxBand_id; //接收机模式波段ID
	/*-------------------------------------------------------------------------------------*/
	__IO uint8_t  mode_Reg;	   /*
								*	机器模式操控
								*	mode_reg:
								*				位0：VFO A/B选择，0：VFO_A,1: VFO_B;
								*				位1：信道CH/VFO选择，0：VFO，1：CH;
								*				位2：电台的工作模式，0：收发模式，1：只接收模式；
								*				位3-位5：保留；
								*				位6：同频/异频收发选择，0：收发同频，1：收发异频；
								*				位7：收发转换，0：接收RX，1：发射TX；
								*/
	/*---------------------------------------------------------------------------------------*/
	__IO int8_t  Mode;
	__IO uint8_t CW_keySW;
	__IO uint8_t CW_Exercise;
	__IO uint8_t CW_AB;
	//__IO uint8_t cw_B;
	__IO int8_t  FilterSelected_id;
	__IO int8_t  IF_shift;
	__IO int8_t  StepSelected_idx;
	__IO uint8_t  RIT_key;		//RIT开关，0，关闭，1：开启。
	__IO uint8_t  RIT_LONG;
	__IO uint8_t  ATT_key;		//ATT开关，0：不启用ATT, 1：启用ATT.
	__IO uint8_t  AGC_Constant; //AGC恢复时间控制
	__IO uint8_t  PWR_key;		//功率调整开关。
	__IO uint8_t  PWR_LONG;
	__IO uint8_t  menu_key;
	__IO uint8_t  Spk_key;
	__IO uint8_t  NR_key;
	__IO uint8_t  NOT_key;
	__IO uint8_t  NR_project;
	//__IO uint8_t  Spe_clk;
	__IO uint8_t  vfo_read;
	__IO uint8_t  Enc0_Use;
	__IO int8_t  Enc2_Use;
	__IO uint8_t  EC1_12;
	__IO uint8_t  EC1_12_LONG;
	//__IO uint8_t  Selected_idx;
	__IO uint8_t  Ec2_up;
	__IO uint8_t  EC2_13_LONG;
	__IO uint8_t  agc_rest;
	__IO uint8_t  agc_m_adjust;
	__IO uint8_t  FunctionWindow_Use;
	__IO uint8_t  tx_no;
	__IO uint8_t  bt_key;
	__IO uint8_t	rit_tx;	
		 u8 cat_ptt:8;
		 u8 rx_adjuset_key;
}KeyManagement_TypeDef;
extern  KeyManagement_TypeDef	ks;
/*
*	
*/
typedef struct 
{	
	float32_t Dsp_Lo1;
	  float32_t Dsp_Bfo;
	__IO  int16_t  rit:16;
	
		  int16_t	Pow; /*功率设置 */
	__IO  uint8_t menu:8;
	__IO  uint8_t menu_read:8;
	//__IO  uint32_t tune_step;
	int8_t addr_a:8;
	int8_t addr_b:8;
	int8_t num_a:8;
	 int8_t num_b:8;
	 int16_t Enc0_Value:16;
	 int16_t Enc2_Value:16;
	 int16_t Enc_FREQ_Var:16;
	 int16_t Enc_VOL_Var:16;
	__IO int16_t Enc2_delay:16;
		 int16_t rit_delay:16;	
	//_
	__IO u16 vfo_read:16;
	__IO u16 menu_time:16;
		  u8 menu_end:8;
		  u8 menu_page:8;
	__IO u16 IF_1:16;			/*第一中频滤波器中心频率 */
	__IO u32 Lo1;			/*第一本振频率 */
	__IO u32 Tcxo;			/*一本振频率校准 */
	     //u16 IF_gain[15];     		// DSP中频 
		 u8  band;
	__IO u32 Lo2; 			/*第二本振频率 */
	__IO u32 Freq;			/*K4按键次数 */
	__IO u32 tuning_step;
		 u32 FFT;
	//__IO s8 ch;	/*频道号*/
	__IO int8_t Sql:8;		// 静噪门限调节
	__IO u16 TX_Delay_cw:16;	// 发射延时
	__IO u16 CW_Delay:16;		// CW接收延时
	__IO u16 CW_Sidetone:16;	// CW音调调整
	__IO  u8 CW_Speed:8;		// CW自动键速度调整 
	__IO  int8_t CW_vol:8;		// CW侧音音量
	__IO u16 Rx_delay:16;		// 接收延迟
	__IO u16 TIM2_Arr:16;
	__IO u16 TIM2_Arr_vol:16;
	__IO u8  TX_flong:8;	/*收发转换*/
		 u8  TX_spe;
	__IO int8_t  Vol:8;
		uint32_t  agc_Start;
	__IO  uint8_t  agc_ma:8;
	//u16 tx_amp:16;
	__IO uint16_t Riss:16;
	__IO uint16_t Riss0:16;
	__IO int8_t		S_correct:8;
		 int8_t	  	mic_gain:8;
		 int8_t	  	alc_start:8;
	__IO uint16_t key_BL_delay;
	__IO uint16_t band_set_delay;
		 u8 Rx_amp_ma:8;
		 u8 amp_set:8;
		 u8 amp_phase:8;
		 u8 ritatt_ptt:8;
		 u8 cw_fil_id:8;
		 u8 lsb_fil_id:8;
		 u8 usb_fil_id:8;
		 u8 am_fil_id:8;
		 u8 fm_fil_id:8;
		 u8 spe_fil_id:8;
		 u8 spe_fil_time:8;
		 int8_t spe_fil_mode:8;
		 u8 alc_mic:8;
		 u8 enc_freq:8;
		 u8 enc_exti:8;
		 u8 tx_filter:8;
		 u16 fm_offset:16;
		 int16_t IF_shift:16;
		 u8 pow_corre:8;
	__IO uint16_t txband_no:16;	 
}Storage_Data;
//
extern Storage_Data 	sd;
/*
*
*/
//#define		SSB_BFO_1000 				1180
#define		SSB_BFO_1100 				1100//1060.0f
#define		SSB_BFO_1200 				1200//1120.0f
#define		SSB_BFO_1300 				1300//1260.0f
#define		SSB_BFO_1400 				1400//1460.0f
#define		SSB_BFO_1500 				1500//1540.0f
#define		SSB_BFO_1600 				1600//1640.0f

/*
*	波段
*/
#define BAND_ON		0X0001	//IO输出高电平
#define BAND_OFF	0X0000	//IO输出低电平
typedef enum
{
    //COUPLING_2200M = 0,
    //COUPLING_630M,
    //COUPLING_160M,
    COUPLING_80M = 0,
	COUPLING_60M,
    COUPLING_40M,
	COUPLING_30M,
    COUPLING_20M,
	COUPLING_17M,
    COUPLING_15M,
	COUPLING_12M,
	COUPLING_10M,
    COUPLING_6M,
   // COUPLING_2M,
   // COUPLING_70CM,
   // COUPLING_23CM,
    COUPLING_MAX
}BandName_Type;
//
typedef enum
{
    RADIO_MW = 0,
    RADIO_120M,
    RADIO_90M,
	RADIO_75M,
	RADIO_60M,
    RADIO_49M,
	RADIO_41M,
	RADIO_31M,
    RADIO_25M,
	RADIO_22M,
	RADIO_19M,
	RADIO_16M,
    RADIO_13M,
	RADIO_11M,
	RADIO_10M,
    //RADIO_6M,
    //RADIO_2M5,
    //RADIO_2M,
    RADIO_MAX
}RxBandName_Type;
/*
*	SSB滤波器中心频率
*/
typedef struct
{
	u16 min;
	u16 Default;
	u16 max;
}Filte_Center_TypeDef;
extern Filte_Center_TypeDef	filte_center[6];
extern uint16_t Filte_Center[6];
/*
*	波段频率记忆
*/
//#define ADDR_RT_BAND_FREQ_M	1900//波段频率存储地址
//#define ADDR_RX_BAND_FREQ_M	1950//波段频率存储地址
typedef struct
{
	uint16_t  x:16;
	uint16_t  y:16;
	char  * text;
    uint32_t start;
	uint32_t Default;
    uint32_t end;
} BandGenInfo;
extern const BandGenInfo   Band_Info[ COUPLING_MAX];
extern const BandGenInfo   RxBand_Info[ RADIO_MAX];
/*
*	AGC恢复时间参数
*/
typedef struct
{
	char * text;
	//u8 num;
	//u16 min;
	u16 Default;
	//u16 max;
}AGC_Recovery_Time;
extern AGC_Recovery_Time AGC_Time[3];
/*
*	NR DSP降噪参数
*/
typedef struct
{
	char* text;
	u16 min;
	u16 Default;
	u16 max;
	
}DSP_NR_parameter;
extern DSP_NR_parameter Dsp_nr[3];
/*
*	step
*/
#define 	STEP_PRESS_MINUS			10
#define		STEP_PRESS_PLUS				10000
//
#define 	T_STEP_1HZ					1
#define 	T_STEP_10HZ					10
#define 	T_STEP_100HZ				100
#define 	T_STEP_1KHZ					1000
#define 	T_STEP_5KHZ					5000
#define 	T_STEP_9KHZ					9000
#define 	T_STEP_10KHZ				10000
#define 	T_STEP_100KHZ				100000
#define		T_STEP_1MHZ					1000000		// Used for transverter offset adjust

//
typedef enum
{
    T_STEP_1HZ_IDX = 0,
    T_STEP_10HZ_IDX,
    T_STEP_100HZ_IDX,
    T_STEP_1KHZ_IDX,
    //T_STEP_5KHZ_IDX,
   // T_STEP_9KHZ_IDX
    T_STEP_10KHZ_IDX,
    T_STEP_100KHZ_IDX,
    T_STEP_1MHZ_IDX,
	T_STEP_5KHZ_IDX,
	T_STEP_9KHZ_IDX,
    T_STEP_MAX_STEPS
}STEP_NAME;
/*
*	CW WPM
*/
typedef struct
{
	char * cw_wpm;
	uint8_t cw_ms;	
}CW_WPM_management;

extern CW_WPM_management	wpm_tab[46];
//

/*
*	MENU 
*/
#define MENU		 		0	/* 0 菜单自动退出延迟时间 */
#define MENU_POW_CORRE		1	/* 0 菜单自动退出延迟时间 */
#define MENU_IF_1 	 		4	/* 1 第一中频频率 		 */
#define MENU_IF_GAIN 	 	8	/* 2 第二中频频率 		 */
#define MENU_TCXO			12	/* 3 温补晶振频率校准 	 */
#define MENU_SQL			16	/* 4 静噪门限调整 		 */
#define MENU_CW_VOL			17	/* 5 CW侧音音量 			 */
#define MENU_CW_SI			18	/* 6 CW音调调整 			 */
#define MENU_CW_SP			20	/* 7 CW自动键速度 		 */
#define MENU_CW_DELAY		21	/* 8 CW接收延迟时间 		 */
#define	MENU_POW			22	/* 9 输出功率设置   		 */
#define MENU_AGC_START		23	/* 10 AGC起控门限 		 */
#define MENU_AGC_M_A		24	/* 11 AGC M/A转换 	 	*/
#define MENU_BLUETOOTH		26	/* 12 蓝牙开关 		 */
#define MENU_MIC_GAIN		28	/* 13 麦克风增益设置 	 */
#define MENU_NR_STRONG		29	/* 14 AGC检测零点设置 	 */
#define MENU_NR_BUFLEN		30	/* 15 AGC偏置设置 		 */
#define MENU_NR_NUMTAPS		32	/* 16 麦克风增益设置 	 */
#define MENU_NOT_STRONG		33	/* 17 AGC检测零点设置 	 */
#define MENU_NOT_BUFLEN		34	/* 18 AGC偏置设置 		 */
#define MENU_NOT_NUMTAPS	36	/* 19 麦克风增益设置 	 */
#define MENU_RITATT_PTT		37	/* 20 RIT/PTT转换设置 	 */
#define MENU_ALC			38	/* 21 ALC开关 0-OFF 1-ON */
#define MENU_ENC_FREQ		39	/* 22 本机频率编码器调整模式 0：正常倍率，*/
#define MENU_ENC_EXTI		40	/* 23 外接光电编码器调整模式 */
#define MENU_TX_FILTER		41	/* 24 SSB发射带宽设置 */
#define MENU_S_CORRECT		42	/* 25 S表校准 */
//开发菜单地址developers
#define MENU_SPE_DISPALY	70//TX发射频谱显示 0：不显示，1显示
#define MENU_RX_PHASE		71//TX发射IQ平衡
#define MENU_ALC_START		73//TX发射ALC
#define MENU_ALC_MIC		74//TX发射MIC
#define MENU_TX_AMP_80M			76//TX发射IQ平衡
#define MENU_TX_PHASE_80M		78//TX发射IQ平衡
#define MENU_TX_AMP_60M			80//TX发射IQ平衡
#define MENU_TX_PHASE_60M		82//TX发射IQ平衡
#define MENU_TX_AMP_40M			84//TX发射IQ平衡
#define MENU_TX_PHASE_40M		86//TX发射IQ平衡
#define MENU_TX_AMP_30M			88//TX发射IQ平衡
#define MENU_TX_PHASE_30M		90//TX发射IQ平衡
#define MENU_TX_AMP_20M			92//TX发射IQ平衡
#define MENU_TX_PHASE_20M		94//TX发射IQ平衡
#define MENU_TX_AMP_17M			96//TX发射IQ平衡
#define MENU_TX_PHASE_17M		98//TX发射IQ平衡
#define MENU_TX_AMP_15M			100//TX发射IQ平衡
#define MENU_TX_PHASE_15M		102//TX发射IQ平衡
#define MENU_TX_AMP_12M			104//TX发射IQ平衡
#define MENU_TX_PHASE_12M		106//TX发射IQ平衡
#define MENU_TX_AMP_10M			108//TX发射IQ平衡
#define MENU_TX_PHASE_10M		110//TX发射IQ平衡
#define MENU_TX_AMP_6M			112//TX发射IQ平衡
#define MENU_TX_PHASE_6M		114//TX发射IQ平衡
//#define MENU_VOLWINDOW_DELAY		38	/* 20 ATT/PTT转换设置 	 */
//
#define ADDR_ENC_DIR		130 /* 编码器方向 */
#define ADDR_TX_BAND_NO		131 /* 编码器方向 */
//
#define MENU_END			8
#define MENU_HIDD_END		28//开发者菜单数量
//
//
#define	MENU_ADDR(x)		(x*4)
#define MENU_OFF				0
#define MENU_ON					1
/*
*
*/
#define	ADDR_MODE_REG  			(120)
#define ADDR_VOL  				(121)
//#define ADDR_CW_POW_GAIN(x)  	(122+x*2)
//
// Frequency information
//
#define	ADDR_A					290
#define	ADDR_B					291
#define	ADDR_OFFSET				300
#define	ADDR_BYTE_NUM			7
#define	FREQ_ADDR_BYTE			4

#define	FREQ_ADDR 	 			0
#define	MODE_ADDR 	 			4
#define	BAND_ADDR 				5	
#define	FILTERSELECTED_ID_ADDR 	6	 
 //
#define	CH_MIN 					0
#define	CH_MAX 					99
//
#define	FREQ_CH(x)  				(ADDR_OFFSET +  FREQ_ADDR + ADDR_BYTE_NUM *x)
#define	MODE_CH(x) 					(ADDR_OFFSET + 	MODE_ADDR + ADDR_BYTE_NUM *x)
#define	BAND_CH(x)					(ADDR_OFFSET + 	BAND_ADDR + ADDR_BYTE_NUM *x)
#define	FILTERSELECTED_CH(x)	(ADDR_OFFSET + 	FILTERSELECTED_ID_ADDR + ADDR_BYTE_NUM *x)

#define ADDR_CH_NUM(x)  		(ADDR_OFFSET - 18 + x*1)

#define	ADDR_RT_OFFSET			0
#define	ADDR_RX_OFFSET			(100 * ADDR_BYTE_NUM)
/* 
*	音量多功能显示窗延迟关闭时间
*/
#define VOL_WINDOW_DELAY		60
enum
 {
	VFO_A = 0,
	VFO_B , 
	CHA_NUM ,
	CHB_NUM,
	RXVFO_A,
	RXVFO_B , 
	RXCHA_NUM ,
	RXCHB_NUM 
 };
typedef struct
{
	uint32_t Freq;
	int8_t Mode:8;
	int8_t Band_id:8;
	int8_t RxBand_id:8;
	int8_t FilterSelected_id:8;
}At24c_addr_GenInfo;	
extern __IO At24c_addr_GenInfo	vfo[2];
/*
*	接收时波段相位调整记忆
*/
//#define ADDR_RT_BAND_FIL_M	1890//波段频率存储地址
#define ADDR_RX_BAND_PHASE_M	1700//波段频率存储地址
#define MENU_RX_AMP_80M			1700//RX接收IQ幅度平衡
#define MENU_RX_PHASE_80M		1702//RX接收IQ相位校准
#define MENU_RX_AMP_60M			1703//RX接收IQ幅度平衡
#define MENU_RX_PHASE_60M		1705//RX接收IQ相位校准
#define MENU_RX_AMP_40M			1706//RX接收IQ幅度平衡
#define MENU_RX_PHASE_40M		1708//RX接收IQ相位校准
#define MENU_RX_AMP_30M			1709//RX接收IQ幅度平衡
#define MENU_RX_PHASE_30M		1711//RX接收IQ相位校准
#define MENU_RX_AMP_20M			1712//RX接收IQ幅度平衡
#define MENU_RX_PHASE_20M		1714//RX接收IQ相位校准
#define MENU_RX_AMP_17M			1715//RX接收IQ幅度平衡
#define MENU_RX_PHASE_17M		1717//RX接收IQ相位校准
#define MENU_RX_AMP_15M			1718//RX接收IQ幅度平衡
#define MENU_RX_PHASE_15M		1720//RX接收IQ相位校准
#define MENU_RX_AMP_12M			1721//RX接收IQ幅度平衡
#define MENU_RX_PHASE_12M		1723//RX接收IQ相位校准
#define MENU_RX_AMP_10M			1724//RX接收IQ幅度平衡
#define MENU_RX_PHASE_10M		1726//RX接收IQ相位校准
#define MENU_RX_AMP_6M			1727//RX接收IQ幅度平衡
#define MENU_RX_PHASE_6M		1729//RX接收IQ相位校准
//
/*
*	RX幅度修正
*/
//#define MENU_RX_AMP_JUSET		1730
/*
*	
*/
#define MENU_TX_AMP_MM		1730
/*
*	POWER_MAX
*/
#define	ADDRPOWER_GAIN		1800 
typedef struct
{
	u16 min;
	u16 Default;
	u16 max;
}Power_addr_GenInfo;	
extern  Power_addr_GenInfo	Power_max[COUPLING_MAX];
/*
*	POW_MM
*/
#define	ADDRPOW_MM		1820 
typedef struct
{
	u16 min;
	u16 Default;
	u16 max;
}Power_addr_MMInfo;	
extern  Power_addr_MMInfo	Power_set[COUPLING_MAX];
//
/*
*	波段模式记忆
*/
#define ADDR_RT_BAND_MODE_M	1830//波段频率存储地址
#define ADDR_RX_BAND_MODE_M	1840//波段频率存储地址
/*
*	波段滤波器记忆
*/
#define ADDR_RT_BAND_FIL_M	1860//波段频率存储地址
#define ADDR_RX_BAND_FIL_M	1870//波段频率存储地址
/*
*	波段频率记忆
*/
#define ADDR_RT_BAND_FREQ_M	1900//波段频率存储地址
#define ADDR_RX_BAND_FREQ_M	1950//波段频率存储地址
//#define ADDR_BAND_FREQ_M	1850//波段频率存储地址
/*
*	波段IF增益
*/
#define ADDR_IF_GAIN	1970//波段频率存储地址
#define ADDR_IF_RXGAIN	1985//波段频率存储地址
/*
*   滤波器关机存储
*/
#define ADDR_FIL_CW	    2000//CW滤波器存储地址
#define ADDR_FIL_LSB	2001//CW滤波器存储地址
#define ADDR_FIL_USB	2002//CW滤波器存储地址
#define ADDR_FIL_AM	    2003//CW滤波器存储地址
#define ADDR_FIL_FM	    2004//CW滤波器存储地址

void PowerControl_off(void);
void key_ampset(void);
void Key_Management_Set(void);
void key_Mode_Control(void);
void KeyCode_lock_settings(void);
void Key_Band_Control(void);
void key_Filter_Control(u8 idx);
void key_Step_Control(u8 step_idx);
void Hp_set(void);
void pow_gain_set(void);
void Data_init(void);
void Key_AGC_RecoveryTime_set(void);
void key_ATT_Control(void);
void key_bluetooth_control(void);
void key_voltage_Display(void);
void Pow_max_set(void);
void RXandTX_Control_Management(void);
void Menus_Set(void);
void key_Spectrum_Control(void);
void Enc0_UseManagement(void);
void Enc2_UseManagement(void);
void key_DSP_NoiseProcessing(void);
#endif

