#include "Audio_driver.h"
#include "dsp_filter.h"
#include "timer.h" 
#include "arm_math.h" 
#include "key.h" 
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "gpio_set.h"
#include "KEY_Control.h"
#include "lcd.h"
//#include "arm_const_structs.h"
#include "ui_Driver.h"
#include "softdds.h"
#include "wm8978.h"
#include "sai.h"
#include "usart.h"
#include "Si5351AI2C.h"
/*
**************************************************************
	本程序仅供学习参考，未经作者许可，不得用于其它任何用途；
	
	名称：Audio_drtver.c
	用途：FX-4C;
	功能：音频驱动；
	版本：V1.0;
	作者：张玉亮（BG2IXD）;
	时间：2020/9/19；
	QQ:  1413894726
**************************************************************
*/
/* 版本定义 */
#define FX_4C_V_1_0

__attribute__((section (".RAM_D1"))) u16 ADC1_Buff0[SAMPLE_SIZE];
__attribute__((section (".RAM_D1"))) u16 ADC1_Buff1[SAMPLE_SIZE];
__attribute__((section (".RAM_D1"))) u32 ADC_Buff0[SAMPLE_SIZE];
__attribute__((section (".RAM_D1"))) u32 ADC_Buff1[SAMPLE_SIZE];	
__attribute__((section (".RAM_D1"))) u32 DAC_Buff0[SAMPLE_SIZE];
__attribute__((section (".RAM_D1"))) u32 DAC_Buff1[SAMPLE_SIZE];
__attribute__((section (".RAM_D1"))) u16 DAC2_Buff0[SAMPLE_SIZE/2];
__attribute__((section (".RAM_D1"))) u16 DAC2_Buff1[SAMPLE_SIZE/2];
//
float32_t	lms1_nr_delay[LMS_NR_DELAYBUF_SIZE_MAX+BUFF_LEN];
//
float32_t errsig1[64+10];
float32_t errsig2[64+10];
float32_t result[64+10];
//
// LMS Filters for RX
__IO Dspnr_typedef nr;

arm_lms_norm_instance_f32	lms1Norm_instance;
arm_lms_instance_f32	lms1_instance;
float32_t	lms1StateF32[LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN];
float32_t	lms1NormCoeff_f32[LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN];
//
arm_lms_norm_instance_f32	lms2Norm_instance;
arm_lms_instance_f32	lms2_instance;
float32_t	lms2StateF32[DSP_NOTCH_NUMTAPS_MAX + BUFF_LEN];
float32_t	lms2NormCoeff_f32[DSP_NOTCH_NUMTAPS_MAX + BUFF_LEN];
//
float32_t	lms2_nr_delay[LMS_NOTCH_DELAYBUF_SIZE_MAX + BUFF_LEN];
//
/* IIR音频滤波器 */
arm_biquad_casd_df1_inst_f32 IIR_AF;
float32_t AF_State[IIR_NUM_TAPS]; /* 状态缓存，大小numStages*4 */
float32_t  AF_Values;
/* IIR TX IF滤波器 */
arm_biquad_casd_df1_inst_f32 IIR_IF_I;
arm_biquad_casd_df1_inst_f32 IIR_IF_Q;
float32_t IF_State_I[IIR_NUM_TAPS]; /* 状态缓存，大小numStages*4 */
float32_t IF_State_Q[IIR_NUM_TAPS]; /* 状态缓存，大小numStages*4 */
float32_t  IF_Values;
/* IIR TX滤波器 */
arm_biquad_casd_df1_inst_f32 IIR_TX_I;
arm_biquad_casd_df1_inst_f32 IIR_TX_Q;
float32_t TX_State_I[IIR_NUM_TAPS]; /* 状态缓存，大小numStages*4 */
float32_t TX_State_Q[IIR_NUM_TAPS]; /* 状态缓存，大小numStages*4 */
float32_t  TX_Values;
/*	Riss带通 */
arm_biquad_casd_df1_inst_f32 RISS;
float32_t Riss_State[IIR_NUM_TAPS]; /* 状态缓存，大小numStages*4 */
float32_t  Riss_Values;
/* IIR单边带滤波器 */
arm_biquad_casd_df1_inst_f32 IIR_LPF_I;
arm_biquad_casd_df1_inst_f32 IIR_LPF_Q;
float32_t IIRStateF32_I[IIR_NUM_TAPS]; /* 同相 I 通道状态缓存，大小numStages*4 */
float32_t IIRStateF32_Q[IIR_NUM_TAPS]; /* 同相 Q 通道状态缓存，大小numStages*4 */
float32_t  IIR_LP_Values;			   /* IIR滤波器缩放系数 */

/* RX希尔伯特变换滤波器*/
arm_fir_instance_f32 Hilbert_I;
arm_fir_instance_f32 Hilbert_Q;
float32_t Hilbert_State_I[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];
float32_t Hilbert_State_Q[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];

/* 抽取FIR滤波器 */
arm_fir_decimate_instance_f32 DECIMATE_I;//抽取滤波器 I
arm_fir_decimate_instance_f32 DECIMATE_Q;//抽取滤波器 Q
float32_t DecimState_I[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR抽取滤波器状态缓存I */
float32_t DecimState_Q[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR抽取滤波器状态缓存Q */

/* FIR内插滤波器 */
arm_fir_interpolate_instance_f32 INTERPOLATE_I;
arm_fir_interpolate_instance_f32 INTERPOLATE_Q;
float32_t InterpState_I[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR内插滤波器状态缓存I */
float32_t InterpState_Q[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];/* FIR内插滤波器状态缓存Q */

/* FIR边带滤波器 */
arm_fir_instance_f32 FIR_LPF_I;
arm_fir_instance_f32 FIR_LPF_Q;
float32_t FIR_State_I[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];
float32_t FIR_State_Q[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];

/* FIR带通滤波器 */
arm_fir_instance_f32 FIR_BPF;
float32_t FIR_State_BPF[FIR_NUM_TAPS+FIR_BLOCK_SIZE-1];
/**/
SpectrumProcessing_typedef	spe;

arm_rfft_fast_instance_f32  Spe_FFT;
arm_rfft_fast_instance_f32  FFT_LSB;
arm_rfft_fast_instance_f32  FFT_USB;
//arm_rfft_instance_f32  FFT;
float32_t fft_buf[FFT_LENGTH*2];
float32_t spe_buf[FFT_LENGTH*2];
/**/

 Audio_Digital_Signal	 ads;
 CW_Auto_key_management	cw_Auto_key;	
//extern 	KeyManagement_TypeDef		ks;
//extern 	Storage_Data 				sd;
/*
******************************************************
*	中值滤波器
******************************************************
*/
#define MidFilterOrder  16     				/* 滤波阶数 */
#define ZeroSize        MidFilterOrder

float32_t DstDate[SAMPLE_SIZE/2];     /* 滤波后数据 */

static float32_t SortData[MidFilterOrder];  /* 滤波排序 */ 
static float32_t TempDate[ZeroSize + SAMPLE_SIZE +ZeroSize] = {0}; /* 滤波阶段用到的临时变量 */
//extern  u16 Rx_amp_adjust[12];
extern u16  Rx_amp_user[10];
#if f5bud
/*
*********************************************************************************************************
*	函 数 名: MidFilterBlock
*	功能说明: 中值滤波器，对一段数据的中值滤波。
*	形    参: pSrc 源数据地址。
*             pDst 滤波后数据地址。
*             blockSize 数据个数，至少为2。
*             order 至少2阶。
*	返 回 值: 无
*********************************************************************************************************
*/
void MidFilterBlock(float32_t *pSrc, float32_t *pDst, uint32_t blockSize, uint32_t order)
{
	arm_sort_instance_f32 S;
	uint32_t N, i;
	
	
	S.dir = ARM_SORT_ASCENDING;
	S.alg = ARM_SORT_QUICK;
	
	
	N = order / 2;
	
	/* 数据幅值给临时缓冲 */
	for(i =0; i < blockSize; i++)
	{
		TempDate[i + ZeroSize] = pSrc[i];
	}
	
	
	/* 求每个数据点的中值 */
	for(i =0; i < blockSize; i++)
	{
		/* 排序 */
		arm_sort_f32(&S, &TempDate[i + ZeroSize - N], &SortData[0], order);
		
		/* 奇数 */
		if(N)
		{
			pDst[i] = SortData[N];
		}
		/* 偶数 */
		else
		{
			pDst[i] = (SortData[N] + SortData[N-1])/2;
		}
	}
}
void MidFilterRT(float32_t *pSrc, float32_t *pDst, uint8_t ucFlag, uint32_t order)
{
	arm_sort_instance_f32 S;
	uint32_t N, i;
	
	static uint32_t CountFlag = 0;
	
	
	S.dir = ARM_SORT_ASCENDING;
	S.alg = ARM_SORT_QUICK;
	
	N = order / 2;
	
	/* 首次滤波先清零 */
	if(ucFlag == 1)
	{
		CountFlag = 0;
	}
	
	/* 填充数据 */
	if(CountFlag < order)
	{
		TempDate[CountFlag] = pSrc[0];
		CountFlag++;
	}
	else
	{
		for(i =0; i < order - 1; i++)
		{
			TempDate[i] = TempDate[1 + i];  
		}
		TempDate[order - 1] = pSrc[0];
	}
	
	/* 排序 */
	arm_sort_f32(&S, &TempDate[0], &SortData[0], order);
	
	/* 奇数 */
	if(N)
	{
		pDst[0] = SortData[N];
	}
	/* 偶数 */
	else
	{
		pDst[0] = (SortData[N] + SortData[N-1])/2;
	}
}
#endif
/*****************************************************/
/*
*	音频处理系统的ADC DAC TIM初始化 
*/
void Audio_hardware_init(void)
{
	Audio_Filter_init();
	WM8978_Init();
	
	SAIA_Init(0,1,6);			//SAI1 Block A,从发送,24位数据
	SAIB_Init(3,1,6);			//SAI1 Block B主模式接收,24位
	SAIA_SampleRate_Set(SAMPLING_RETE);//设置采样率 
	
	SAIA_TX_DMA_Init(DAC_Buff0,DAC_Buff1,SAMPLE_SIZE,2);	//配置TX DMA,32位 
	SAIB_RX_DMA_Init(ADC_Buff0,ADC_Buff1,SAMPLE_SIZE,2);//配置RX DMA
  
	SAI_Rec_Start(); 			//开始SAI数据接收(从机)
	SAI_Play_Start();			//开始SAI数据发送(主机)
	
	ADC1_DMA_Config(DMA1_Stream0,9);
	DAC1_DMA_Config(DMA1_Stream1, 67);
	Adc1_Init();
	
	
	Dac_Init();
	TIM6_Int_Init(TIM_ARR ,0);
	DMA1_Stream0->CR|=1<<0;
	DMA1_Stream1->CR|=1<<0;
	ADC1->CR|=1<<0;
	ADC1->CR|=1<<2;       		//启动规则转换通道 
	
	TIM6->CR1|=1<<0;
}
/*
*	滤波器初始化 
*/
void Audio_Filter_init()
{
	u16 i;
	static float mu_calc;
	arm_rfft_fast_init_f32(&Spe_FFT,FFT_LENGTH);//初始化scfft结构体，le[i]=设定FFT相关参数
	arm_rfft_fast_init_f32(&FFT_LSB ,FFT_LENGTH);//初始化scfft结构体，设定FFT相关参数
	arm_rfft_fast_init_f32(&FFT_USB ,FFT_LENGTH);//初始化scfft结构体，设定FFT相关参数
	
	//softdds_setfreq(800, SAMPLING_RETE, 1);
	/* IIR Filter init */
	/* AF音频低通滤波 */
	for(i=0;i<IIR_NUM_TAPS;i++)
	{
		AF_State[i]=0;
		Riss_State[i]=0;
		TX_State_I[i]=0;
		TX_State_Q[i]=0;
		IF_State_I[i]=0;
		IF_State_Q[i]=0;
	}
	IIR_AF.pState=(float32_t *)& AF_State;
	IIR_AF.numStages = IIR_dfI_AF.numStages;
	IIR_AF.pCoeffs = IIR_dfI_AF.pCoeffs;
	/*	TX SSB滤波器*/
	IIR_TX_I.pState=(float32_t *)& TX_State_I;
	IIR_TX_I.numStages = IIR_TX_3k_BPF.numStages;
	IIR_TX_I.pCoeffs = IIR_TX_3k_BPF.pCoeffs;
	IIR_TX_Q.pState=(float32_t *)& TX_State_Q;
	IIR_TX_Q.numStages = IIR_TX_3k_BPF.numStages;
	IIR_TX_Q.pCoeffs = IIR_TX_3k_BPF.pCoeffs;
	/*	TX IF滤波器*/
	IIR_IF_I.pState=(float32_t *)& IF_State_I;
	IIR_IF_I.numStages = IIR_30k_LPF.numStages;
	IIR_IF_I.pCoeffs = IIR_30k_LPF.pCoeffs;
	IIR_IF_Q.pState=(float32_t *)& IF_State_Q;
	IIR_IF_Q.numStages = IIR_30k_LPF.numStages;
	IIR_IF_Q.pCoeffs = IIR_30k_LPF.pCoeffs;
	//
	RISS.pState=(float32_t *)& Riss_State;
	RISS.numStages = RISS_BPF.numStages;
	RISS.pCoeffs = RISS_BPF.pCoeffs;
	/* 带宽滤波器 */	
	IIR_LPF_I.numStages = FilterPathInfo[8].LP_instance->numStages;
	IIR_LPF_Q.numStages = FilterPathInfo[8].LP_instance->numStages;
	IIR_LPF_I.pCoeffs = FilterPathInfo[8].LP_instance->pCoeffs;
	IIR_LPF_Q.pCoeffs = FilterPathInfo[8].LP_instance->pCoeffs;
	IIR_LP_Values = FilterPathInfo[8].Lpf_ScaleValues;
	for(i=0;i<IIR_NUM_TAPS;i++)
	{
		IIRStateF32_I[i]=0;
		IIRStateF32_Q[i]=0;
	}
	IIR_LPF_I.pState=(float32_t *)&IIRStateF32_I;
	IIR_LPF_Q.pState=(float32_t *)&IIRStateF32_Q;
	
	/* RX希尔伯特变换*/
	Hilbert_I.numTaps = Hilbert_Transform_I.numTaps ;
	Hilbert_Q.numTaps = Hilbert_Transform_Q.numTaps ;
	Hilbert_I.pCoeffs = Hilbert_Transform_I.pCoeffs;
	Hilbert_Q.pCoeffs = Hilbert_Transform_Q.pCoeffs;
	Hilbert_I.pState = (float32_t *)&Hilbert_State_I;
	Hilbert_Q.pState = (float32_t *)&Hilbert_State_Q;
	
	/*抽取和内插滤波器 */
	DECIMATE_I.numTaps = FilterPathInfo[AM_6K].Decimate_LP->numTaps;
	DECIMATE_Q.numTaps = FilterPathInfo[AM_6K].Decimate_LP->numTaps;
	INTERPOLATE_I.phaseLength = FilterPathInfo[AM_6K].Interpolate_LP->phaseLength/DECIMATION_RATE;
	INTERPOLATE_Q.phaseLength = FilterPathInfo[AM_6K].Interpolate_LP->phaseLength/DECIMATION_RATE;
	
	DECIMATE_I.pCoeffs = FilterPathInfo[AM_6K].Decimate_LP->pCoeffs;
	DECIMATE_Q.pCoeffs = FilterPathInfo[AM_6K].Decimate_LP->pCoeffs;
	INTERPOLATE_I.pCoeffs = FilterPathInfo[AM_6K].Interpolate_LP->pCoeffs;
	INTERPOLATE_Q.pCoeffs = FilterPathInfo[AM_6K].Interpolate_LP->pCoeffs;
	
	DECIMATE_I.M = DECIMATION_RATE;
	DECIMATE_Q.M = DECIMATION_RATE;
	
	INTERPOLATE_I.L = DECIMATION_RATE;
	INTERPOLATE_Q.L = DECIMATION_RATE;
	
	DECIMATE_I.pState = (float32_t *)&DecimState_I;
	DECIMATE_Q.pState = (float32_t *)&DecimState_Q;
	
	INTERPOLATE_I.pState = (float32_t *)&InterpState_I;
	INTERPOLATE_Q.pState = (float32_t *)&InterpState_Q;
	
//	FIR_LPF_I.numTaps = FIR_Mrii_LPF.numTaps;
//	FIR_LPF_Q.numTaps = FIR_Mrii_LPF.numTaps;
//	FIR_LPF_I.pCoeffs = FIR_Mrii_LPF.pCoeffs;
//	FIR_LPF_Q.pCoeffs = FIR_Mrii_LPF.pCoeffs;
	FIR_LPF_I.pState = (float32_t *)&FIR_State_I;
	FIR_LPF_Q.pState = (float32_t *)&FIR_State_Q;
	
	for(i=0;i<FIR_NUM_TAPS+FIR_BLOCK_SIZE-1;i++)
	{
		DecimState_I[i]=0;
		DecimState_Q[i]=0;
		InterpState_I[i]=0;
		InterpState_Q[i]=0;
		Hilbert_State_I[i]=0;
		Hilbert_State_Q[i]=0;
	}
	
//	FIR_BPF.numTaps = FIR_12k_BPF.numTaps;
//	FIR_BPF.pCoeffs = FIR_12k_BPF.pCoeffs ;
//	FIR_BPF.pState = (float32_t *)&FIR_State_BPF[0];
//	for(i=0;i<FIR_NUM_TAPS+FIR_BLOCK_SIZE-1;i++)
//	{
//		FIR_State_BPF[i] =0;
//	}
	static uint32_t	calc_taps;
    //
	nr.dsp_nr_strength=15;
	nr.dsp_nr_delaybuf_len=110;
	nr.dsp_nr_numtaps=48;
    if((nr.dsp_nr_numtaps < DSP_NR_NUMTAPS_MIN) || (nr.dsp_nr_numtaps > DSP_NR_NUMTAPS_MAX))
        calc_taps = DSP_NR_NUMTAPS_DEFAULT;
    else
        calc_taps = (uint16_t)nr.dsp_nr_numtaps;
    
    lms1Norm_instance.numTaps = calc_taps;
    lms1Norm_instance.pCoeffs = lms1NormCoeff_f32;
    lms1Norm_instance.pState = lms1StateF32;
    //
    
    mu_calc = (float)nr.dsp_nr_strength;		
    mu_calc /= 2;	
    mu_calc += 2;	
    mu_calc /= 10;	
    mu_calc = powf(10,mu_calc);		
    mu_calc = 1/mu_calc;			
    lms1Norm_instance.mu = mu_calc;

    for(i = 0; i < LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN; i++)	 		
    {
        lms1_nr_delay[i] = 0;
    }
    //
    for(i = 0; i < DSP_NR_NUMTAPS_MAX + BUFF_LEN; i++)	 		
    {
        lms1StateF32[i] = 0;			
        if(nr.reset_dsp_nr)	 			
        {
            lms1NormCoeff_f32[i] = 0;		
        }
    }
    
    arm_lms_norm_init_f32(&lms1Norm_instance, calc_taps, &lms1NormCoeff_f32[0], &lms1StateF32[0], (float32_t)mu_calc, nr.dsp_nr_numtaps);
    //
	
    if((nr.dsp_nr_delaybuf_len > DSP_NR_BUFLEN_MAX/*48*/) || (nr.dsp_nr_delaybuf_len < DSP_NR_BUFLEN_MIN/*512*/))
        nr.dsp_nr_delaybuf_len = DSP_NR_BUFLEN_DEFAULT;// DSP_NR_BUFLEN_DEFAULT=192
}
/*
*********************************************************
*
*********************************************************
*/
static void audio_lms_noise_reduction(int16_t psize)
{
    static uint32_t		lms1_inbuf = 0, lms1_outbuf = 0;

    arm_copy_f32((float32_t *)ads.a_buffer, (float32_t *)&lms1_nr_delay[lms1_inbuf], psize);	// put new data into the delay buffer
	
	arm_lms_norm_f32(&lms1Norm_instance, (float32_t *)ads.a_buffer, (float32_t *)&lms1_nr_delay[lms1_outbuf], (float32_t *)ads.a_buffer, (float32_t *)errsig1 ,psize);	// do noise reduction
	
    if((((u32)fabs(ads.a_buffer[0])) * DSP_ZERO_DET_MULT_FACTOR) < DSP_OUTPUT_MINVAL)	// is DSP level too low?
																						// DSP_ZERO_DET_MULT_FACTOR = 10000000;DSP_OUTPUT_MINVAL = 1
    {
        if(ads.dsp_zero_count < MAX_DSP_ZERO_COUNT)
        {
            ads.dsp_zero_count++;
        }
    }
    else
        ads.dsp_zero_count = 0;
  //
    ads.dsp_nr_sample = ads.a_buffer[0];		
    //
    lms1_inbuf += psize;	
    lms1_outbuf = lms1_inbuf + psize;	
    lms1_inbuf %= nr.dsp_nr_delaybuf_len;
    lms1_outbuf %= nr.dsp_nr_delaybuf_len;
}
/*
*	函数名称：Gain_adjustment_control（）
*	函数功能：用于ADC  ALC的增益控制
*/
u16 Power_Gain_factor[] =
{
	24,//80m
	15,//60m
	11,//40m
	10,//30m
	10,//20m
	10,//17m
	10,//15m
	10,//12m
	20,//10m
	10
};
/*
 *	DMA2->Stream1中断程序
 *	使用DMA采集ADC3->DR数据;
 *	采用双缓存以防数据丢失；
 */
u32 m;
void DMA2_Stream5_IRQHandler (void) 
{ 
	u8 temp;
	//u8 dsp_ptt;
	//float32_t pow_v;
	//static u32 freq_lo;
	static u8 lock;
	static float32_t vol;
   // static u16 dac_offset_level;
	static float32_t ssb_mic_gain;
	static float32_t mic_gain;
	static float32_t agc_gain=1.0f;
	static float32_t agc2_gain=1.0f,agc0;
			float32_t calc;
			float32_t varI_0,varI_1;//calcI_amp;
			float32_t varQ_0,varQ_1;//calcQ_amp;
	static  float32_t mag_gain=1.0f;
	//static  float32_t alc_gain=50.0f;
	static	float32_t ssb_alc_gain;
	static	float32_t am_alc_gain;
	static	float32_t cw_alc_gain=1.0f;
	static	float32_t fm_alc_gain;
	static  float32_t fm_gain;
	static float32_t riss_gain;
	static float32_t riss_buf[SAMPLE_SIZE/2];
	float32_t dp_I, dp_Q, ratio;
	  u16 i;
	static u16 agc_time=1,agc2_time;
	static uint32_t A;
	//static uint32_t B;
	//static uint32_t C;
	 u16 psize;
	
	//static   float32_t bfo_i;
	//static   float32_t bfo_q; 
	//static   float32_t LO2_I;
	//static   float32_t LO2_Q;
	   float32_t add_I;/* 累加缓存 */
	   float32_t add_Q;/* 累加缓存 */
	static   float32_t Data_I1[SAMPLE_SIZE/2];/* DSP缓存 */
	static   float32_t Data_Q1[SAMPLE_SIZE/2];/* DSP缓存 */
	static   float32_t Data_I[SAMPLE_SIZE/2];/* DSP缓存 */
	static   float32_t Data_Q[SAMPLE_SIZE/2];/* DSP缓存 */
	static   float32_t FM_I[2]; /* BP输入数组 */
	static   float32_t FM_Q[2];/* BP滤波后的输出 */
	static   float32_t Data_FM_I[SAMPLE_SIZE/2];
	//static   float32_t Data_FM_Q[SAMPLE_SIZE/2];
	//static   float32_t Dac2_out_I[SAMPLE_SIZE];/* DSP缓存 */
	//static   float32_t Dac2_out_Q[SAMPLE_SIZE];/* DSP缓存 */
	//static   float32_t AGC_out[SAMPLE_SIZE];/* DSP缓存 */
	
	if(DMA2->HISR&(1<<11))	//等待DMA2_Steam5传输完成
	{
		DMA2->HIFCR|=1<<11;	//清除DMA2_Steam5传输完成标志		
		/*
		*	提取I/Q采样数据，在DMA采用双缓冲；
		*/
		if(DMA2_Stream5->CR&1<<19)
		{
			for(i=0;i<SAMPLE_SIZE/2;i++)
			{
				Data_I1[i]= (float32_t)ADC_Buff0[i*2];	// I 
				Data_Q1[i]= (float32_t)ADC_Buff0[i*2+1];// Q
			}
		}
		else
		{
			for(i=0;i<SAMPLE_SIZE/2;i++)
			{
				Data_I1[i]= (float32_t)ADC_Buff1[i*2];// * LO1_I;//LO1_I 数字1变频同相 I
				Data_Q1[i]= (float32_t)ADC_Buff1[i*2+1];// * LO1_Q;//LO1_Q 数字1变频移相 Q
			}
		}
		psize=SAMPLE_SIZE/(ads.Rate*2);//抽取后的取样数量
		
		/*
		** RX接收 
		*/
		if(ks.cat_ptt &ks.rit_tx)PTT_IN();
		if(PTT_RT & ks.cat_ptt &ks.rit_tx& CW_K_DI & CW_K_DA)//((ks.CW_keySW ==0 &&CW_K_DI == 1 && CW_K_DA ==1) || (ks.CW_keySW ==1 && CW_K_DI ==1)))
		{
			
			/* 接收延迟 */
			if(ads.tx_delay>0)
			{
				for(i=0;i<SAMPLE_SIZE/2 ;i++)
				{	
					ads.out_buff [i*2] = 0;
					ads.out_buff [i*2+1] = 0;
				}
				ads.tx_delay -=1;ads.rx_delay =200;vol = 1; 
				//ads.spk_vol =0;ads.hp_vol =0;
				if(vfo[VFO_A].Mode >= DEMOD_LSB&&ads.tx_delay<100)
				{
					MIC_POWER(1);
					TR_CONTROL(CONTROL_RX);/* 转换到接收状态 */

					agc2_gain =1.0f;
				}
				if(vfo[VFO_A].Mode < DEMOD_LSB&&ads.tx_delay<2)
				{
					ads.spk_vol =20;ads.hp_vol =20;
					agc2_gain =0.025f;
					agc_gain =3000;
				}					
			}
			
			if(ads.tx_delay <1)
			{
				if(vfo[VFO_A].Mode < DEMOD_LSB)
				{
					TR_CONTROL(CONTROL_RX);/* 转换到接收状态 */
					if(ks.Spk_key)ads.spk_vol =0;
					else {ads.spk_vol++;if(ads.spk_vol>63)ads.spk_vol=63;}
					ads.hp_vol++;if(ads.hp_vol>55)ads.hp_vol=55;
				}
				else
				if(vfo[VFO_A].Mode == DEMOD_USB_DIG || vfo[VFO_A].Mode == DEMOD_LSB_DIG )
				{
					ads.spk_vol =0;
					ads.hp_vol =55;
				}
				else 
				{
					if(ks.Spk_key)ads.spk_vol =0;	
					else ads.spk_vol =63;
					ads.hp_vol =55;
				}
				ads.tx_delay =0;
				//ads.pow_gain_temp =0;
				if(vfo[VFO_A].Mode < DEMOD_LSB)TR_CONTROL(CONTROL_RX);/* 转换到接收状态 */
                //if(rit_0!=sd.rit)
				//{
				//	vol =0;
				//	rit_0=sd.rit;
				//}
				//else
				//{
					if(vol < sd.Vol*sd.Vol)
					{
						calc = sd.Vol*sd.Vol -vol;
						calc /=sd.Vol*sd.Vol;
						vol +=vol*calc*0.1f+1.0f;	
					}
					if(vol >= sd.Vol*sd.Vol)
						vol = sd.Vol*sd.Vol;//*(1600/LCD_Pow(2,ads.spk_vol/6));
				//}
				/*
				*	相位平衡
				*/
				arm_scale_f32(Data_Q1,ads.rx_phase[ads.rx_amp_band ], ads.b_buffer,SAMPLE_SIZE/2);//取一点点I通道信号
				arm_add_f32(Data_I1, ads.b_buffer, Data_I1, SAMPLE_SIZE/2);// 取一点点I通道信号与Q相加
				/*
				*	幅度平衡
				*/
				if(sd.Rx_amp_ma==0 )
				{
					arm_scale_f32(Data_Q1, ads.rx_amp[ads.rx_amp_band], Data_Q1,SAMPLE_SIZE/2);
					//arm_scale_f32(Data_Q1, ads.amp_adjust[ads.amp_adjust_num], Data_Q1,SAMPLE_SIZE/2);
				}
				else
				{
					
					//arm_scale_f32(Data_I, mag_gain, Data_I,SAMPLE_SIZE/2);
					arm_scale_f32(Data_Q1, mag_gain, Data_Q1,SAMPLE_SIZE/2);
					//arm_max_f32(Data_I1, SAMPLE_SIZE/2, (float32_t *)&add_I, 0);/* AGC样品，提取最大值 */
					//arm_max_f32(Data_Q1, SAMPLE_SIZE/2, (float32_t *)&add_Q, 0);/* AGC样品，提取最大值 */
					arm_mean_f32(Data_I1,SAMPLE_SIZE/2, &add_I);
					arm_mean_f32(Data_Q1,SAMPLE_SIZE/2, &add_Q);
					calc = add_Q/add_I;//I Q 幅度比例计算
					//add_I = 0;//清零
					//add_Q = 0;//清
					if(calc>1.0f)
					{
						//mag_gainI +=calc*0.0001f;
						//if(mag_gainI>5.0f)mag_gainI =1.0f;
						mag_gain -=calc*0.001f;
						if(mag_gain<0.2f)mag_gain =1.0f;
					}
					if(calc<1.0f)
					{
						//mag_gainI -=calc*0.0001f;
						//if(mag_gainI<0.2f)mag_gainI =1.0f;
						mag_gain +=calc*0.00001f;
						if(mag_gain>5.0f)mag_gain =1.0f;
					}
				}
				
				/*
				*	一次AGC检测，采用峰值检测；
				*/
				arm_max_f32(Data_I1, SAMPLE_SIZE/2, (float32_t *)&varI_0, 0);/* AGC样品，提取最大值 */
				arm_min_f32(Data_I1, SAMPLE_SIZE/2, (float32_t *)&varI_1, 0);/* AGC样品，提取最大值 */
				arm_max_f32(Data_Q1, SAMPLE_SIZE/2, (float32_t *)&varQ_0, 0);/* AGC样品，提取最大值 */
				arm_min_f32(Data_Q1, SAMPLE_SIZE/2, (float32_t *)&varQ_1, 0);/* AGC样品，提取最大值 */
				
				//calc=(varI_0 - varI_1);
				calc=(varQ_0+varI_0 - varQ_1-varI_1);//*0.5f;
				if(calc<0)calc*=-1;
				/*
				*	一次AGC控制
				*	采用正向AGC控制
				*/
				if((u32)calc > sd.agc_Start) /* 大于起控点 */
				{
					calc = (u32)calc - sd.agc_Start;
					calc /= sd.agc_Start;
					agc_gain -= agc_gain * calc *AGC_FAST;
					if(agc_gain <330 )agc_gain = 330;
				}
				else
				//if((u32)calc < sd.agc_Start)	/* 小于起控点 */
				{
					if(++agc_time>=ads.agc_constant )
					{
						agc_gain+=1;
						calc = sd.agc_Start - (u32)calc;
						calc /= sd.agc_Start;
						agc_gain += agc_gain * calc *AGC_SLOW;
						if(agc_gain >4095 )agc_gain = 4095;
						agc_time =0;
					}
				}
				/*
				* 	采用正向AGC控制，所以DAC2输出电压随信号增大而增加；
				*/
				DAC1->DHR12R2 = (u16)(agc_gain);/* DAC2输出 */			
				/*
				*	信号强度计算
				*/
				temp = sd.Riss *0.75f; 
				ads.Riss = (u8)(46 - (DAC1->DHR12R2-330) * 0.0122178f - temp +sd.S_correct + log10(1.0f/riss_gain)*20.0f)&0x7f;
				ads.agc_gain = SIGNAl_SET - ads.Riss;//去往频谱显示
				//ads.amp_adjust_num = (u8)((ads.Riss -78)*0.2f);
				//if(ads.amp_adjust_num>11)ads.amp_adjust_num =11;
				//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
				/*
				*	Hilbert Transform 
				*/
				arm_fir_f32(&Hilbert_I,(float32_t*)Data_I1,(float32_t*)Data_I,SAMPLE_SIZE/2);
				arm_fir_f32(&Hilbert_Q,(float32_t*)Data_Q1,(float32_t*)Data_Q,SAMPLE_SIZE/2);
				
				
				/*
				*	镜频消除 
				*/
				arm_add_f32((float32_t *)Data_I, (float32_t *)Data_Q, (float32_t *)ads.b_buffer, SAMPLE_SIZE/2);	// 相加提取上边带USB
				arm_sub_f32((float32_t *)Data_I, (float32_t *)Data_Q, (float32_t *)ads.a_buffer, SAMPLE_SIZE/2);	// 相减提取下边带LSB
				/*
				*	 FFT数据提取；
				*/
				if(TR_READ==CONTROL_RX)
				{
					if(spe.fft_flang ==0)
					{
						for(i=0;i<SAMPLE_SIZE/2;i++)
						{
							spe.fft_num ++;//FFT数据采样计数；
							if(spe.fft_num > FFT_LENGTH*2-1)
							{
								spe.fft_num =0;//FFT数据采样计数；
								spe.fft_flang =1;
							}
							fft_buf[spe.fft_num*2] = ads.a_buffer[i];
							fft_buf[spe.fft_num*2+1] = ads.b_buffer[i];
						}
					}
				}
				/*
				*	DSP本振
				*/
				
				softdds_runf(DDS_LO1,ads.ddslo_q ,ads.ddslo_i, SAMPLE_SIZE/2);
				/* DSP混频，生成 I/Q信号 */
				for(i=0;i<SAMPLE_SIZE/2;i++)
				{
					Data_I[i]= ads.a_buffer[i] *ads.ddslo_i[i];//LO1_I;// 数字1变频同相 I
					Data_Q[i]= ads.a_buffer[i] *ads.ddslo_q[i];//LO1_Q;//数字1变频移相 Q	
				}
				
				/*
				* 	信号抽取，降低采样率；
				* 	低通带宽低于降低后的采样率的1/2；
				*/
				arm_fir_decimate_f32(&DECIMATE_I, (float32_t *)Data_I, (float32_t *)Data_I, SAMPLE_SIZE/2);
				arm_fir_decimate_f32(&DECIMATE_Q, (float32_t *)Data_Q, (float32_t *)Data_Q, SAMPLE_SIZE/2);
				
				/* CW和单边带接收 */
				if(vfo[VFO_A].Mode <  DEMOD_AM)	
				{
					/*
					*	Hilbert Transform 
					*/
					//arm_fir_f32(&Hilbert_I,(float32_t*)Data_I,(float32_t*)Data_I1,psize);
					//arm_fir_f32(&Hilbert_Q,(float32_t*)Data_Q,(float32_t*)Data_Q1,psize);
				

					/*  
					* 	IIR低通滤波器，决定最终的接收带宽；
					*/
					arm_biquad_cascade_df1_f32(&IIR_LPF_I, (float32_t *)Data_I, (float32_t *)Data_I, psize);//LP低通滤波 I通道
					arm_biquad_cascade_df1_f32(&IIR_LPF_Q, (float32_t *)Data_Q, (float32_t *)Data_Q, psize);//LP低通滤波 Q通道
					/*
					* 	LP_Values是IIR滤波器缩放系数，
					*/
					arm_scale_f32(Data_I, IIR_LP_Values , Data_I,psize);
					arm_scale_f32(Data_Q, IIR_LP_Values , Data_Q,psize);
					//
					/*BFO*/
					softdds_runf(DDS_BFO,ads.ddsbfo_q ,ads.ddsbfo_i, psize);
					//CW/SSB解调
					for(i=0;i<psize;i++)
					{	
		
						if(vfo[VFO_A].Mode == DEMOD_LSB_DIG ||  vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_CW_LSB)
							ads.a_buffer[i] =  Data_Q[i] * ads.ddsbfo_q[i] - Data_I[i] * ads.ddsbfo_i[i]; //数字2变频后相减得到下边带LSB音频
						 
						if(vfo[VFO_A].Mode == DEMOD_USB_DIG ||  vfo[VFO_A].Mode == DEMOD_USB ||  vfo[VFO_A].Mode == DEMOD_CW_USB)
							ads.a_buffer[i] =  Data_Q[i] * ads.ddsbfo_q[i] + Data_I[i] * ads.ddsbfo_i[i];//数字2变频后相加得到上边带LSB音频	
							
//						if(vfo[VFO_A].Mode == DEMOD_LSB_DIG ||  vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_CW_LSB)
//							ads.a_buffer[i] =  Data_I[i] + Data_Q[i] ; //数字2变频后相减得到下边带LSB音频
//						 
//						if(vfo[VFO_A].Mode == DEMOD_USB_DIG ||  vfo[VFO_A].Mode == DEMOD_USB ||  vfo[VFO_A].Mode == DEMOD_CW_USB)
//							ads.a_buffer[i] =  Data_I[i] - Data_Q[i] ;//数字2变频后相加得到上边带LSB音频	
					}
					//arm_biquad_cascade_df1_f32(&IIR_AF, (float32_t *)ads.a_buffer, (float32_t *)ads.a_buffer, psize);//LP低通滤波 Q通道
					//arm_scale_f32((float32_t *)ads.a_buffer, SSB_AF_ScaleValues, (float32_t *)ads.a_buffer,psize);
					//arm_copy_f32(ads.a_buffer,riss_buf ,psize);
					arm_scale_f32(ads.a_buffer,riss_gain,riss_buf ,psize);
					
					for(i=0;i<psize;i++)
					{
						ads.a_buffer[i] *= agc2_gain;
						calc = ads.a_buffer[i];
						if(calc <0)calc*=-1;
						//二次AGC计算
						if(calc > AGC_THRESHOLD)
						{
							calc =calc - AGC_THRESHOLD;
							calc /= AGC_THRESHOLD;
							agc2_gain -= agc2_gain * calc * AGC_FAST;
							if(agc2_gain <0.00001f )agc2_gain =0.00001f ;
						}
						else
						//if(calc < AGC_SSB_THRESHOLD)
						{
							if(++agc2_time>=ads.agc_constant )
							{
								calc = AGC_THRESHOLD - calc;
								calc /= AGC_THRESHOLD;
								agc2_gain +=  agc2_gain * calc * AGC_SLOW;
								if(agc2_gain > 1.0f )agc2_gain = 1.0f ;
								agc2_time =0;
							}
						}
					}
					//
					/*
					*	信号强度二次取样计算
					*/
					arm_mean_f32((float32_t *)riss_buf ,psize, (float32_t *)&calc );
					
					if(calc <0)calc*=-1;
					if(calc > RISS_SSB_THRESHOLD)
					{
						calc =calc - RISS_SSB_THRESHOLD;
						calc /= RISS_SSB_THRESHOLD;
						riss_gain -= riss_gain * calc * RISS_FAST;
						if(riss_gain <0.0000001f )riss_gain =0.0000001f ;
					}
					else
					{
						calc = RISS_SSB_THRESHOLD - calc;
						calc /= RISS_SSB_THRESHOLD;
						riss_gain += riss_gain * calc * RISS_SLOW;
						if(riss_gain > AGC_GAIN_MAX )riss_gain = AGC_GAIN_MAX;
					}
					//
					// DSP noise reduction using LMS (Least Mean Squared) algorithm
					//
					if(ks.NR_key ==1)// && ( vfo[VFO_A].Mode == DEMOD_LSB ||  vfo[VFO_A].Mode == DEMOD_USB))	 	// Do this if enabled and "Pre-AGC" DSP NR enabled
					{
						audio_lms_noise_reduction(psize);	
					}
					arm_biquad_cascade_df1_f32(&IIR_AF, (float32_t *)ads.a_buffer, (float32_t *)ads.a_buffer, psize);//LP低通滤波 Q通道
					arm_scale_f32((float32_t *)ads.a_buffer, SSB_AF_ScaleValues, (float32_t *)ads.a_buffer,psize);
					
					
					//内插恢复到原采样，兼做低通；
					arm_fir_interpolate_f32(&INTERPOLATE_I,(float32_t *)ads.a_buffer, (float32_t *)ads.b_buffer, psize);
					//
					for(i=0;i<SAMPLE_SIZE/2 ;i++)
					{	
						ads.out_buff [i*2] = ads.b_buffer[i]*vol*4.0f*ads.Af_mute;//  + DAC_OFFSET_LEVEL ;
						ads.out_buff [i*2+1] = ads.b_buffer[i]*vol*4.0f*ads.Af_mute;// + DAC_OFFSET_LEVEL ;
					}
				}
				//break;
				else
				//case DEMOD_AM:
				if(vfo[VFO_A].Mode  ==  DEMOD_AM)	
				{
					//arm_fir_decimate_f32(&DECIMATE_I, (float32_t *)Data_I, (float32_t *)Data_I, SAMPLE_SIZE);
					//arm_fir_decimate_f32(&DECIMATE_Q, (float32_t *)Data_Q, (float32_t *)Data_Q, SAMPLE_SIZE);
					
					arm_biquad_cascade_df1_f32(&IIR_LPF_I, (float32_t *)Data_I, (float32_t *)Data_I, psize);//LP低通滤波 I通道
					arm_biquad_cascade_df1_f32(&IIR_LPF_Q, (float32_t *)Data_Q, (float32_t *)Data_Q, psize);//LP低通滤波 Q通道
					arm_scale_f32(Data_I, IIR_LP_Values , Data_I,psize);//I通道二次AGC控制，LP_Values是IIR滤波器系数
					arm_scale_f32(Data_Q, IIR_LP_Values , Data_Q,psize);//Q通道二次AGC控制，LP_Values是IIR滤波器系数
					//arm_max_f32((float32_t *)riss_buf, psize, (float32_t *)&calc0, 0);/* AGC样品，提取最大值 */
					//arm_min_f32((float32_t *)riss_buf, psize, (float32_t *)&calc1, 0);/* AGC样品，提取最大值 */
					/*
					*	AM解调
					*/
					for(i=0;i<psize;i++)
					{	
						/*AM解调*/	
						arm_sqrt_f32 ((Data_Q[i] *Data_Q[i] + Data_I[i] * Data_I[i]), (float32_t *)&ads.a_buffer[i] );//AM解调
						//ads.out_buff[i] = (u16)(ads.a_buffer[i]*sd.Vol * 0.05f + DAC_OFFSET_LEVEL);
					}
					arm_biquad_cascade_df1_f32(&RISS, ads.a_buffer, riss_buf, psize);//LP低通滤波 Q通道
					arm_scale_f32(riss_buf, Riss_ScaleValues*riss_gain, riss_buf,psize);
					//arm_scale_f32(ads.a_buffer, riss_gain, riss_buf ,psize);//信号强度二次取
					
					for(i=0;i<psize;i++)
					{
						ads.a_buffer[i] *= agc2_gain;
						calc = ads.a_buffer[i];
						if(calc <0)calc*=-1;
						//二次AGC计算
						if(calc > AGC_THRESHOLD)
						{
							calc =calc - AGC_THRESHOLD;
							calc /= AGC_THRESHOLD;
							agc2_gain -= agc2_gain * calc * AGC_FAST;
							if(agc2_gain <0.00001f )agc2_gain =0.00001f ;
						}
						else
						//if(calc < AGC_SSB_THRESHOLD)
						{
							if(++agc2_time>=ads.agc_constant )
							{
								calc = AGC_THRESHOLD - calc ;
								calc /= AGC_THRESHOLD;
								agc2_gain +=  agc2_gain * calc * AGC_SLOW;
								if(agc2_gain > 1 )agc2_gain = 1;
								agc2_time =0;
							}
						}
					}
					//

					/*
					*	信号强度二次取样计算
					*/
					arm_mean_f32((float32_t *)riss_buf ,psize, (float32_t *)&calc );
					//arm_max_f32((float32_t *)riss_buf, psize+1, (float32_t *)&calc0, 0);/* AGC样品，提取最大值 */
					//arm_min_f32((float32_t *)riss_buf, psize+1, (float32_t *)&calc1, 0);/* AGC样品，提取最大值 */
					//calc1=calc0-calc1;
					if(calc <0)calc*=-1;
					if(calc > RISS_AMFM_THRESHOLD)
					{
						calc =calc - RISS_AMFM_THRESHOLD;
						calc /= RISS_AMFM_THRESHOLD;
						riss_gain -= riss_gain * calc * RISS_FAST;
						if(riss_gain <0.00001f )riss_gain =0.00001f ;
					}
					else
					{
						//if(++Riss_time>=ads.agc_constant )
						//{
							calc = RISS_AMFM_THRESHOLD - calc ;
							calc /= RISS_AMFM_THRESHOLD;
							riss_gain += riss_gain * calc * RISS_SLOW;
							if(riss_gain > AGC_GAIN_MAX )riss_gain = AGC_GAIN_MAX;
							//Riss_time =0;
						//}
					}
					/*
					*	AF低通滤波
					*/
					arm_biquad_cascade_df1_f32(&IIR_AF, (float32_t *)ads.a_buffer, (float32_t *)ads.a_buffer, psize);//LP低通滤波 Q通道
					arm_scale_f32((float32_t *)ads.a_buffer, AF_ScaleValues , (float32_t *)ads.a_buffer,psize);
					//内插恢复到原采样，兼做低通；
					arm_fir_interpolate_f32(&INTERPOLATE_I,(float32_t *)ads.a_buffer, (float32_t *)ads.b_buffer, psize);
					/*
					*
					*/
					for(i=0;i<SAMPLE_SIZE/2;i++)
					{	
						/*AM解调*/	
						ads.out_buff [i*2] = ads.b_buffer[i]*vol*0.4f*ads.Af_mute;// + DAC_OFFSET_LEVEL ;
						ads.out_buff [i*2+1] = ads.b_buffer[i]*vol*0.4f*ads.Af_mute;//  + DAC_OFFSET_LEVEL ;
					}
				}
				else
				/*
				*	case DEMOD_FM:
				*/
				if(vfo[VFO_A].Mode == DEMOD_FM)
				{
					/*载波提取*/
					for(i=0;i<psize;i++)
					{		
						arm_sqrt_f32 ((Data_Q[i] *Data_Q[i] + Data_I[i] * Data_I[i]), (float32_t *)&ads.b_buffer[i] );//
					}
					arm_biquad_cascade_df1_f32(&RISS, ads.b_buffer, riss_buf, psize);//LP低通滤波 Q通道
					arm_scale_f32(riss_buf, Riss_ScaleValues*riss_gain, riss_buf,psize);
					//arm_scale_f32(ads.b_buffer, riss_gain, riss_buf ,psize);//信号强度二次取
					/*
					*	二次AGC
					*/
					for(i=0;i<psize;i++)
					{	
						Data_I[i] *= agc0;
						Data_Q[i] *= agc0;
						arm_sqrt_f32 ((Data_Q[i] *Data_Q[i] + Data_I[i] * Data_I[i]), (float32_t *)&ads.b_buffer[i] );
						calc = ads.b_buffer[i];
						if(calc <0)calc*=-1;
						if(calc > AGC_THRESHOLD )
						{
							calc  = calc - AGC_THRESHOLD;
							calc /= AGC_THRESHOLD;
							agc0 -=   agc0 * calc * AGC_SLOW;
							if(agc0 <0.00001f )agc0 = 0.00001f ;
						}
						else
						{
							//if(++agc2_time>=ads.agc_constant )
							//{
								calc = AGC_THRESHOLD - calc ;
								calc/= AGC_THRESHOLD;
								agc0 +=  agc0 * calc * AGC_FAST;
								if(agc0 > AGC_GAIN_MAX_FM )agc0 = AGC_GAIN_MAX_FM ;
							//	agc2_time =0;
							//}
						}
						arm_sqrt_f32( agc2_gain,&agc0);
						ads.a_buffer[i] *= agc0;
					}
					
					//
					/*
					*	信号强度二次取样计算
					*/
					arm_mean_f32((float32_t *)riss_buf ,psize, (float32_t *)&calc );
					if(calc <0)calc*=-1;
					if(calc > RISS_AMFM_THRESHOLD )
					{
						calc =calc - RISS_AMFM_THRESHOLD;
						calc /= RISS_AMFM_THRESHOLD;
						riss_gain -= riss_gain * calc * RISS_FAST;
						if(riss_gain <0.00001f )riss_gain =0.00001f ;
					}
					else
					{
						//if(++Riss_time>=ads.agc_constant )
						//{
							calc = RISS_AMFM_THRESHOLD - calc ;
							calc /= RISS_AMFM_THRESHOLD;
							riss_gain += riss_gain * calc * RISS_MED;
							if(riss_gain > AGC_GAIN_MAX )riss_gain = AGC_GAIN_MAX;
							//Riss_time =0;
						//}
					}
						/*FM解调*/
					/*******************************************************************************
						正交相关法FM解调  （正式采用的算法）
						公式：
						X(n)=[Q(n)/I(n)-Q(n-1)/I(n-1)] / [Q(n)*Q(n)+I(n)*I(n)]
					*******************************************************************************/
					/***正交相关法FM解调***/
					for(i=0;i<psize;i++)
					{
						//	
						
						dp_I = Data_I[i] *Data_I[i] + Data_Q[i] * Data_Q[i];
						dp_Q = Data_Q[i] * FM_I[1] - Data_I[i] * FM_Q[1];
						//
						if (dp_Q > 0)
						{
							if (dp_I > 0) 				// Quadrant I
							{
								ratio = (dp_I - dp_Q) / (dp_I + dp_Q);
								ads.a_buffer[i] = (1 - ratio);
							}
							else 						// Quadrant II
							{
								ratio = (dp_I + dp_Q) / (dp_Q - dp_I);
								ads.a_buffer[i] = (3 - ratio);
							}
						}
						else
						{
							if (dp_I < 0)				 // Quadrant III
							{
								ratio = (dp_Q - dp_I) / (dp_I + dp_Q);
								ads.a_buffer[i] = (ratio-3);
							}
							else 						 // Quadrant IV
							{
								ratio = (dp_I + dp_Q) / (dp_I - dp_Q);
								ads.a_buffer[i] = (ratio-1);
							}
						}
						//
						if(FM_5K)  ads.a_buffer[i] *= FM_RX_SCALING_2K5;
						else
						if(FM_10K) ads.a_buffer[i] *= FM_RX_SCALING_5K;
						
						FM_I[1] = Data_I[i];
						FM_Q[1] = Data_Q[i];
					}
					//
					/*******************************************************************************
						采用小角度接近法: (本机未采用，只是作为参考)
							公式：
							X(n)=[Q(n)*I(n-1)-I(n)*Q(n-1)] 
					*******************************************************************************/
	//					else
	//					if(k_num.SPE_K1==1)
	//					{
	//						/***小角度接近法FM解调***/
	//						for(i=0;i<psize;i++)
	//						{	
	//							
	//							//dp_I = Data_I[i] * FM_I[1] + Data_I[i] * FM_I[1];//FM解调
	//							ads.a_buffer[i] = Data_Q[i] * FM_I[1] - Data_I[i] * FM_Q[1];//FM解调					
	//							//
	//							if(FM_5K)  ads.a_buffer[i] *= FM_RX_SCALING_2K5;
	//							else
	//							if(FM_10K) ads.a_buffer[i] *= FM_RX_SCALING_5K;
	//							//
	//							FM_I[1] = Data_I[i];
	//							FM_Q[1] = Data_Q[i];
	//						}
	//					}
					
					/*
					*	AF低通滤波
					*/
					
					arm_biquad_cascade_df1_f32(&IIR_AF, (float32_t *)ads.a_buffer, (float32_t *)ads.a_buffer, psize);//LP低通滤波 Q通道
					arm_scale_f32((float32_t *)ads.a_buffer, AF_ScaleValues, (float32_t *)ads.a_buffer,psize);
					//内插恢复到原采样，兼做去加重低通；
					arm_fir_interpolate_f32(&INTERPOLATE_I,(float32_t *)ads.a_buffer, (float32_t *)ads.b_buffer,psize);
					//加载到输出
					for(i=0;i<SAMPLE_SIZE/2;i++)
					{	
						ads.out_buff [i*2] = ads.b_buffer[i]*vol*0.02f*ads.Af_mute;//  + DAC_OFFSET_LEVEL ;
						ads.out_buff [i*2+1] = ads.b_buffer[i]*vol*0.02f*ads.Af_mute;//  + DAC_OFFSET_LEVEL ;
						//ads.out_buff[i] =(u16)(Data_I[i]*sd.Vol * 0.01f + DAC_OFFSET_LEVEL);
					}
				}
				if(vfo[VFO_A].Mode>DEMOD_CW_USB && ads.tx_delay >10 )
				{
					vol =0;
					//ads.Af_mute =0;
					for(i=0;i<SAMPLE_SIZE ;i++)
					{	
						ads.b_buffer[i] = 0;//+ DAC_OFFSET_LEVEL) ;
					}
				}
			}
		}
/* 
***********************************************************************************************************************
*			TX发射处理
***********************************************************************************************************************
*/		
		
		if(ks.rt_rx ==0 && ks.tx_no !=1 )//&&(PTT_RT == 0 || cw_Auto_key.dit==0 || cw_Auto_key.dah==0))//&& vfo[VFO_A].Band_id != COUPLING_60M)/* 收发信机模式 */
		{
			//pow_gain_set();
			/* CW模式发射 */
			if(vfo[VFO_A].Mode <  DEMOD_LSB)/**/	
			{
				MIC_POWER(1);
				/*
				*	di/da键转换
				*/
				if(ks.CW_AB ==1)
				{
					cw_Auto_key.dit = CW_K_DI;
					cw_Auto_key.dah = CW_K_DA;
				}
				else  
				{
					cw_Auto_key.dit = CW_K_DA;
					cw_Auto_key.dah = CW_K_DI;
				}
				//softdds_runf(DDS_SI,ads.ddssi_i,ads.ddssi_q, SAMPLE_SIZE/2);
				//softdds_runf(DDS_LO1,ads.ddslo_i ,ads.ddslo_q, SAMPLE_SIZE/2);//CW载频
				/*
				*	手动键模式
				*/
				if(ks.CW_keySW ==1 /*&& ks.PWR_key ==0*/)
				{
					if(PTT_RT == TX || cw_Auto_key.dit == 0 )/* 手动键按下 */
					{
						ads.tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
						if(ks.Spk_key)ads.spk_vol =0;
						else ads.spk_vol =63;
						ads.hp_vol =55;
						
						AMP_OFF;
						ATT_OFF;
						
						softdds_runf(DDS_SI,ads.ddssi_i,ads.ddssi_q, SAMPLE_SIZE/2);
						if(ks.CW_Exercise <1)/* 如果不在练习模式 */
						{
							TR_CONTROL(CONTROL_TX);/* 如果不在练习模式,发射开启 */
							softdds_runf(DDS_TX_LO,ads.ddslo_i_tx ,ads.ddslo_q_tx, SAMPLE_SIZE/2);//CW载频
						}
						
						if(sd.menu_page ==1)sd.Pow =50;
		
						/* ALC */
						
						calc = ads.pow;/* ALC取样 */
						if(calc<0)calc*=-1;
						if(sd.Pow !=50)
						{	
							//if(calc<0)calc*=-1;
							if(calc > sd.Pow )
							{
								calc = calc - sd.Pow;
								calc = calc / sd.Pow;
								arm_sqrt_f32 (calc, (float32_t *)&calc );
								cw_alc_gain -= cw_alc_gain * calc * 0.01f;
								if(cw_alc_gain  <0.1f )cw_alc_gain = 0.1f;
							}
							if(calc < sd.Pow)	
							{
								calc =  sd.Pow -calc;
								calc = calc/ sd.Pow;
								arm_sqrt_f32 (calc, (float32_t *)&calc );
								cw_alc_gain += cw_alc_gain * calc * 0.0001f;
								if(cw_alc_gain >1.0f)cw_alc_gain = 1.0f;
							}
							ads.pow_gain_temp=0;
						}
						else
						{
							cw_alc_gain=1.0f;
							if(ads.pow_gain_temp==0)
							{
								if(ads.pow > sd.Pow )
								{
									ads.pow_gain [vfo[VFO_A].Band_id ] -=1;
									if(ads.pow_gain [vfo[VFO_A].Band_id ]<1)ads.pow_gain [vfo[VFO_A].Band_id ]=1;
									lock=100;
								}
								
								if(ads.pow < sd.Pow )
								{
									ads.pow_gain [vfo[VFO_A].Band_id ] +=1;
									if(ads.pow_gain [vfo[VFO_A].Band_id ]>POW_GAIN_MAX)ads.pow_gain [vfo[VFO_A].Band_id ]=1;
									lock=100;
								}
							 	
								if((ads.pow >= sd.Pow)&&(ads.pow <= sd.Pow+3))
								{
									if(lock>0)lock--;
									if(lock<1)ads.pow_gain_temp=1;	
								}									
							}
							/*	计算设定功率下各频段增益系数	*/
							//arm_sqrt_f32 (sd.Pow*50, (float32_t *)&calc);//计算标定功率下的电压值
							//if(sd.Pow <10)ads.cw_gain[1] =((10-sd.Pow)+ads.pow_gain [vfo[VFO_A].Band_id ]) *calc*500.0f;
							//else
							//if(sd.Pow>=10&&sd.Pow <=100)
							ads.cw_gain[1] =ads.pow_gain [vfo[VFO_A].Band_id ]*2500.0f;
							//else 
							//if(sd.Pow>100)ads.cw_gain[1] =((sd.Pow -100)*0.12f+ads.pow_gain [vfo[VFO_A].Band_id ]) *calc*500.0f;
						}
						for(i=0;i<SAMPLE_SIZE/2;i++)
						{	
							ads.si_i [i] = ads.ddssi_i[i] * sd.CW_vol*63;
						
							Data_I[i] = ads.ddslo_i_tx[i] * cw_alc_gain*ads.cw_gain[1];//*ads.cw_gain[2];//*ads.tx_amp ;//
							Data_Q[i] = ads.ddslo_q_tx[i] * cw_alc_gain*ads.cw_gain[1];//*ads.cw_gain[2];//
						}
//						if(vfo[VFO_A].Band_id<COUPLING_6M)
//						{
//							if(ads.pow ==200)
//							{
//								arm_max_f32(Data_I, SAMPLE_SIZE/2, (float32_t *)&calc, 0);
//								
//								if(ads.pow_amp[vfo[VFO_A].Band_id] <calc||ads.pow_amp[vfo[VFO_A].Band_id] >calc)
//								ads.pow_amp[vfo[VFO_A].Band_id] =(u32)calc;
//							}
//						}
//						else
						//{
//							if(sd.Pow ==50)
//							{
//								arm_mean_f32(Data_I,SAMPLE_SIZE/2, (float32_t *)&calc );
//								if(calc<0)calc*=-1;
//								calc *=ads.cw_gain[3];
//								//if(ads.pow_amp[vfo[VFO_A].Band_id] <calc||ads.pow_amp[vfo[VFO_A].Band_id] >calc)
//								if(ads.pow ==sd.Pow)ads.pow_amp[vfo[VFO_A].Band_id] =(u32)calc;
//							}
						//}
						ads.tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
					}
					//else 
					if(PTT_RT == RX && cw_Auto_key.dit == 1 )// 手动键松开 
					{
						if(ads.tx_delay)
						{
							for(i=0;i<SAMPLE_SIZE/2;i++)
							{	
								ads.si_i[i] = 0;
								Data_I[i] = 0;
								Data_Q[i] = 0;
							}
						}
					}
					
				}
				//else
				/*
				*	自动键模式
				*/
				if(ks.CW_keySW ==0)
				{
					if(PTT_RT == 1 && cw_Auto_key.dit == 0 && cw_Auto_key.dah ==0)/* 自动键模式 di滴键和哒键都按下 */
					{
						cw_Auto_key.test_state =3;
						//ads.tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
					}
					else 
					if(PTT_RT == 1 && cw_Auto_key.dit == 0 )/* 自动键模式 di滴键按下 */
					{
						cw_Auto_key.test_state =2;
						//ads.tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
					}
					else					
					if(PTT_RT == 1 && cw_Auto_key.dah ==0)/* 自动键模式 da哒键按下 */
					{
						cw_Auto_key.test_state =1;
						//ads.tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
					}
					else
					if(PTT_RT == 1 && cw_Auto_key.dit == 1 && cw_Auto_key.dah ==1)/* 自动键模式 da哒键和哒键都松开 */
					{
						cw_Auto_key.test_state =0;
					}
					
					if(cw_Auto_key.now_state != cw_Auto_key.test_state )cw_Auto_key.now_state = cw_Auto_key.test_state;
					
					//softdds_runf(DDS_SI,ads.ddssi_i,ads.ddssi_q, SAMPLE_SIZE/2);
					//softdds_runf(DDS_LO1,ads.ddslo_i ,ads.ddslo_q, SAMPLE_SIZE/2);//CW载频
					if(cw_Auto_key.ms_time >0)
					{
						if(ks.Spk_key)ads.spk_vol =0;
						else ads.spk_vol =63;
						ads.hp_vol =55;
						AMP_OFF;
						ATT_OFF;
				
						//AMP_OFF;
						if(cw_Auto_key.ms_time >ads.cw_spe)
						{
							softdds_runf(DDS_SI,ads.ddssi_i,ads.ddssi_q, SAMPLE_SIZE/2);
							if(ks.CW_Exercise <1)//如果不在练习模式
							{
								TR_CONTROL(CONTROL_TX);/* 发射开启 */
								softdds_runf(DDS_TX_LO,ads.ddslo_i_tx ,ads.ddslo_q_tx, SAMPLE_SIZE/2);//CW载频
							}
							/*	ALC	*/
							if(sd.menu_page ==1)sd.Pow =50;
							calc = ads.pow;/* ALC取样 */
							if(calc<0)calc*=-1;
							if(calc > sd.Pow )
							{
								calc = calc - sd.Pow;
								calc = calc / sd.Pow;
								arm_sqrt_f32 (calc, (float32_t *)&calc );
								cw_alc_gain -= cw_alc_gain * calc * 0.01f;
								if(cw_alc_gain  <0.1f )cw_alc_gain = 0.1f;
							}
							if(calc < sd.Pow)	
							{
								calc =  sd.Pow -calc;
								calc = calc/ sd.Pow;
								arm_sqrt_f32 (calc, (float32_t *)&calc );
								cw_alc_gain += cw_alc_gain * calc * 0.0001f;
								if(cw_alc_gain >1.0f)cw_alc_gain = 1.0f;
							}
							//
							for(i=0;i<SAMPLE_SIZE/2;i++)
							{
								ads.si_i [i] = ads.ddssi_i[i] * sd.CW_vol*63;
								//ads.a_buffer[i] = ads.ddssi_q[i] * cw_alc_gain*500;//
								//if(ks.CW_Exercise <1)//如果不在练习模式
								//{
								
								Data_I[i] = ads.ddslo_i_tx[i] * cw_alc_gain*ads.cw_gain[1];//*ads.cw_gain[2];//*ads.tx_amp ;//
								Data_Q[i] = ads.ddslo_q_tx[i] * cw_alc_gain*ads.cw_gain[1];//*ads.cw_gain[2];//
								//}
							}
							//
							ads.tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
							//arm_fir_f32(&Hilbert_I,ads.a_buffer,(float32_t*)Data_I,SAMPLE_SIZE/2);
							//arm_fir_f32(&Hilbert_Q,ads.a_buffer,(float32_t*)Data_Q,SAMPLE_SIZE/2);
						}
						else
						{
							if(ads.tx_delay)
							{
								for(i=0;i<SAMPLE_SIZE/2;i++)
								{
									Data_I[i] = 0;
									Data_Q[i] = 0;
									ads.si_i[i] *= 0;//sd.CW_vol*500;
								}
							}
				
						}
						/* CW自动键速度记时 */
						cw_Auto_key.ms_time --;//时间自减
						//
						if(cw_Auto_key.mm_count==0)
						{
							if((cw_Auto_key.now_state>0)&&(cw_Auto_key.now_state!=cw_Auto_key.out_state) )
							{
								if(cw_Auto_key.now_state<3)
								{
									cw_Auto_key.mm_state=cw_Auto_key.now_state;
								}
								else
								{
									if(cw_Auto_key.out_state==1)cw_Auto_key.mm_state=2;
									if(cw_Auto_key.out_state==2)cw_Auto_key.mm_state=1;
								 }
								 cw_Auto_key.mm_count =1;
							 }
						}
					}
					else//如果自动发报发完了，判断下一步应该发什么
					{
						if(cw_Auto_key.mm_count >0)//如果上一次还有存储键，则提取键值
						{
							cw_Auto_key.out_state = cw_Auto_key.mm_state;
							cw_Auto_key.mm_count  =0;
						}
						else
						{
							cw_Auto_key.out_state = cw_Auto_key.now_state;
						}
				
						if(cw_Auto_key.out_state ==0)//2个按键都放开
						{
							if(ks.CW_keySW ==2)//B模式
							{
								if(cw_Auto_key.last_one ==0 && cw_Auto_key.last_key_state ==3)//还没有补发一个反码
								{
									if(cw_Auto_key.last_auto_key ==1)cw_Auto_key.ms_time = ads.cw_spe*4;//发DA
									else 
										cw_Auto_key.ms_time = ads.cw_spe*2;//发DI
								}
								cw_Auto_key.last_one =1;
							}
							cw_Auto_key.last_key_state =0;
							cw_Auto_key.last_auto_key =0;
							//TX_sign =0;
						}
						else
						if(cw_Auto_key.out_state==1)//滴 按下
						{
							if(ads.tx_delay)cw_Auto_key.ms_time =ads.cw_spe*2;//发di
							else cw_Auto_key.ms_time =ads.cw_spe*2 +10;//发di 第一次拍发时间补偿
							cw_Auto_key.last_auto_key=1;
							cw_Auto_key.last_key_state=1;
							cw_Auto_key.last_one=0;
						}
						else
						if(cw_Auto_key.out_state==2)//嗒 按下
						{
							if(ads.tx_delay)cw_Auto_key.ms_time =ads.cw_spe*4;//发da
							else cw_Auto_key.ms_time =ads.cw_spe*4 +10;//发da 第一次拍发时间补偿
							cw_Auto_key.last_auto_key=2;
							cw_Auto_key.last_key_state=2;
							cw_Auto_key.last_one=0;
						}
						else
						if(cw_Auto_key.out_state==3) //滴 答 都按下
						{
							 if(cw_Auto_key.last_auto_key==1)//上次=di
							 {
								 cw_Auto_key.ms_time =ads.cw_spe*4;//发da
								 cw_Auto_key.last_auto_key=2;
							 }
							 else
							 {
								 cw_Auto_key.ms_time =ads.cw_spe*2;//发di
								 cw_Auto_key.last_auto_key=1;
							 }
							 cw_Auto_key.last_key_state=3;
							 cw_Auto_key.last_one=0;
						}
					}
				}

			}
			//else	
			/*
			*	SSB单边带模式发射处理
			*/
			if((PTT_RT == TX||ks.cat_ptt ==TX||ks.rit_tx==TX)&& (vfo[VFO_A].Mode >= DEMOD_LSB && vfo[VFO_A].Mode <= DEMOD_USB_DIG))	
			{
				//ads.spk_vol =0;
				//ads.hp_vol =0;
				
				if(ads.rx_delay>0)
				{
					ads.spk_vol =0;/* 音频关闭 */
					ads.hp_vol =0;
					ads.tx_delay = 300;
					for(i=0;i<SAMPLE_SIZE/2 ;i++)
					{	
						ads.out_buff [i*2] = 0;
						ads.out_buff [i*2+1] = 0;
					}
					if(ks.cat_ptt ==TX)MIC_POWER(1);
					else MIC_POWER(0);
					
					ads.rx_delay--;
					//if(ads.rx_delay<1)
					TR_CONTROL(CONTROL_TX);/* 发射开启 */
					if(ads.rx_delay>2)
					{
						ssb_mic_gain=0;
						ssb_alc_gain=0;
					}
					if(ads.rx_delay<=2)
					{
						ssb_mic_gain=0.5f;
						ssb_alc_gain =0.53f;
					}
				}
				if(ads.rx_delay<200)
				{
					if(ads.rx_delay<1)ads.rx_delay =0;
					//if(ads.rx_delay>0)
					//{
					//	mic_gain +=0.007f;
					//}
					
					if(ks.bt_key==1 && ks.cat_ptt ==TX)
						arm_fir_decimate_f32(&DECIMATE_Q, (float32_t *)Data_Q1,ads.a_buffer, SAMPLE_SIZE/2);
					else arm_fir_decimate_f32(&DECIMATE_I, (float32_t *)Data_I1,ads.a_buffer, SAMPLE_SIZE/2);
					arm_biquad_cascade_df1_f32(&IIR_AF, ads.a_buffer, ads.a_buffer, psize);//LP低通滤波 Q通道
					
					if(ks.cat_ptt ==TX)//声卡
						arm_scale_f32(ads.a_buffer,  AF_ScaleValues*30.0f, ads.a_buffer,psize);
					else 
					{
						if(sd.Pow >100)
						arm_scale_f32(ads.a_buffer,  AF_ScaleValues*(sd.mic_gain*1.4f+(sd.Pow-100)*0.1f), ads.a_buffer,psize);
						else
						arm_scale_f32(ads.a_buffer,  AF_ScaleValues*sd.mic_gain*1.4f, ads.a_buffer,psize);	
					}
					
					//arm_mean_f32(ads.a_buffer,psize, (float32_t *)&calc );
					//arm_max_f32(ads.a_buffer, psize, (float32_t *)&calc, 0);/* AGC样品，提取最大值 */
					/*
					*	ALC 
					*/
					
					for(i=0;i<psize;i++)
					{
						ads.a_buffer[i] *=ssb_mic_gain*ads.cw_gain[0]*2.0f;
						calc = ads.a_buffer[i];				
						if(calc<0)calc*=-1.0f;
						
						if(calc > ads.cw_gain[1])
						{
							calc = calc - ads.cw_gain[1];
							calc = calc / ads.cw_gain[1];
							//arm_sqrt_f32 (calc, (float32_t *)&calc );
							ssb_mic_gain -= ssb_mic_gain * calc*0.01f;
							if(ssb_mic_gain  < 0.01f )ssb_mic_gain = 0.01f;
							//ads.a_buffer[i] *=ssb_mic_gain;//MIC幅度过大，衰减加大
						}
						else
						//if(calc < ads.cw_gain[1])	
						{
							calc = ads.cw_gain[1] -calc;
							calc = calc/ ads.cw_gain[1];
							//arm_sqrt_f32 (calc, (float32_t *)&calc );
							ssb_mic_gain += ssb_mic_gain * calc * 0.0001f;
							if(ssb_mic_gain >1.0f)ssb_mic_gain = 1.0f;
						}
						//if(ads.a_buffer[i] > (ads.cw_gain[1]*2.0f))ads.a_buffer[i] *=ssb_mic_gain;
					}
					softdds_runf(DDS_BFO,ads.ddsbfo_i ,ads.ddsbfo_q , psize);
					softdds_runf(DDS_TX_LO,ads.ddslo_i_tx ,ads.ddslo_q_tx, SAMPLE_SIZE/2);//SSB本振
					for(i=0;i<psize;i++)
					{
						/* 一次调制生成90度相位差的I Q信号 */	
						Data_I1[i] = ads.a_buffer[i] * ads.ddsbfo_i[i];
						Data_Q1[i] = ads.a_buffer[i] * ads.ddsbfo_q[i];//LO2_Q;
						
					}
					arm_biquad_cascade_df1_f32(&IIR_LPF_I, Data_I1, Data_I1, psize);
					arm_biquad_cascade_df1_f32(&IIR_LPF_Q, Data_Q1, Data_Q1, psize);
					arm_scale_f32(Data_I1, IIR_LP_Values , Data_I1,psize);
					arm_scale_f32(Data_Q1, IIR_LP_Values , Data_Q1,psize);
					
					
					
					arm_fir_interpolate_f32(&INTERPOLATE_I,(float32_t *)Data_I1, (float32_t *)ads.a_buffer, psize);
					arm_fir_interpolate_f32(&INTERPOLATE_Q,(float32_t *)Data_Q1, (float32_t *)ads.b_buffer, psize);
					for(i=0;i<SAMPLE_SIZE/2;i++)
					{
						if(vfo[VFO_A].Mode == DEMOD_LSB ||vfo[VFO_A].Mode ==DEMOD_LSB_DIG )
						{
							Data_I1[i] = ads.b_buffer[i] * ads.ddslo_q_tx[i] - ads.a_buffer[i] * ads.ddslo_i_tx[i];//LSB下边带TX
						}
						else 
						if(vfo[VFO_A].Mode == DEMOD_USB ||vfo[VFO_A].Mode ==DEMOD_USB_DIG )	
						{
							Data_I1[i] = ads.b_buffer[i] * ads.ddslo_q_tx[i] + ads.a_buffer[i] * ads.ddslo_i_tx[i];//USB上边带TX
						}
						/*
						*	ALC
						*/
						Data_I1[i] *=ssb_alc_gain;
					}
						
						calc = ads.pow;/* ALC取样 */
						if(calc<0)calc*=-1;
						if(calc > sd.Pow)
						{
							calc = calc -sd.Pow;
							calc = calc / sd.Pow;
							arm_sqrt_f32 (calc, &calc );
							ssb_alc_gain -= ssb_alc_gain * calc * 0.01f;
							if(ssb_alc_gain  <0.01f )ssb_alc_gain = 0.01f;
						}
						//else
						if(calc < sd.Pow)	
						{
							calc = sd.Pow -calc;
							calc = calc/ sd.Pow;
							arm_sqrt_f32 (calc, (float32_t *)&calc );
							ssb_alc_gain += ssb_alc_gain * calc * 0.0001f;
							if(ssb_alc_gain >1.0f)ssb_alc_gain = 1.0f;
						}
					//}
					
					/*
					*	Hilbert Transform 
					*/
					if(ads.rx_delay<1)
					{
						arm_fir_f32(&Hilbert_I,Data_I1,Data_I,SAMPLE_SIZE/2);
						arm_fir_f32(&Hilbert_Q,Data_I1,Data_Q,SAMPLE_SIZE/2);
					}
					
					ads.tx_delay = 200;/* 接收延迟时间设置 */
				}
				if(ks.cat_ptt ==TX||ks.rit_tx==TX)
				{
					PTT_OUT();
					PTT_RT_VOX(TX);
				}
			}
			//else 
				
			/*
			*	AM调幅发射模式处理
			*/
			if((PTT_RT == TX ||ks.cat_ptt ==TX||ks.rit_tx==TX)&& vfo[VFO_A].Mode == DEMOD_AM)	
			{
				//ads.spk_vol =0;
				//ads.hp_vol =0;
				
				if(ads.rx_delay>0)
				{
					ads.spk_vol =0;/* 音频关闭 */
					ads.hp_vol =0;
					ads.tx_delay = 300;
					for(i=0;i<SAMPLE_SIZE ;i++)
					{	
						ads.out_buff [i] = 0;
					}
					if(PTT_RT == TX||ks.rit_tx==TX)MIC_POWER(0);
					
					ads.rx_delay--;
					//if(ads.rx_delay<150)
					TR_CONTROL(CONTROL_TX);/* 发射开启 */
					
					if(ads.rx_delay>2)
					{
						am_alc_gain =0;
						mic_gain=0;
					}
					if(ads.rx_delay<=2)
					{
						am_alc_gain =1.0f;
						mic_gain=1.0f;
					}
				}
				if(ads.rx_delay<200)
				{
					if(ads.rx_delay<1)ads.rx_delay =0;
					//if(ads.rx_delay>1)am_alc_gain +=0.006f;
					
					/*抽取降低采样率 */
					if(ks.bt_key==1 && ks.cat_ptt ==TX)
						arm_fir_decimate_f32(&DECIMATE_Q, (float32_t *)Data_Q1,ads.a_buffer, SAMPLE_SIZE/2);
					else arm_fir_decimate_f32(&DECIMATE_I, (float32_t *)Data_I1,ads.a_buffer, SAMPLE_SIZE/2);
					
					arm_biquad_cascade_df1_f32(&IIR_AF, (float32_t *)ads.a_buffer, (float32_t *)ads.a_buffer, psize);//LP低通滤波 Q通道
					//if(ks.bt_key!=1 && ks.cat_ptt ==TX)
						arm_scale_f32(ads.a_buffer,  AF_ScaleValues*sd.mic_gain, ads.a_buffer,psize);
					//else arm_scale_f32(ads.a_buffer,  AF_ScaleValues*sd.mic_gain*2.0f, ads.a_buffer,psize);
					//arm_max_f32(ads.a_buffer, psize+1, (float32_t *)&calc, 0);/* AGC样品，提取最大值 */
					//arm_mean_f32(ads.a_buffer,psize, (float32_t *)&calc );
					/*
					*	ALC 
					*/
					if(sd.menu_page ==1)sd.Pow =50;
					for(i=0;i<psize;i++)
					{
						//if(sd.menu_page ==1)sd.Pow =50;
						ads.a_buffer[i] *=mic_gain*ads.cw_gain[0];
						calc = ads.a_buffer[i];				
						if(calc<0)calc*=-1.0f;
					
						if(calc > ads.cw_gain[1])
						{
							calc = calc - ads.cw_gain[1];
							calc = calc / ads.cw_gain[1];
							mic_gain -= mic_gain * calc*0.1f;
							if(mic_gain  < 0.01f )mic_gain = 0.01f;
							//ads.a_buffer[i] *=mic_gain;//MIC幅度过大，衰减加大
						}
						//else
						if(calc < ads.cw_gain[1])	
						{
							calc = ads.cw_gain[1] -calc;
							calc = calc/ ads.cw_gain[1];
							mic_gain += mic_gain * calc * 0.001f;
							if(mic_gain >1.0f)mic_gain = 1.0f;
						}
						if(ads.a_buffer[i]>ads.cw_gain[1])ads.a_buffer[i] *=mic_gain;
					}
					arm_fir_interpolate_f32(&INTERPOLATE_I,(float32_t *)ads.a_buffer, (float32_t *)ads.b_buffer, psize);
					/*	
					*	AM调制公式  Sam(n) = Ac[1+m(n)]*Cos(2π*fc*n/k)
					*					   =AF*AC*LO + AC*LO
					*	Ac 载波振幅；m(n) 调制信号 
					*/
					//softdds_setfreq(DDS_TX_LO,(float32_t)(sd.Dsp_Lo1), SAMPLING_RETE, 0);
					softdds_runf(DDS_TX_LO,ads.ddslo_i_tx ,ads.ddslo_q_tx, SAMPLE_SIZE/2);//SSB本振
					for(i=0;i<SAMPLE_SIZE/2;i++)
					{	
						Data_I1[i] = (ads.b_buffer[i]+ads.cw_gain[1])*ads.ddslo_i_tx[i];//
						Data_Q1[i] = (ads.b_buffer[i]+ads.cw_gain[1])*ads.ddslo_q_tx[i];//ads.cw_gain[2]
					}
					/*
					*	ALC
					*/
					//am_alc_gain = 1.0f;
					calc = ads.pow;/* ALC取样 */
					if(calc<0)calc*=-1;
					if(calc > sd.Pow )
					{
						calc = calc - sd.Pow;
						calc = calc / sd.Pow;
						arm_sqrt_f32 (calc, (float32_t *)&calc );
						am_alc_gain -= am_alc_gain * calc * 0.1f;
						if(am_alc_gain  <0.1f )am_alc_gain = 0.1f;
					}
					//else
					if(calc < sd.Pow)	
					{
						calc =  sd.Pow -calc;
						calc = calc/ sd.Pow;
						arm_sqrt_f32 (calc, (float32_t *)&calc );
						am_alc_gain += am_alc_gain * calc * 0.001f;
						if(am_alc_gain >1.0f)am_alc_gain = 1.0f;
					}
					if(ads.rx_delay<1)
					{
						for(i=0;i<SAMPLE_SIZE/2;i++)
						{	
							Data_I[i] = Data_I1[i]*am_alc_gain*0.98f;//
							Data_Q[i] = Data_Q1[i]*am_alc_gain*0.98f;//
						}
					}
					ads.tx_delay = 200;/* 接收延迟时间设置 */
                }
				if(ks.cat_ptt ==TX||ks.rit_tx==TX)
				{
					PTT_OUT();
					PTT_RT_VOX(TX);
				}
			}
			//else 
				
			/*
			*	FM调频发射模式处理
			*/
			if((PTT_RT == TX||ks.cat_ptt ==TX||ks.rit_tx==TX) && vfo[VFO_A].Mode == DEMOD_FM)	
			{
				//ads.spk_vol =0;/* 音频关闭 */
				//ads.hp_vol =0;
				//ads.spk_vol =0;
				if(ads.rx_delay>0)
				{
					ads.spk_vol =0;/* 音频关闭 */
					ads.hp_vol =0;
					ads.tx_delay = 300;
					for(i=0;i<SAMPLE_SIZE/2 ;i++)
					{	
						ads.out_buff [i*2] = 0;
						ads.out_buff [i*2+1] = 0;
					}
					if(PTT_RT == TX||ks.rit_tx==TX)MIC_POWER(0);
					
					ads.rx_delay--;
					//if(ads.rx_delay<150)
					TR_CONTROL(CONTROL_TX);/* 发射开启 */
					if(ads.rx_delay>2)
					{
						fm_alc_gain =0;
						fm_gain=0;
					}
					if(ads.rx_delay<=2)
					{
						fm_alc_gain =1.0f;
						fm_gain=0.8f;
					}
				}
				if(ads.rx_delay<200)
				{
					if(ads.rx_delay<1)ads.rx_delay =0;
					//if(ads.rx_delay>0)fm_alc_gain +=0.0085f;
					
					/*抽取降低采样率 */
					if(ks.bt_key==1 && ks.cat_ptt ==TX)
						arm_fir_decimate_f32(&DECIMATE_Q, (float32_t *)Data_Q1,ads.a_buffer, SAMPLE_SIZE/2);
					else arm_fir_decimate_f32(&DECIMATE_I, (float32_t *)Data_I1,ads.a_buffer, SAMPLE_SIZE/2);
					
					arm_biquad_cascade_df1_f32(&IIR_AF, (float32_t *)ads.a_buffer, (float32_t *)ads.a_buffer, psize);//LP低通滤波 Q通道
					arm_scale_f32((float32_t *)ads.a_buffer, AF_ScaleValues *sd.mic_gain *0.00033f , (float32_t *)ads.a_buffer,psize);
					/*	频偏限制	  */
					//arm_max_f32(ads.a_buffer, SAMPLE_SIZE, &varI_1, 0);/* AGC样品，提取最大值 */
					//arm_min_f32(ads.a_buffer, SAMPLE_SIZE, &varI_0, 0);/* AGC样品，提取最大值 */
					//calc = (varI_1 - varI_0);
					for(i=0;i<psize;i++)
					{
						ads.a_buffer[i] *= fm_gain;
						calc = ads.a_buffer[i];
						if(calc <0)calc *=-1;
						
						if(calc > sd.fm_offset)
						{
							calc = calc - sd.fm_offset;
							calc = calc/ sd.fm_offset;
							fm_gain -= fm_gain*calc*0.01f;
							if(fm_gain  < 0.01f)fm_gain =0.01f;
						}
						//else
						if(calc < sd.fm_offset)
						{
							calc = sd.fm_offset -calc;
							calc = calc/ sd.fm_offset;
							fm_gain += fm_gain * calc* 0.0001f;
							if(fm_gain >1.0f)fm_gain =1.0f;
						}
						if(ads.a_buffer[i]> sd.fm_offset)ads.a_buffer[i] *= fm_gain;
					}
					//arm_fir_interpolate_f32(&INTERPOLATE_Q,(float32_t *)ads.a_buffer, (float32_t *)ads.b_buffer, psize);
					/*	
					*	FM调制公式  S(n) = A*Cos[2πLO*n/k+sin[2π*AF*n/k]
				    *					= Cos[2π((X(n)+X(n-1))/k]*Cos[2π*LO1*n/k]
					*				      -Sin[2π((X(n)+X(n-1))/k]*Sin[2π*LO1*n/k]
					*/
					softdds_runf(DDS_TX_LO,ads.ddslo_i_tx ,ads.ddslo_q_tx, SAMPLE_SIZE/2);//SSB本振
					for(i=0;i<psize;i++)
					{
						A++;if(A>SAMPLE_SIZE/2)A=0;
						/* (X(n)+X(n-1) */
						Data_FM_I[i] = ads.a_buffer[i]+FM_I[1];

						Data_I1[i] = arm_cos_f32(PI_CALC*Data_FM_I[i]*A/8);
						Data_Q1[i] = arm_sin_f32(PI_CALC*Data_FM_I[i]*A/8);
						 
						FM_I[1] =ads.a_buffer[i];
					}
					/* 内插低通滤波以限制带宽 */
					arm_fir_interpolate_f32(&INTERPOLATE_I,(float32_t *)Data_I1, (float32_t *)ads.a_buffer, psize);
					arm_fir_interpolate_f32(&INTERPOLATE_Q,(float32_t *)Data_Q1, (float32_t *)ads.b_buffer, psize);
					for(i=0;i<SAMPLE_SIZE/2;i++)
					{
						Data_I1[i] = ads.a_buffer[i]*ads.ddslo_i_tx[i] - ads.b_buffer[i]*ads.ddslo_q_tx[i];
						Data_I1[i] *= ads.cw_gain[1]*fm_alc_gain*8;
					}
					/* 
					*	Hilbert Transform 
					*/
					if(ads.rx_delay<1)
					{
						arm_fir_f32(&Hilbert_I,Data_I1,Data_I,SAMPLE_SIZE/2);
						arm_fir_f32(&Hilbert_Q,Data_I1,Data_Q,SAMPLE_SIZE/2);
					}
					/*
					*	ALC
					*/
					//fm_alc_gain = 1.0f;
					if(sd.menu_page ==1)sd.Pow =50;
					calc = ads.pow;/* ALC取样 */
					if(calc<0)calc*=-1;
					if(calc > sd.Pow )
					{
						calc = calc - sd.Pow;
						calc = calc / sd.Pow;
						arm_sqrt_f32 (calc, (float32_t *)&calc );
						fm_alc_gain -= fm_alc_gain * calc * 0.01f;
						if(fm_alc_gain  <0.01f )fm_alc_gain = 0.01f;
					}
					//else
					if(calc < sd.Pow)	
					{
						calc =  sd.Pow -calc;
						calc = calc/ sd.Pow;
						arm_sqrt_f32 (calc, (float32_t *)&calc );
						fm_alc_gain += fm_alc_gain * calc * 0.001f;
						if(fm_alc_gain >1.0f)fm_alc_gain = 1.0f;
					}
//					for(i=0;i<SAMPLE_SIZE/2;i++)
//					{	
//						Data_I[i] = Data_I1[i]*cw_alc_gain;//
//						Data_Q[i] = Data_Q1[i]*cw_alc_gain;//
//					}
					//Dac2_out_I[i] = (u16)agc_gain;
					ads.tx_delay = 200;/* 接收延迟时间设置 */
				}
			}
			if(ks.cat_ptt ==TX||ks.rit_tx==TX)
			{
				PTT_OUT();
				PTT_RT_VOX(TX);
			}
		}
		/*
		*	TX I/Q幅度相位调整
		*/
		if(TR_READ==CONTROL_TX)
		{
			arm_scale_f32(Data_I,ads.tx_amp[vfo[VFO_A].Band_id], Data_I,SAMPLE_SIZE/2);//调整I通道增益
			arm_scale_f32(Data_I,ads.tx_phase[vfo[VFO_A].Band_id] , ads.b_buffer,SAMPLE_SIZE/2);//取一点点I通道信号

			arm_add_f32(Data_Q, ads.b_buffer, Data_Q, SAMPLE_SIZE/2);// 取一点点I通道信号与Q相加
			//arm_sub_f32((float32_t *)Data_Q, (float32_t *)Data_Q, (float32_t *)ads.a_buffer, SAMPLE_SIZE/2);
			//arm_scale_f32(Data_I,ads.tx_amp[vfo[VFO_A].Band_id] , Data_I,SAMPLE_SIZE/2);//调整I通道增益
			if(vfo[VFO_A].Mode <  DEMOD_LSB)
			{
				for(i=0;i<SAMPLE_SIZE/2;i++)
				{
					ads.out_buff[i*2]  = Data_I[i];
					ads.out_buff[i*2+1]= Data_Q[i];
				}
			}
			else 
			{
				if(ads.rx_delay<1)
				{
					for(i=0;i<SAMPLE_SIZE/2;i++)
					{
						ads.out_buff[i*2]  = Data_I[i];
						ads.out_buff[i*2+1]= Data_Q[i];
					}
				}
			}
		}
		/*
		*	数据加载到DAC 
		*/
		
		if(DMA2->LISR&(1<<27))	//等待DMA2_Steam3传输完成
		{
			DMA2->LIFCR|=1<<27;	//清除DMA2_Steam3传输完成标志
           
			if(DMA2_Stream3->CR&(1<<19))
			{
				for(i=0;i<SAMPLE_SIZE;i++)
				{
					DAC_Buff0 [i] = (int32_t)ads.out_buff[i];
				}
			}
			else
			{
				for(i=0;i<SAMPLE_SIZE;i++)
				{
					DAC_Buff1 [i] = (int32_t)ads.out_buff[i];
				}
			}
			if(TR_READ==CONTROL_TX)
			{
				for(i=0;i<SAMPLE_SIZE/2;i++)
				{	
					if(spe.fft_flang ==0)
					{
						spe.fft_num ++;//FFT数据采样计数；
						if(spe.fft_num >= FFT_LENGTH*2)
						{
							spe.fft_num =0;//FFT数据采样计数；
							spe.fft_flang =1;
						}
						fft_buf[spe.fft_num*2] = (float32_t)ads.out_buff[i*2];
						fft_buf[spe.fft_num*2+1] = (float32_t)ads.out_buff[i*2+1];	
					}
				}
			}
		}
		if(DMA1->LISR&(1<<11))	//等待DMA2_Steam0传输完成
		{
			DMA1->LIFCR|=1<<11;	//清除DMA2_Steam1传输完成标志
			if(DMA1_Stream1->CR&1<<19)
			{
				for(i=0;i<SAMPLE_SIZE/2;i++)
				{
					DAC2_Buff0 [i] = (int16_t)ads.si_i[i]+DAC_OFFSET_LEVEL;
				}
			}
			else
			{
				for(i=0;i<SAMPLE_SIZE/2;i++)
				{
					DAC2_Buff1 [i] = (int16_t)ads.si_i [i]+DAC_OFFSET_LEVEL;
				}
			}
		}
		if(DMA1->LISR&(1<<5))	//等待DMA2_Steam0传输完成
		{
			float32_t temp[ADC_SIZE];
			float32_t temp1[ADC_SIZE];
			float32_t temp2[ADC_SIZE];
			float32_t temp3[ADC_SIZE];
			
			DMA1->LIFCR|=1<<5;	//清除DMA2_Steam1传输完成标志
			if(DMA1_Stream0->CR&1<<19)
			{
				for(i=0;i<ADC_SIZE;i++)
				{
					 temp[i] = (float32_t)ADC1_Buff0[i*4];
					temp1[i] = (float32_t)ADC1_Buff0[i*4+1];
					temp2[i] = (float32_t)ADC1_Buff0[i*4+2];
					temp3[i] = (float32_t)ADC1_Buff0[i*4+3];
				}
			}
			else
			{
				for(i=0;i<ADC_SIZE;i++)
				{
					 temp[i] =ADC1_Buff1[i*4];
					temp1[i] =ADC1_Buff1[i*4+1];
					temp2[i] =ADC1_Buff1[i*4+2];
					temp3[i] =ADC1_Buff1[i*4+3];
				}
			}
			
			arm_mean_f32(temp3,ADC_SIZE, &ads.power_v);
			ads.power_v *= 0.0038f;/* 16位ADC */
			
			arm_mean_f32(temp,ADC_SIZE, &ads.fwd);
			if(vfo[VFO_A].Mode >= DEMOD_LSB && vfo[VFO_A].Mode <= DEMOD_USB_DIG)
				ads.pow = ads.fwd *ads.pow_corre * ads.fwd *ads.pow_corre * 0.2f*(3.0f-ads.cw_gain[2])*1.5f;/* 16位ADC */
			else
				ads.pow = ads.fwd *ads.pow_corre * ads.fwd *ads.pow_corre * 0.2f*(3.0f-ads.cw_gain[2]);/* 16位ADC */
			//ads.pow = ads.fwd * 0.01221f * ads.fwd *0.01221f * 0.2f;/* 12位ADC */
			arm_mean_f32(temp1,ADC_SIZE, &ads.ref);
			ads.pwr_rev = ads.ref * 0.00068f * ads.ref *0.00068f * 0.2f*(3.0f-ads.cw_gain[2]);/* 16位ADC */
			//arm_sqrt_f32(ads.ref/ads.pow,(float32_t *)&calc);
			
			if(ads.pow>1)ads.swr =(u8)((1+ads.ref/ads.fwd)/(1-ads.ref/ads.fwd)*10);/* 驻波计算 */
			//if(ads.pow>1)ads.swr=(1+sqrtf(ads.ref*ads.ref/(ads.fwd*ads.fwd)))/(1-sqrt(ads.ref*ads.ref/(ads.fwd*ads.fwd)))*10;
			arm_mean_f32(temp2,ADC_SIZE, &ads.vox);
			
			UART3_Task();
			UART_Task();
		}
		
	}
}
/*
*	DMA1->Stream5中断程序
*	DSP加工处理完的数据通过DMA传输到DAC1的DAC1->DHR12R1;
*	采用双缓存以防数据丢失；
*/
void DMA1_Stream0_IRQHandler (void) 
{
	//u16 i;
	if(DMA1->LISR&(1<<5))	//等待DMA2_Steam0传输完成
	{
		DMA1->LIFCR|=1<<5;	//清除DMA2_Steam1传输完成标志
		if(DMA1_Stream0->CR&1<<19)
		{
			ads.fwd = (ADC1_Buff0[0] + ADC1_Buff0[4] + ADC1_Buff0[8] + ADC1_Buff0[12])>>2;
			ads.ref = (ADC1_Buff0[1] + ADC1_Buff0[5] + ADC1_Buff0[9] + ADC1_Buff0[13])>>2;
			ads.vox = (ADC1_Buff0[2] + ADC1_Buff0[6] + ADC1_Buff0[10] + ADC1_Buff0[14])>>2;
			ads.power_v = (ADC1_Buff0[3] + ADC1_Buff0[7] + ADC1_Buff0[11] + ADC1_Buff0[15])>>2;
		}
		else
		{
			ads.fwd = (ADC1_Buff1[0] + ADC1_Buff1[4] + ADC1_Buff1[8] + ADC1_Buff1[12])>>2;
			ads.ref = (ADC1_Buff1[1] + ADC1_Buff1[5] + ADC1_Buff1[9] + ADC1_Buff1[13])>>2;
			ads.vox = (ADC1_Buff1[2] + ADC1_Buff1[6] + ADC1_Buff1[10] + ADC1_Buff1[14])>>2;
			ads.power_v = (ADC1_Buff1[3] + ADC1_Buff1[7] + ADC1_Buff1[11] + ADC1_Buff1[15])>>2;
		}
		ads.power_v *= 0.00383f;/* 16位ADC */
		//ads.power_v *= 0.00513f;/* 16位ADC 电压检测*/
		//ads.power_v *= 0.0615f;/* 12位ADC */
		ads.pow = ads.fwd * 0.001f * ads.fwd *0.001f * 0.2f;/* 16位ADC */
		//ads.pow = ads.fwd * 0.01221f * ads.fwd *0.01221f * 0.2f;/* 12位ADC */
		if(ads.pow>1)ads.swr = (ads.fwd*ads.fwd + ads.ref*ads.ref*1.4f)*10
								/(ads.fwd*ads.fwd - ads.ref*ads.ref*1.4f);/* 驻波计算 */
		//arm_sqrt_f32(ads.ref/ads.pow,(float32_t *)&calc);
		UART3_Task();
		UART_Task();
	}
}
/*
********************************************************************************
* 频谱及瀑布图显示
********************************************************************************
*/
typedef struct
{
	u16 x;/* 起点X*/
	u16 y;/* 起点Y*/
	//u8 font;
	char *text;/* 显示的文本 */
}SPE_Text_typedef;
const SPE_Text_typedef	Spe_text[11] =
{
	{  10,222,"5"},
	{  40,222,"4"},
	{  70,222,"3"},
	{ 100,222,"2"},
	{ 130,222,"1"},
	{ 160,222,"0"},
	{ 190,222,"1"},
	{ 220,222,"2"},
	{ 250,222,"3"},
	{ 280,222,"4"},
	{ 310,222,"5"},
};
/*
*********************************************************************************************************
*  函 数 名: Power_Phase_Radians
*  功能说明: 求相位
*  形 参：_usFFTPoints FFT点数， uiCmpValue 阀值
*  返 回 值: 无
*********************************************************************************************************
*/
void Power_Phase_Radians(uint16_t _usFFTPoints,float32_t* intput_i,float32_t* intput_q, float32_t uiCmpValue)
{
	int16_t IX, IY,QX,QY;
	uint16_t i;
	float32_t phase_i,phase_q;
	float32_t mag_i,mag_q;
	for (i=0; i <_usFFTPoints; i++)
	{
		IX= intput_i[i*2]; /* 实部 */
		IY= intput_i[i*2+1]; /* 虚部 */
		QX= intput_q[i*2]; /* 实部 */
		QY= intput_q[i*2+1]; /* 虚部 */
		phase_i = atan2(IY, IX); /* atan2求解的结果范围是(-pi, pi], 弧度制 */
		phase_q = atan2(QY, QX); /* atan2求解的结果范围是(-pi, pi], 弧度制 */
		arm_sqrt_f32((float32_t)(IX*IX+ IY*IY), &mag_i); /* 求模 */
		arm_sqrt_f32((float32_t)(QX*QX+ QY*QY), &mag_q); /* 求模 */
		if(uiCmpValue > mag_i)
		{
			spe.Phase_i[i] = 0;
		}
		else
		{
			spe.Phase_i[i] = phase_i* 180.0f/3.1415926f; /* 将求解的结果由弧度转换为角度 */
		}
		if(uiCmpValue > mag_q)
		{
			spe.Phase_q[i] = 0;
		}
		else
		{
			spe.Phase_q[i] = phase_i* 180.0f/3.1415926f; /* 将求解的结果由弧度转换为角度 */
		}
	}
}
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

float input[1024];
int16_t  falls[60][320];
void Spectrum_Dispaly(u16 x,u16 y,u16 width,u16 falls_h)
{
	
	 u16 x0,x1,y0;
	 u16 fc,bc,fall_bc;
	 u16 k;
	
	//static float32_t input[512];
	//static  uint16_t  falls[20][320];
	static u16 h;
	 u16 i;
	//static u8 m;
	//static float32_t Carrier[60];
	//static float32_t Carrier_max[20];
	//static float32_t Carrier_Strength;
	//static float32_t FFT_out[FFT_LENGTH];
	static float32_t buff_lsb[FFT_LENGTH];
	//static float32_t buff_usb[FFT_LENGTH];
	static float32_t out_lsb[FFT_LENGTH*2];
	//static float32_t out_usb[FFT_LENGTH*2];

	if(spe.fft_flang !=0)
	{			
		for(i=0;i<FFT_LENGTH;i++)
		{
			/* 正玄窗 */
			buff_lsb[i] = fft_buf [i*2] * arm_sin_f32( PI*2 *i/FFT_LENGTH);
			//buff_usb[i] = fft_buf [i*2+1] ;//* arm_sin_f32( PI*2 *i/FFT_LENGTH);
			/*	Hanning 汉宁窗 1.36 */
			//spe_buf [i*2] = (float32_t)((1.0f-arm_cos_f32(PI*2.0f * (float32_t)i / 1023.0f)) * fft_buf [i]);
			/* Blackman 布莱克曼窗 1.75 */
			//spe_buf [i*2] =(0.42659f - (0.49656f*arm_cos_f32((2.0f*PI*(float32_t)i)/FFT_LENGTH)) + (0.076849f*arm_cos_f32((4.0f*PI*(float32_t)i)/FFT_LENGTH))) * fft_buf [i];
			//spe_buf [i*2+1] =0;//fft_buf [i]*arm_cos_f32(2.0f*PI*0.01f*(float32_t)(i/96000));
		}
		arm_rfft_fast_f32(&FFT_LSB, buff_lsb, out_lsb,0 );	//FFT计算（基4)
		//arm_rfft_fast_f32(&FFT_USB, buff_usb, out_usb,0 );	//FFT计算（基4)
		/***取出FFT模值***/
		arm_cmplx_mag_f32(  out_lsb,buff_lsb,FFT_LENGTH/2);
		//arm_cmplx_mag_f32(  out_usb,buff_usb,FFT_LENGTH/2);
		/*	相位提取 */
		//Power_Phase_Radians(FFT_LENGTH/2,out_lsb,out_usb, 0.1f);
		//Power_Phase_Radians(FFT_LENGTH/2,out_usb,out_usb, 0.1f);
		/*
		*	FFT取模点数512点，步进93.75Hz
		*	1/93.75=0.010667f
		*	LCD屏320点,步进125Hz；缩放系数1.3333f,
		*	1/150=0.006667f
		*/
		for(i=0;i<384;i++)
		{ 
			spe_buf[i] = buff_lsb[(u16)(i*1.3333f)];
		}
		//
		if(++h >falls_h-1)h =0;	
		for(k=0;k<width;k++)
		{
			
			x0=sd.Dsp_Bfo*0.008f + Fil_p[sd.spe_fil_id].spe_width/2;
			x1=sd.Dsp_Bfo*0.008f - Fil_p[sd.spe_fil_id].spe_width/2;
			if(sd.spe_fil_mode>0)//LSB
			{
				if((k+x)>=(SPE_CENTER - x0) && (k+x)<(SPE_CENTER-x1 ))
				{
					bc=GREEN3;
					//fc=GREEN5 |(u16)input[k];//BLUE1 |YELLOW2;
					fc= GREEN3;
					fall_bc = GREEN3;
				}
				else
				{
					bc=BLACK;
					fc=BLUE |YELLOW2;
					fall_bc = 0;
				}
			}
			else 
			if(sd.spe_fil_mode==0)//AM/FM/CW
			{
				if((k+x)>=(SPE_CENTER - Fil_p[sd.spe_fil_id].spe_width/2) && (k+x)<=(SPE_CENTER+Fil_p[sd.spe_fil_id].spe_width/2))
				{
					bc=GREEN3;
					//fc=GREEN5 |(u16)input[k];//BLUE1 |YELLOW2;
					fc= GREEN3;
					fall_bc = GREEN3;
				}
				else 
				{
					bc=BLACK;
					fc=BLUE |YELLOW2;
					fall_bc = 0;
				}
			}
			else
			if(sd.spe_fil_mode<0)//USB
			{
				if((k+x)>=(SPE_CENTER + x1) && (k+x)<=(SPE_CENTER + x0))
				{
					bc=GREEN3;
					//fc=GREEN5 |(u16)input[k];//BLUE1 |YELLOW2;
					fc=GREEN3;
					fall_bc = GREEN3;
				}
				else 
				{
					bc=BLACK;
					fc=BLUE |YELLOW2;
					fall_bc = 0;
				}
			}	
			
			/* 旧波形清楚 */
			for(i=(u16)input[k]&0x3f;i>0;i--)
			{
				Gui_DrawPoint(k+x,y-i-falls_h-1,bc);	
			}
			/* 新波形数据提取 */
			input[k] =  spe_buf[k  + (u16)(sd.IF_1*0.008f) -157 ]*0.5f 
					  + spe_buf[k-1+ (u16)(sd.IF_1*0.008f) -157 ]*0.25f 
					  + spe_buf[k+1+ (u16)(sd.IF_1*0.008f) -157 ]*0.25f;
			input[k]*=(k/30+5)*0.1f;		 
			if(TR_READ==CONTROL_TX)falls[h][k] = (u32)(input[k])>>20;// 
			else falls[h][k] = (u32)(input[k])>>( 17+sd.Riss/16);//
			//speput[k]=(u8)(log2((u32)input[k]))*8;
			input[k]*=0.01f;
			arm_sqrt_f32( input[k],(float32_t *)&input[k]);
			arm_sqrt_f32( input[k],(float32_t *)&input[k]);
			input[k] -= 3;
			if(input[k]>48 )input[k] =48;
			if(k>250 && k<320)
			{
				if(input[k]>spe.fft_max)spe.fft_max  = (u8)input[k];
			}
			/* 绘制波形 */	
			if(k>0)
			{
				/* 中线标识显示 */
				//if(k==width/2 && i< (u16)input[k])fc=YELLOW3;
				//else
				//fc=BLUE;
				/* 频谱显示 */
				//if(k==width/2 && (u16)input[k]< 40)input[k]=40;
				
				for(i = ((u16)input[k]);i>0;i--)
				{
					//if(k==width/2 && i<= 40)fc=YELLOW;
					//else
					//fc=BLUE |YELLOW2;
					Gui_DrawPoint(k+x,y-i-falls_h-1, (BLUE << (i>>2))|fc);	
				}
//				/* 瀑布图显示 */
				if(sd.amp_phase ==0&&ks.IF_shift ==0)
				{
					for(i=0;i<falls_h;i++)
					{
						if(i<=h)
						Gui_DrawPoint(k+x,y+i-falls_h, (falls[h-i][k]*falls[h-i][k]) | fall_bc);
						else 
						if(i>h)
						Gui_DrawPoint(k+x,y+i-falls_h, (falls[falls_h+h-i][k]*falls[falls_h+h-i][k]) | fall_bc);
					}
					if((k+x)>1&&(k+x)<20)LCD_ShowString(2,182,GRAY3,BLUE3 ,12,1,"20k");
					if((k+x)>156&&(k+x)<163)LCD_ShowString(157,182,GRAY3,BLUE3 ,12,1,"0");
					if((k+x)>301&&(k+x)<319)LCD_ShowString(302,182,GRAY3,BLUE3 ,12,1,"20k");
				}
			}
			/* 瀑布图更新时钟 */
			if(k==0)
			{
				if(ads.vol_timer)sd.spe_fil_time = SPE_FIL_TIME;//频谱带宽显示回复计时标志
				//if(sd.spe_fil_time>0)sd.spe_fil_time--;if(sd.spe_fil_time<1)sd.spe_fil_time=0;
				if(ads.vol_timer ==0)
				{
					ui_Spe_Filter_Display(sd.spe_fil_id,sd.spe_fil_mode,sd.spe_fil_time,GREEN3,ks.rx_adjuset_key );
					
					
					for(y0=130;y0<182;y0+=17)
					{
						for(i=1;i<317;i++) Gui_DrawPoint(i,y0, GRAY4);
					}
					for(i=1;i<320;i+=79)
					{
						for(y0=130;y0<180;y0+=1)Gui_DrawPoint(i,y0, GRAY4);
					}
				
					if(sd.spe_fil_time>0)sd.spe_fil_time--;if(sd.spe_fil_time<1)sd.spe_fil_time=0;
				
					LCD_ShowNum(265,131,GREEN,BLACK, 3,16,ads.agc_gain);
				}
				if(ks.IF_shift ==1)
				{
					LCD_ShowString(30,182,YELLOW,BLUE1, 16, 0,"  IQ_BL   " );
					LCD_ShowNum(220,182,GRAY1,BLUE1, 9,16,Rx_amp_user[ads.rx_amp_band]);
				}
//				if(clk0==0)
//				{
//					LCD_ShowString(257,130,YELLOW ,BLACK , 16, 0, "-");
//					LCD_ShowString(290,130,YELLOW ,BLACK , 16, 0, "db");
//					clk0=1;
//				}
			}
		}
		spe.fft_flang =0;
	}
}

//外部中断0服务程序
void EXTI15_10_IRQHandler(void)
{ 
	//if(CW_K_DI==0)EXTI_D1->PR1=1<<10;  //清除LINE0上的中断标志位 
	//if(CW_K_DA==0)EXTI_D1->PR1=1<<11;  //清除LINE0上的中断标志位 	
	/*
	*	di/da键转换
	*/
//	if(ks.CW_AB ==1)
//	{
//		cw_Auto_key.dit = CW_K_DI;
//		cw_Auto_key.dah = CW_K_DA;
//	}
//	else  
//	{
//		cw_Auto_key.dit = CW_K_DA;
//		cw_Auto_key.dah = CW_K_DI;
//	}
	//
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 && cw_Auto_key.dah ==0)/* 自动键模式 di滴键和哒键都按下 */
//	{
//		cw_Auto_key.test_state =3;
//		//tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
//	}
//	else 
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 )//&& cw_Auto_key.dah ==1)/* 自动键模式 di滴键按下 */
//	{
//		cw_Auto_key.test_state =2;
//		//tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
//	}
//	else					
//	if(PTT_RT == 1 /*&& cw_Auto_key.dit == 1*/ && cw_Auto_key.dah ==0)/* 自动键模式 da哒键按下 */
//	{
//		cw_Auto_key.test_state =1;
//		//tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
//	}
//	else
//	if(PTT_RT == 1 && cw_Auto_key.dit == 1 && cw_Auto_key.dah ==1)/* 自动键模式 da哒键和哒键都松开 */
//	{
//		cw_Auto_key.test_state =0;
//	}
	
//	if(cw_Auto_key.now_state != cw_Auto_key.test_state )cw_Auto_key.now_state = cw_Auto_key.test_state;
	//
	if(ks.rt_rx ==0)
	{
//		if((CW_K_DI==0 || CW_K_DA==0 )&&vfo[VFO_A].Mode < DEMOD_LSB)
//		{
//			//WM8978_MIC_Gain(0);
//			WM8978_Output_Cfg(1,1);
//		}
		if(PTT_RT == TX&& vfo[VFO_A].Mode >= DEMOD_LSB )
		{
			ads.spk_vol =0;
			ads.hp_vol =0;
			WM8978_SPKvol_Set(0);
			WM8978_HPvol_Set(0,0);
		}
	}
	//si5351aSetFrequency_LoA(sd.Tcxo, (vfo[VFO_A].Freq - ads.dsp_IF + sd.rit)*4);
	//if(CW_K_DI==0)
		//EXTI_D1->PR1=1<<10;  //清除LINE0上的中断标志位 
	//if(CW_K_DA==0)
		//EXTI_D1->PR1=1<<11;  //清除LINE0上的中断标志位
	//if(PTT_RT == 0)
		EXTI_D1->PR1 |=1<<14;
//	EXTI_D1->PR1=1<<11;  //清除LINE2上的中断标志位   
//	/*
//	*	di/da键转换
//	*/
//	if(ks.CW_AB ==1)
//	{
//		cw_Auto_key.dit = CW_K_DI;
//		cw_Auto_key.dah = CW_K_DA;
//	}
//	else  
//	{
//		cw_Auto_key.dit = CW_K_DA;
//		cw_Auto_key.dah = CW_K_DI;
//	}
//	//
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 && cw_Auto_key.dah ==0)/* 自动键模式 di滴键和哒键都按下 */
//	{
//		cw_Auto_key.now_state =3;
//		//tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
//	}
//	else 
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 && cw_Auto_key.dah ==1)/* 自动键模式 di滴键按下 */
//	{
//		cw_Auto_key.now_state =2;
//		//if(cw_Auto_key.now_state ==2)
//		//{
//		//	if(cw_Auto_key.dah ==0)
//		//		cw_Auto_key.mm_state =3;
//		//};
//		//tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
//	}
//	else					
//	if(PTT_RT == 1 && cw_Auto_key.dit == 1 && cw_Auto_key.dah ==0)/* 自动键模式 da哒键按下 */
//	{
//		cw_Auto_key.now_state =1;
//		//if(cw_Auto_key.now_state ==1)
//		//{
//		//	if(cw_Auto_key.dit == 0)
//		//		cw_Auto_key.mm_state =3;
//		//};
//		//tx_delay = sd.CW_Delay;/* 接收延迟时间设置 */
//	}
//	else
//	if(PTT_RT == 1 && cw_Auto_key.dit == 1 && cw_Auto_key.dah ==1)/* 自动键模式 da哒键和哒键都松开 */
//	{
//		cw_Auto_key.now_state =0;
//	}	 
}






