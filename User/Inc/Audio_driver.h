#ifndef __AUDIO_DRIVER_H
#define __AUDIO_DRIVER_H	
#include "sys.h"
#include "arm_math.h" 
/*
******************************************************************************
*
******************************************************************************
*/
#define SAMPLE_SIZE			192	/* �������� */
#define SAMPLING_RETE		96000	/* �����ٶ� 192kHz*/
//#define DSP_RETE			SAMPLING_RETE/Sample_size /* DSP��Ƶ�����ٶ� */

#define DSP_IF 				(SAMPLING_RETE/4) /* ������Ƶ */

#define PI_CALC				(2*PI/SAMPLING_RETE)	/* �������ֱ����õ��ĺ� */

#define TIM_ARR			((SYSTEM_CLK/2)/SAMPLING_RETE-1)	/* ��ʱ��6�ĳ�װֵ */

#define BUFF_LEN 			512		/* ������������ */

#define DECIMATION_RATE 	8		/* ��ȡϵ�� */
//
// -----------------------------
// Half of total buffer
#define	IQ_BUFSZ 	(BUFF_LEN)

// Audio filter

#define IIR_NUM 			6 			/* IIR�˲���ϵ������ */
#define IIR_NUM_TAPS		IIR_NUM*4	/* IIR�˲���״̬�������� */
#define IIR_BLOCK_SIZE		SAMPLE_SIZE/2	/* IIR�˲����������� */

#define FIR_BLOCK_SIZE		SAMPLE_SIZE/2	/* FIR�˲����������� */	
#define FIR_NUM_TAPS		96			/* FIR�˲���ϵ������ */
/*******************************************************************************/
//
#define ADC_SIZE					 SAMPLE_SIZE/4
#define ADC_16LEN					16
#define DAC_OFFSET_LEVEL			2048	// DAC���ƫ�� 
//
#define AGC_GAIN_MAX_AM				1.0f
#define AGC_GAIN_MAX_FM				10000.0f
#define AGC_GAIN_MAX				1.0f

#define AGC_THRESHOLD				65536
#define AGC_SCALE					1.0f/(float32_t)AGC_THRESHOLD

#define AM_OUT_GAIN					4

#define RISS_SSB_THRESHOLD			4
#define RISS_SSB_SCALE				1.0f/(float32_t)RISS_SSB_THRESHOLD
#define RISS_AMFM_THRESHOLD			65536*2
#define RISS_AMFM_SCALE				1.0f/(float32_t)RISS_AMFM_THRESHOLD

#define	AGC_SLOW					0.01f  // Mode setting for slow AGC 	AGC�ָ�ʱ��3��
#define	AGC_MED				        0.033f	// Mode setting for medium AGC  AGC�ָ�ʱ��1��
#define	AGC_FAST					0.1f	// Mode setting for fast AGC	 AGC�ָ�ʱ��0.1��
#define	AGC_CUSTOM					0.25f	// Mode setting for custom AGC

#define	RISS_SLOW					0.1f  // Mode setting for slow AGC 	AGC�ָ�ʱ��3��
#define	RISS_MED					0.01f	// Mode setting for medium AGC  AGC�ָ�ʱ��1��
#define	RISS_FAST					0.001f	// Mode setting for fast AGC	 AGC�ָ�ʱ��0.1��
//#define	AGC_CUSTOM					0.25f	// Mode setting for custom AGC

#define CW_POW_GAIN_MAX				1500
//
// FM Demodulator parameters
//
#define	FM_OFFSET_2K5			4000			// Amplitude scaling factor of demodulated FM audio (normalized for +/- 2.5 kHz deviation at 1 kHZ)
#define FM_OFFSET_5K			7000
#define	FM_RX_SCALING_2K5		65536*8			// Amplitude scaling factor of demodulated FM audio (normalized for +/- 2.5 kHz deviation at 1 kHZ)
#define FM_RX_SCALING_5K		(FM_RX_SCALING_2K5/2)	// 
#define	FM_DEMOD_COEFF1			PI/4			// Factors used in arctan approximation used in FM demodulator
#define	FM_DEMOD_COEFF2			PI*0.75
// ***************************************************************************************************************************
// DSP system parameters
//
// Noise reduction
//
#define	LMS_NR_DELAYBUF_SIZE_MAX		512	// maximum size of LMS delay buffer for the noise reduction
//
//
#define	DSP_NR_STRENGTH_MAX		55//35	// Maximum menu setting for DSP "Strength"
#define	DSP_NR_STRENGTH_DEFAULT	15	// Default setting
//
#define	DSP_STRENGTH_YELLOW		25	// Threshold at and above which DSP number is yellow
#define	DSP_STRENGTH_ORANGE		35	// Threshold at and above which DSP number is orange
#define DSP_STRENGTH_RED		45	// Threshold at and above which DSP number is red
//
//
#define	DSP_NR_BUFLEN_MIN		16		// minimum length of de-correlation buffer on the LMS NR DSP
#define	DSP_NR_BUFLEN_MAX		LMS_NR_DELAYBUF_SIZE_MAX	// maximum length of de-correlation buffer on the LMS NR DSP
#define	DSP_NR_BUFLEN_DEFAULT	256		// default length of de-correlation buffer on the LMS NR DSP
//
#define DSP_NR_NUMTAPS_MIN		32		// minimum number of FIR taps in the LMS NR DSP
#define	DSP_NR_NUMTAPS_MAX		128		// maximum number of FIR taps in the LMS NR DSP
#define	DSP_NR_NUMTAPS_DEFAULT	96		// default number of FIR taps in the LMS NR DSP
//
// Automatic Notch Filter
//
#define	LMS_NOTCH_DELAYBUF_SIZE_MAX	512
//
#define	DSP_NOTCH_NUMTAPS_MAX	128
#define	DSP_NOTCH_NUMTAPS_MIN		32
#define	DSP_NOTCH_NUMTAPS_DEFAULT	96
//
#define	DSP_NOTCH_BUFLEN_MIN	32		// minimum length of decorrelation buffer for the notch filter FIR
#define	DSP_NOTCH_BUFLEN_MAX	192	// maximum decorrelation buffer length for the notch filter FIR
#define	DSP_NOTCH_DELAYBUF_DEFAULT	104	// default decorrelation buffer length for the notch filter FIR
//
#define	DSP_NOTCH_MU_MAX		40		// maximum "strength" (convergence) setting for the notch
#define	DSP_NOTCH_MU_DEFAULT	25		// default convergence setting for the notch
//
#define	MAX_DSP_ZERO_COUNT		2048
#define	DSP_ZERO_COUNT_ERROR	512
#define	DSP_ZERO_DET_MULT_FACTOR	10000000	// work-around because the stupid compiler wouldn't compare fractions!
#define	DSP_OUTPUT_MINVAL		1		// minimum out level from DSP LMS NR, indicating "quiet" crash
#define	DSP_HIGH_LEVEL			10000	// output level from DSP LMS NR, indicating "noisy" crash
#define	DSP_CRASH_COUNT_THRESHOLD	35	// "hit" detector/counter for determining if the DSP has crashed

#define SPE_FIL_TIME			1//Ƶ�״�����ʾ�ָ���ʱ
#define SPE_CENTER			159//Ƶ�״�����ʾ�ָ���ʱ

#define SIGNAl_SET			127//�źű�У׼
//
/* ��Ƶ�ṹ�� */
typedef struct
{
	u16 tx_delay:16;
	u16 cw_spe:16;
	u8  Rate:8;			//�ڲ�ͳ�ȡ����	
	u16 cw_time:16;
	u8  cw_time_start:8;
	float32_t dsp_IF;

	float32_t si_i[SAMPLE_SIZE/2];/* DSP_LO����I */
	float32_t si_q[SAMPLE_SIZE/2];/* DSP_LO����Q */

	float32_t ddslo_i_tx[SAMPLE_SIZE/2];/* DSP_LO����I */
	float32_t ddslo_q_tx[SAMPLE_SIZE/2];/* DSP_LO����Q */

	float32_t ddslo_i[SAMPLE_SIZE/2];/* DSP_LO����I */
	float32_t ddslo_q[SAMPLE_SIZE/2];/* DSP_LO����Q */

	float32_t ddsbfo_i[SAMPLE_SIZE/8];/* DSP_BFO����I */
	float32_t ddsbfo_q[SAMPLE_SIZE/8];/* DSP_BFO����Q */

	float32_t ddssi_i[SAMPLE_SIZE/2];/* CW����I */
	float32_t ddssi_q[SAMPLE_SIZE/2];/* CW����Q */

	float32_t cw_gain[3];

	/* AUdio */
	float32_t	a_buffer[BUFF_LEN/2];
	float32_t	b_buffer[BUFF_LEN/2];
	int32_t	out_buff[BUFF_LEN];

	/* AGC */
	float32_t	agc_constant;
		u32 	agc_gain;
		u8 IF_gain;

	/* TX */
	u16		rx_delay:16;
	float32_t 	tx_amp[10];
	float32_t 	tx_phase[10];
	float32_t 	rx_amp[10];
	float32_t 	rx_phase[10];
	float32_t 	amp_adjust[12];
			u8 	amp_adjust_num;
			u8  rx_amp_band;
	float32_t 	vox;
	float32_t 	fwd;
	float32_t	ref;
	float32_t	pow;
	float32_t	pow_corre;
	float32_t	pwr_rev;
	float32_t	swr;
	float32_t   power_v;
		u16	pow_gain[10];
		u16 cw_alc_gain[10];
		u16 cw_gain_time;
		u8  vol_timer:8;
		u8  alc:8;
		u16  Signal_table:16;
		u16 Riss:16;
		u8  spk_vol:8;
		u8  hp_vol:8;

		u8	Af_mute:2;
		u8  cw_fant:2;

    uint32_t				dsp_zero_count;			// used for detecting zero output from DSP which can occur if it crashes
    float					dsp_nr_sample;	
} Audio_Digital_Signal;

extern  Audio_Digital_Signal	ads;

#define DSP_NR_ENABLE			0x01
#define DSP_NR_POSTAGC_ENABLE	0x02
#define DSP_NOTCH_ENABLE		0x04
#define DSP_NB_ENABLE			0x08

typedef struct 
{
    u8	dsp_active:8;					// Used to hold various aspects of DSP mode selection
    
    u8	dsp_nr_strength:8;			// "Strength" of DSP Noise reduction - to be converted to "Mu" factor
    u16	dsp_nr_delaybuf_len:16;		// size of DSP noise reduction delay buffer
    u8	dsp_nr_numtaps:8;				// Number of FFT taps on the DSP Noise reduction
    u8	dsp_notch_numtaps:8;
    u8	dsp_notch_mu:8;				// mu adjust of notch DSP LMS
    u16	dsp_notch_delaybuf_len:16;		// size of DSP notch delay buffer
	
    int32_t	dsp_inhibit;				// if TRUE, DSP (NR, Notch) functions are inhibited.  Used during power-up
    int32_t	dsp_inhibit_mute;			// holder for "dsp_inhibit" during muting operations to allow restoration of previous state
    int32_t	dsp_timed_mute;				// TRUE if DSP is to be muted for a timed amount
    u32	dsp_inhibit_timing;				// used to time inhibiting of DSP when it must be turned off for some reason
    int32_t	reset_dsp_nr;				// TRUE if DSP NR coefficients are to be reset when "audio_driver_set_rx_audio_filter()" is called

	u16 fm_Scaling:16;
} Dspnr_typedef;
//
extern 	__IO Dspnr_typedef nr;
/**/
#define FFT_LENGTH				1024	/* FFT���� */
typedef struct 
{
//#define FFT_LENGTH				1024	/* FFT���� */
	u8 fft_flang:8;
	u8 clk:8;
	float32_t Phase_i[FFT_LENGTH]; /* I��λ*/
	float32_t Phase_q[FFT_LENGTH]; /* Q��λ*/
	//float32_t fft_buf[FFT_LENGTH];
	//float32_t spe_buf[FFT_LENGTH*2];
	uint32_t fft_num;
	u8 fft_max:8;
}SpectrumProcessing_typedef;

extern SpectrumProcessing_typedef	spe;

/*
*	�Զ�������
*/
typedef struct 
{
	u8 dit;
	u8 dah;
	u8 out_state;
	u8 now_state;
	u8 test_state;
	u8 mm_state;
	u8 mm_count;
	u8 last_one;
	u8 last_auto_key;
	u8 last_key_state;
	u16 ms_time; 
}CW_Auto_key_management;
extern CW_Auto_key_management	cw_Auto_key;
/*
*	���ι��ʲ���ϵ��
*/
typedef struct 
{
	float32_t cw;
	float32_t ssb;
	float32_t am;
	float32_t fm;
}Band_power_compensation_coefficient;

extern Band_power_compensation_coefficient 	power_coefficient[];
//extern  u32 ADC_Buff0[SAMPLE_SIZE];
//extern  u32 ADC_Buff1[SAMPLE_SIZE];

void Audio_hardware_init(void);
void Audio_Filter_init(void);
void audio_DAC_wirte(void);
void Spectrum_Dispaly(u16 x,u16 y,u16 width,u16 falls_h);
u16 Get_Adc3(u8 ch);

#endif
