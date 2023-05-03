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
#include "ui_Driver.h"
#include "softdds.h"
#include "wm8978.h"
#include "sai.h"
#include "usart.h"
#include "Si5351AI2C.h"
/*
 **************************************************************
 ���������ѧϰ�ο���δ��������ɣ��������������κ���;��

 ���ƣ�Audio_drtver.c
 ��;��FX-4C;
 ���ܣ���Ƶ������
 �汾��V1.0;
 ���ߣ���������BG2IXD��;
 ʱ�䣺2020/9/19��
 QQ:  1413894726
 **************************************************************
 */
/* �汾���� */
#define FX_4C_V_1_0

__attribute__((section (".RAM_D1")))  u16 ADC1_Buff0[SAMPLE_SIZE];
__attribute__((section (".RAM_D1")))  u16 ADC1_Buff1[SAMPLE_SIZE];
__attribute__((section (".RAM_D1")))  u32 ADC_Buff0[SAMPLE_SIZE];
__attribute__((section (".RAM_D1")))  u32 ADC_Buff1[SAMPLE_SIZE];
__attribute__((section (".RAM_D1")))  u32 DAC_Buff0[SAMPLE_SIZE];
__attribute__((section (".RAM_D1")))  u32 DAC_Buff1[SAMPLE_SIZE];
__attribute__((section (".RAM_D1")))  u16 DAC2_Buff0[SAMPLE_SIZE / 2];
__attribute__((section (".RAM_D1")))  u16 DAC2_Buff1[SAMPLE_SIZE / 2];
//
float32_t lms1_nr_delay[LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN];
//
float32_t errsig1[64 + 10];
float32_t errsig2[64 + 10];
float32_t result[64 + 10];
//
// LMS Filters for RX
__IO Dspnr_typedef nr;

arm_lms_norm_instance_f32 lms1Norm_instance;
arm_lms_instance_f32 lms1_instance;
float32_t lms1StateF32[LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN];
float32_t lms1NormCoeff_f32[LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN];
//
arm_lms_norm_instance_f32 lms2Norm_instance;
arm_lms_instance_f32 lms2_instance;
float32_t lms2StateF32[DSP_NOTCH_NUMTAPS_MAX + BUFF_LEN];
float32_t lms2NormCoeff_f32[DSP_NOTCH_NUMTAPS_MAX + BUFF_LEN];
//
float32_t lms2_nr_delay[LMS_NOTCH_DELAYBUF_SIZE_MAX + BUFF_LEN];
//
/* IIR��Ƶ�˲��� */
arm_biquad_casd_df1_inst_f32 IIR_AF;
float32_t AF_State[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
float32_t AF_Values;
/* IIR TX IF�˲��� */
arm_biquad_casd_df1_inst_f32 IIR_IF_I;
arm_biquad_casd_df1_inst_f32 IIR_IF_Q;
float32_t IF_State_I[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
float32_t IF_State_Q[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
float32_t IF_Values;
/* IIR TX�˲��� */
arm_biquad_casd_df1_inst_f32 IIR_TX_I;
arm_biquad_casd_df1_inst_f32 IIR_TX_Q;
float32_t TX_State_I[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
float32_t TX_State_Q[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
float32_t TX_Values;
/*	Riss��ͨ */
arm_biquad_casd_df1_inst_f32 RISS;
float32_t Riss_State[IIR_NUM_TAPS]; /* ״̬���棬��СnumStages*4 */
float32_t Riss_Values;
/* IIR���ߴ��˲��� */
arm_biquad_casd_df1_inst_f32 IIR_LPF_I;
arm_biquad_casd_df1_inst_f32 IIR_LPF_Q;
float32_t IIRStateF32_I[IIR_NUM_TAPS]; /* ͬ�� I ͨ��״̬���棬��СnumStages*4 */
float32_t IIRStateF32_Q[IIR_NUM_TAPS]; /* ͬ�� Q ͨ��״̬���棬��СnumStages*4 */
float32_t IIR_LP_Values; /* IIR�˲�������ϵ�� */

/* RXϣ�����ر任�˲���*/
arm_fir_instance_f32 Hilbert_I;
arm_fir_instance_f32 Hilbert_Q;
float32_t Hilbert_State_I[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];
float32_t Hilbert_State_Q[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];

/* ��ȡFIR�˲��� */
arm_fir_decimate_instance_f32 DECIMATE_I; //��ȡ�˲��� I
arm_fir_decimate_instance_f32 DECIMATE_Q; //��ȡ�˲��� Q
float32_t DecimState_I[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];/* FIR��ȡ�˲���״̬����I */
float32_t DecimState_Q[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];/* FIR��ȡ�˲���״̬����Q */

/* FIR�ڲ��˲��� */
arm_fir_interpolate_instance_f32 INTERPOLATE_I;
arm_fir_interpolate_instance_f32 INTERPOLATE_Q;
float32_t InterpState_I[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];/* FIR�ڲ��˲���״̬����I */
float32_t InterpState_Q[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];/* FIR�ڲ��˲���״̬����Q */

/* FIR�ߴ��˲��� */
arm_fir_instance_f32 FIR_LPF_I;
arm_fir_instance_f32 FIR_LPF_Q;
float32_t FIR_State_I[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];
float32_t FIR_State_Q[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];

/* FIR��ͨ�˲��� */
arm_fir_instance_f32 FIR_BPF;
float32_t FIR_State_BPF[FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1];
/**/
SpectrumProcessing_typedef spe;

arm_rfft_fast_instance_f32 Spe_FFT;
arm_rfft_fast_instance_f32 FFT_LSB;
arm_rfft_fast_instance_f32 FFT_USB;
//arm_rfft_instance_f32  FFT;
float32_t fft_buf[FFT_LENGTH * 2];
float32_t spe_buf[FFT_LENGTH * 2];
/**/

Audio_Digital_Signal ads;
CW_Auto_key_management cw_Auto_key;
//extern 	KeyManagement_TypeDef		ks;
//extern 	Storage_Data 				sd;
/*
 ******************************************************
 *	��ֵ�˲���
 ******************************************************
 */
#define MidFilterOrder  16     				/* �˲����� */
#define ZeroSize        MidFilterOrder

float32_t DstDate[SAMPLE_SIZE / 2]; /* �˲������� */

static float32_t SortData[MidFilterOrder]; /* �˲����� */
static float32_t TempDate[ZeroSize + SAMPLE_SIZE + ZeroSize] = { 0 }; /* �˲��׶��õ�����ʱ���� */
//extern  u16 Rx_amp_adjust[12];
extern u16 Rx_amp_user[10];
/*****************************************************/
/*
 *	��Ƶ����ϵͳ��ADC DAC TIM��ʼ��
 */
void Audio_hardware_init(void)
{
	Audio_Filter_init();
	WM8978_Init();

	SAIA_Init(0, 1, 6);			//SAI1 Block A,�ӷ���,24λ����
	SAIB_Init(3, 1, 6);			//SAI1 Block B��ģʽ����,24λ
	SAIA_SampleRate_Set(SAMPLING_RETE);			//���ò�����

	SAIA_TX_DMA_Init(DAC_Buff0, DAC_Buff1, SAMPLE_SIZE, 2);	//����TX DMA,32λ
	SAIB_RX_DMA_Init(ADC_Buff0, ADC_Buff1, SAMPLE_SIZE, 2);	//����RX DMA

	SAI_Rec_Start(); 			//��ʼSAI���ݽ���(�ӻ�)
	SAI_Play_Start();			//��ʼSAI���ݷ���(����)

	ADC1_DMA_Config(DMA1_Stream0, 9);
	DAC1_DMA_Config(DMA1_Stream1, 67);
	Adc1_Init();

	Dac_Init();
	TIM6_Int_Init(TIM_ARR, 0);
	DMA1_Stream0->CR |= 1 << 0;
	DMA1_Stream1->CR |= 1 << 0;
	ADC1->CR |= 1 << 0;
	ADC1->CR |= 1 << 2;       		//��������ת��ͨ��

	TIM6->CR1 |= 1 << 0;
}
/*
 *	�˲�����ʼ��
 */
void Audio_Filter_init()
{
	u16 i;
	static float mu_calc;
	arm_rfft_fast_init_f32(&Spe_FFT, FFT_LENGTH); //��ʼ��scfft�ṹ�壬le[i]=�趨FFT��ز���
	arm_rfft_fast_init_f32(&FFT_LSB, FFT_LENGTH);    //��ʼ��scfft�ṹ�壬�趨FFT��ز���
	arm_rfft_fast_init_f32(&FFT_USB, FFT_LENGTH);    //��ʼ��scfft�ṹ�壬�趨FFT��ز���

	//softdds_setfreq(800, SAMPLING_RETE, 1);
	/* IIR Filter init */
	/* AF��Ƶ��ͨ�˲� */
	for (i = 0; i < IIR_NUM_TAPS; i++)
	{
		AF_State[i] = 0;
		Riss_State[i] = 0;

		TX_State_I[i] = 0;
		TX_State_Q[i] = 0;

		IF_State_I[i] = 0;
		IF_State_Q[i] = 0;
	}

	IIR_AF.pState = (float32_t*) &AF_State;
	IIR_AF.numStages = IIR_dfI_AF.numStages;
	IIR_AF.pCoeffs = IIR_dfI_AF.pCoeffs;

	/*	TX SSB�˲���*/
	IIR_TX_I.pState = (float32_t*) &TX_State_I;
	IIR_TX_I.numStages = IIR_TX_3k_BPF.numStages;
	IIR_TX_I.pCoeffs = IIR_TX_3k_BPF.pCoeffs;

	IIR_TX_Q.pState = (float32_t*) &TX_State_Q;
	IIR_TX_Q.numStages = IIR_TX_3k_BPF.numStages;
	IIR_TX_Q.pCoeffs = IIR_TX_3k_BPF.pCoeffs;

	/*	TX IF�˲���*/

	IIR_IF_I.pState = (float32_t*) &IF_State_I;
	IIR_IF_I.numStages = IIR_30k_LPF.numStages;
	IIR_IF_I.pCoeffs = IIR_30k_LPF.pCoeffs;

	IIR_IF_Q.pState = (float32_t*) &IF_State_Q;
	IIR_IF_Q.numStages = IIR_30k_LPF.numStages;
	IIR_IF_Q.pCoeffs = IIR_30k_LPF.pCoeffs;
	//
	RISS.pState = (float32_t*) &Riss_State;
	RISS.numStages = RISS_BPF.numStages;
	RISS.pCoeffs = RISS_BPF.pCoeffs;

	IIR_LPF_I.numStages = FilterPathInfo[8].LP_instance->numStages;
	IIR_LPF_Q.numStages = FilterPathInfo[8].LP_instance->numStages;

	IIR_LPF_I.pCoeffs = FilterPathInfo[8].LP_instance->pCoeffs;
	IIR_LPF_Q.pCoeffs = FilterPathInfo[8].LP_instance->pCoeffs;

	IIR_LP_Values = FilterPathInfo[8].Lpf_ScaleValues;

	for (i = 0; i < IIR_NUM_TAPS; i++)
	{
		IIRStateF32_I[i] = 0;
		IIRStateF32_Q[i] = 0;
	}
	
	IIR_LPF_I.pState = (float32_t*) &IIRStateF32_I;
	IIR_LPF_Q.pState = (float32_t*) &IIRStateF32_Q;

	/* RXϣ�����ر任*/
	Hilbert_I.numTaps = Hilbert_Transform_I.numTaps;
	Hilbert_Q.numTaps = Hilbert_Transform_Q.numTaps;

	Hilbert_I.pCoeffs = Hilbert_Transform_I.pCoeffs;
	Hilbert_Q.pCoeffs = Hilbert_Transform_Q.pCoeffs;

	Hilbert_I.pState = (float32_t*) &Hilbert_State_I;
	Hilbert_Q.pState = (float32_t*) &Hilbert_State_Q;

	/*��ȡ���ڲ��˲��� */
	DECIMATE_I.numTaps = FilterPathInfo[AM_6K].Decimate_LP->numTaps;
	DECIMATE_Q.numTaps = FilterPathInfo[AM_6K].Decimate_LP->numTaps;

	INTERPOLATE_I.phaseLength = FilterPathInfo[AM_6K].Interpolate_LP->phaseLength / DECIMATION_RATE;
	INTERPOLATE_Q.phaseLength = FilterPathInfo[AM_6K].Interpolate_LP->phaseLength / DECIMATION_RATE;

	DECIMATE_I.pCoeffs = FilterPathInfo[AM_6K].Decimate_LP->pCoeffs;
	DECIMATE_Q.pCoeffs = FilterPathInfo[AM_6K].Decimate_LP->pCoeffs;
	INTERPOLATE_I.pCoeffs = FilterPathInfo[AM_6K].Interpolate_LP->pCoeffs;
	INTERPOLATE_Q.pCoeffs = FilterPathInfo[AM_6K].Interpolate_LP->pCoeffs;

	DECIMATE_I.M = DECIMATION_RATE;
	DECIMATE_Q.M = DECIMATION_RATE;

	INTERPOLATE_I.L = DECIMATION_RATE;
	INTERPOLATE_Q.L = DECIMATION_RATE;

	DECIMATE_I.pState = (float32_t*) &DecimState_I;
	DECIMATE_Q.pState = (float32_t*) &DecimState_Q;

	INTERPOLATE_I.pState = (float32_t*) &InterpState_I;
	INTERPOLATE_Q.pState = (float32_t*) &InterpState_Q;

	FIR_LPF_I.pState = (float32_t*) &FIR_State_I;
	FIR_LPF_Q.pState = (float32_t*) &FIR_State_Q;

	for (i = 0; i < FIR_NUM_TAPS + FIR_BLOCK_SIZE - 1; i++)
	{
		DecimState_I[i] = 0;
		DecimState_Q[i] = 0;
		InterpState_I[i] = 0;
		InterpState_Q[i] = 0;
		Hilbert_State_I[i] = 0;
		Hilbert_State_Q[i] = 0;
	}

	static uint32_t calc_taps;
	//
	nr.dsp_nr_strength = 15;
	nr.dsp_nr_delaybuf_len = 110;
	nr.dsp_nr_numtaps = 48;

	if ((nr.dsp_nr_numtaps < DSP_NR_NUMTAPS_MIN) || (nr.dsp_nr_numtaps > DSP_NR_NUMTAPS_MAX))
		calc_taps = DSP_NR_NUMTAPS_DEFAULT;
	else
		calc_taps = (uint16_t) nr.dsp_nr_numtaps;

	lms1Norm_instance.numTaps = calc_taps;
	lms1Norm_instance.pCoeffs = lms1NormCoeff_f32;
	lms1Norm_instance.pState = lms1StateF32;
	//

	mu_calc = (float) nr.dsp_nr_strength;
	mu_calc /= 2;
	mu_calc += 2;
	mu_calc /= 10;
	mu_calc = powf(10, mu_calc);
	mu_calc = 1 / mu_calc;
	lms1Norm_instance.mu = mu_calc;

	for (i = 0; i < LMS_NR_DELAYBUF_SIZE_MAX + BUFF_LEN; i++)
	{
		lms1_nr_delay[i] = 0;
	}
	//
	for (i = 0; i < DSP_NR_NUMTAPS_MAX + BUFF_LEN; i++)
	{
		lms1StateF32[i] = 0;

		if (nr.reset_dsp_nr)
		{
			lms1NormCoeff_f32[i] = 0;
		}
	}

	arm_lms_norm_init_f32(&lms1Norm_instance, calc_taps, &lms1NormCoeff_f32[0],
	        &lms1StateF32[0], (float32_t) mu_calc, nr.dsp_nr_numtaps);
	//

	if ((nr.dsp_nr_delaybuf_len > DSP_NR_BUFLEN_MAX/*48*/) || (nr.dsp_nr_delaybuf_len < DSP_NR_BUFLEN_MIN/*512*/))
		nr.dsp_nr_delaybuf_len = DSP_NR_BUFLEN_DEFAULT; // DSP_NR_BUFLEN_DEFAULT=192
}
/*
 *********************************************************
 *
 *********************************************************
 */
static void audio_lms_noise_reduction(int16_t psize)
{
	static uint32_t lms1_inbuf = 0, lms1_outbuf = 0;

	arm_copy_f32((float32_t*) ads.a_buffer, (float32_t*) &lms1_nr_delay[lms1_inbuf], psize);// put new data into the delay buffer

	arm_lms_norm_f32(&lms1Norm_instance, (float32_t*) ads.a_buffer,
	        (float32_t*) &lms1_nr_delay[lms1_outbuf], (float32_t*) ads.a_buffer,
	        (float32_t*) errsig1, psize);	// do noise reduction

	if ((((u32) fabs(ads.a_buffer[0])) * DSP_ZERO_DET_MULT_FACTOR)
	        < DSP_OUTPUT_MINVAL)	// is DSP level too low?
	                                // DSP_ZERO_DET_MULT_FACTOR = 10000000;DSP_OUTPUT_MINVAL = 1
	{
		if (ads.dsp_zero_count < MAX_DSP_ZERO_COUNT)
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
 *	�������ƣ�Gain_adjustment_control����
 *	�������ܣ�����ADC  ALC���������
 */
u16 Power_Gain_factor[] =
{
	24,											//80m
	15,																//60m
	11,																//40m
	10,																//30m
	10,																//20m
	10,																//17m
	10,																//15m
	10,																//12m
	20,																//10m
	10
};
/*
 *	DMA2->Stream1�жϳ���
 *	ʹ��DMA�ɼ�ADC3->DR����;
 *	����˫�����Է����ݶ�ʧ��
 */
u32 m;

void DMA2_Stream5_IRQHandler(void)
{
	u8 temp;
	//float32_t pow_v;
	//static u32 freq_lo;
	//static int16_t rit_0;
	static float32_t vol;
	// static u16 dac_offset_level;
	static float32_t mic_gain = 1.0f;
	static float32_t agc_gain = 1.0f;
	static float32_t agc2_gain = 1.0f, agc0;
	float32_t calc;
	float32_t varI_0, varI_1;	//calcI_amp;
	float32_t varQ_0, varQ_1;	//calcQ_amp;
	static float32_t mag_gain = 1.0f;
	//static  float32_t alc_gain=50.0f;
	static float32_t cw_alc_gain = 1.0f;
	//static	float32_t cw_gain[5];
	static float32_t fm_gain;
	static float32_t riss_gain;
	static float32_t riss_buf[SAMPLE_SIZE / 2];
	float32_t dp_I, dp_Q, ratio;
	u16 i;
	static u16 agc_time = 1, agc2_time;
	static uint32_t A;
	//static uint32_t B;
	//static uint32_t C;
	u16 psize;

	//static   float32_t bfo_i;
	//static   float32_t bfo_q; 
	//static   float32_t LO2_I;
	//static   float32_t LO2_Q;
	float32_t add_I;/* �ۼӻ��� */
	float32_t add_Q;/* �ۼӻ��� */
	static float32_t Data_I1[SAMPLE_SIZE / 2];/* DSP���� */
	static float32_t Data_Q1[SAMPLE_SIZE / 2];/* DSP���� */
	static float32_t Data_I[SAMPLE_SIZE / 2];/* DSP���� */
	static float32_t Data_Q[SAMPLE_SIZE / 2];/* DSP���� */
	static float32_t FM_I[2]; /* BP�������� */
	static float32_t FM_Q[2];/* BP�˲������� */
	static float32_t Data_FM_I[SAMPLE_SIZE / 2];
	//static   float32_t Data_FM_Q[SAMPLE_SIZE/2];
	//static   float32_t Dac2_out_I[SAMPLE_SIZE];/* DSP���� */
	//static   float32_t Dac2_out_Q[SAMPLE_SIZE];/* DSP���� */
	//static   float32_t AGC_out[SAMPLE_SIZE];/* DSP���� */

	if (DMA2->HISR & (1 << 11))	//�ȴ�DMA2_Steam5�������
	{
		DMA2->HIFCR |= 1 << 11;	//���DMA2_Steam5������ɱ�־
		/*
		 *	��ȡI/Q�������ݣ���DMA����˫���壻
		 */
		if (DMA2_Stream5->CR & 1 << 19)
		{
			for (i = 0; i < SAMPLE_SIZE / 2; i++)
			{
				Data_I1[i] = (float32_t) ADC_Buff0[i * 2];	// I
				Data_Q1[i] = (float32_t) ADC_Buff0[i * 2 + 1];	// Q
			}
		} else
		{
			for (i = 0; i < SAMPLE_SIZE / 2; i++)
			{
				Data_I1[i] = (float32_t) ADC_Buff1[i * 2];// * LO1_I;//LO1_I ����1��Ƶͬ�� I
				Data_Q1[i] = (float32_t) ADC_Buff1[i * 2 + 1];// * LO1_Q;//LO1_Q ����1��Ƶ���� Q
			}
		}
		psize = SAMPLE_SIZE / (ads.Rate * 2);	//��ȡ���ȡ������

		/*
		 ** RX����
		 */
		if (PTT_RT & ks.cat_ptt & ks.rit_tx & CW_K_DI & CW_K_DA)//((ks.CW_keySW ==0 &&CW_K_DI == 1 && CW_K_DA ==1) || (ks.CW_keySW ==1 && CW_K_DI ==1)))
		{

			/* �����ӳ� */
			if (ads.tx_delay > 0)
			{
				for (i = 0; i < SAMPLE_SIZE / 2; i++)
				{
					ads.out_buff[i * 2] = 0;
					ads.out_buff[i * 2 + 1] = 0;
				}
				ads.tx_delay -= 1;
				ads.rx_delay = 200;
				vol = 1;
				//ads.spk_vol =0;ads.hp_vol =0;
				if (vfo[VFO_A].Mode >= DEMOD_LSB && ads.tx_delay < 100)
				{
					MIC_POWER(1);
					TR_CONTROL(CONTROL_RX);/* ת��������״̬ */

					agc2_gain = 1.0f;
				}
				if (vfo[VFO_A].Mode < DEMOD_LSB && ads.tx_delay < 2)
				{
					ads.spk_vol = 20;
					ads.hp_vol = 20;
					agc2_gain = 0.025f;
					agc_gain = 3000;
				}
			}

			if (ads.tx_delay < 1)
			{
				if (vfo[VFO_A].Mode < DEMOD_LSB)
				{
					TR_CONTROL(CONTROL_RX);/* ת��������״̬ */
					if (ks.Spk_key)
						ads.spk_vol = 0;
					else
					{
						ads.spk_vol++;
						if (ads.spk_vol > 63)
							ads.spk_vol = 63;
					}
					ads.hp_vol++;
					if (ads.hp_vol > 55)
						ads.hp_vol = 55;
				} else if (vfo[VFO_A].Mode == DEMOD_USB_DIG
				        || vfo[VFO_A].Mode == DEMOD_LSB_DIG)
				{
					ads.spk_vol = 0;
					ads.hp_vol = 55;
				} else
				{
					if (ks.Spk_key)
						ads.spk_vol = 0;
					else
						ads.spk_vol = 63;
					ads.hp_vol = 55;
				}
				ads.tx_delay = 0;
				if (vfo[VFO_A].Mode < DEMOD_LSB)
					TR_CONTROL(CONTROL_RX);/* ת��������״̬ */
				//if(rit_0!=sd.rit)
				//{
				//	vol =0;
				//	rit_0=sd.rit;
				//}
				//else
				//{
				if (vol < sd.Vol * sd.Vol)
				{
					calc = sd.Vol * sd.Vol - vol;
					calc /= sd.Vol * sd.Vol;
					vol += vol * calc * 0.1f + 1.0f;
				}
				if (vol >= sd.Vol * sd.Vol)
					vol = sd.Vol * sd.Vol;	//*(1600/LCD_Pow(2,ads.spk_vol/6));
				//}
				/*
				 *	��λƽ��
				 */
				arm_scale_f32(Data_Q1, ads.rx_phase[ads.rx_amp_band],
				        ads.b_buffer, SAMPLE_SIZE / 2);			//ȡһ���Iͨ���ź�
				arm_add_f32(Data_I1, ads.b_buffer, Data_I1, SAMPLE_SIZE / 2);// ȡһ���Iͨ���ź���Q���
				/*
				 *	����ƽ��
				 */
				if (sd.Rx_amp_ma == 0)
				{
					arm_scale_f32(Data_Q1, ads.rx_amp[ads.rx_amp_band], Data_Q1,
					        SAMPLE_SIZE / 2);
					//arm_scale_f32(Data_Q1, ads.amp_adjust[ads.amp_adjust_num], Data_Q1,SAMPLE_SIZE/2);
				} else
				{

					//arm_scale_f32(Data_I, mag_gain, Data_I,SAMPLE_SIZE/2);
					arm_scale_f32(Data_Q1, mag_gain, Data_Q1, SAMPLE_SIZE / 2);
					//arm_max_f32(Data_I1, SAMPLE_SIZE/2, (float32_t *)&add_I, 0);/* AGC��Ʒ����ȡ���ֵ */
					//arm_max_f32(Data_Q1, SAMPLE_SIZE/2, (float32_t *)&add_Q, 0);/* AGC��Ʒ����ȡ���ֵ */
					arm_mean_f32(Data_I1, SAMPLE_SIZE / 2, &add_I);
					arm_mean_f32(Data_Q1, SAMPLE_SIZE / 2, &add_Q);
					calc = add_Q / add_I;					//I Q ���ȱ�������
					//add_I = 0;//����
					//add_Q = 0;//��
					if (calc > 1.0f)
					{
						//mag_gainI +=calc*0.0001f;
						//if(mag_gainI>5.0f)mag_gainI =1.0f;
						mag_gain -= calc * 0.001f;
						if (mag_gain < 0.2f)
							mag_gain = 1.0f;
					}
					if (calc < 1.0f)
					{
						//mag_gainI -=calc*0.0001f;
						//if(mag_gainI<0.2f)mag_gainI =1.0f;
						mag_gain += calc * 0.00001f;
						if (mag_gain > 5.0f)
							mag_gain = 1.0f;
					}
				}

				/*
				 *	һ��AGC��⣬���÷�ֵ��⣻
				 */
				arm_max_f32(Data_I1, SAMPLE_SIZE / 2, (float32_t*) &varI_0, 0);/* AGC��Ʒ����ȡ���ֵ */
				arm_min_f32(Data_I1, SAMPLE_SIZE / 2, (float32_t*) &varI_1, 0);/* AGC��Ʒ����ȡ���ֵ */
				arm_max_f32(Data_Q1, SAMPLE_SIZE / 2, (float32_t*) &varQ_0, 0);/* AGC��Ʒ����ȡ���ֵ */
				arm_min_f32(Data_Q1, SAMPLE_SIZE / 2, (float32_t*) &varQ_1, 0);/* AGC��Ʒ����ȡ���ֵ */

				//calc=(varI_0 - varI_1);
				calc = (varQ_0 + varI_0 - varQ_1 - varI_1);				//*0.5f;
				if (calc < 0)
					calc *= -1;
				/*
				 *	һ��AGC����
				 *	��������AGC����
				 */
				if ((u32) calc > sd.agc_Start) /* ������ص� */
				{
					calc = (u32) calc - sd.agc_Start;
					calc /= sd.agc_Start;
					agc_gain -= agc_gain * calc * AGC_FAST;
					if (agc_gain < 330)
						agc_gain = 330;
				} else
				//if((u32)calc < sd.agc_Start)	/* С����ص� */
				{
					if (++agc_time >= ads.agc_constant)
					{
						agc_gain += 1;
						calc = sd.agc_Start - (u32) calc;
						calc /= sd.agc_Start;
						agc_gain += agc_gain * calc * AGC_SLOW;
						if (agc_gain > 4095)
							agc_gain = 4095;
						agc_time = 0;
					}
				}
				/*
				 * 	��������AGC���ƣ�����DAC2�����ѹ���ź���������ӣ�
				 */
				DAC1->DHR12R2 = (u16) (agc_gain);/* DAC2��� */
				/*
				 *	�ź�ǿ�ȼ���
				 */
				temp = sd.Riss * 0.75f;
				ads.Riss = (u8) (46 - (DAC1->DHR12R2 - 330) * 0.0122178f - temp
				        + sd.S_correct + log10(1.0f / riss_gain) * 20.0f)
				        & 0x7f;
				ads.agc_gain = SIGNAl_SET - ads.Riss;				//ȥ��Ƶ����ʾ
				//ads.amp_adjust_num = (u8)((ads.Riss -78)*0.2f);
				//if(ads.amp_adjust_num>11)ads.amp_adjust_num =11;
				//ads.amp_adjust[ads.amp_adjust_num] = Rx_amp_adjust[ads.amp_adjust_num]*0.001f;
				/*
				 *	Hilbert Transform
				 */
				arm_fir_f32(&Hilbert_I, (float32_t*) Data_I1,
				        (float32_t*) Data_I, SAMPLE_SIZE / 2);
				arm_fir_f32(&Hilbert_Q, (float32_t*) Data_Q1,
				        (float32_t*) Data_Q, SAMPLE_SIZE / 2);

				/*
				 *	��Ƶ����
				 */
				arm_add_f32((float32_t*) Data_I, (float32_t*) Data_Q,
				        (float32_t*) ads.b_buffer, SAMPLE_SIZE / 2);// �����ȡ�ϱߴ�USB
				arm_sub_f32((float32_t*) Data_I, (float32_t*) Data_Q,
				        (float32_t*) ads.a_buffer, SAMPLE_SIZE / 2);// �����ȡ�±ߴ�LSB
				/*
				 *	 FFT������ȡ��
				 */
				if (TR_READ == CONTROL_RX)
				{
					if (spe.fft_flang == 0)
					{
						for (i = 0; i < SAMPLE_SIZE / 2; i++)
						{
							spe.fft_num++;	//FFT���ݲ���������
							if (spe.fft_num > FFT_LENGTH * 2 - 1)
							{
								spe.fft_num = 0;	//FFT���ݲ���������
								spe.fft_flang = 1;
							}
							fft_buf[spe.fft_num * 2] = ads.a_buffer[i];
							fft_buf[spe.fft_num * 2 + 1] = ads.b_buffer[i];
						}
					}
				}
				/*
				 *	DSP����
				 */

				softdds_runf(DDS_LO1, ads.ddslo_q, ads.ddslo_i,
				        SAMPLE_SIZE / 2);
				/* DSP��Ƶ������ I/Q�ź� */
				for (i = 0; i < SAMPLE_SIZE / 2; i++)
				{
					Data_I[i] = ads.a_buffer[i] * ads.ddslo_i[i];//LO1_I;// ����1��Ƶͬ�� I
					Data_Q[i] = ads.a_buffer[i] * ads.ddslo_q[i];//LO1_Q;//����1��Ƶ���� Q
				}

				/*
				 * 	�źų�ȡ�����Ͳ����ʣ�
				 * 	��ͨ������ڽ��ͺ�Ĳ����ʵ�1/2��
				 */
				arm_fir_decimate_f32(&DECIMATE_I, (float32_t*) Data_I,
				        (float32_t*) Data_I, SAMPLE_SIZE / 2);
				arm_fir_decimate_f32(&DECIMATE_Q, (float32_t*) Data_Q,
				        (float32_t*) Data_Q, SAMPLE_SIZE / 2);

				/* CW�͵��ߴ����� */
				if (vfo[VFO_A].Mode < DEMOD_AM)
				{
					/*
					 *	Hilbert Transform
					 */
					//arm_fir_f32(&Hilbert_I,(float32_t*)Data_I,(float32_t*)Data_I1,psize);
					//arm_fir_f32(&Hilbert_Q,(float32_t*)Data_Q,(float32_t*)Data_Q1,psize);

					/*  
					 * 	IIR��ͨ�˲������������յĽ��մ���
					 */
					arm_biquad_cascade_df1_f32(&IIR_LPF_I, (float32_t*) Data_I,
					        (float32_t*) Data_I, psize);		//LP��ͨ�˲� Iͨ��
					arm_biquad_cascade_df1_f32(&IIR_LPF_Q, (float32_t*) Data_Q,
					        (float32_t*) Data_Q, psize);		//LP��ͨ�˲� Qͨ��
					/*
					 * 	LP_Values��IIR�˲�������ϵ����
					 */
					arm_scale_f32(Data_I, IIR_LP_Values, Data_I, psize);
					arm_scale_f32(Data_Q, IIR_LP_Values, Data_Q, psize);
					//
					/*BFO*/
					softdds_runf(DDS_BFO, ads.ddsbfo_q, ads.ddsbfo_i, psize);
					//CW/SSB���
					for (i = 0; i < psize; i++)
					{

						if (vfo[VFO_A].Mode == DEMOD_LSB_DIG
						        || vfo[VFO_A].Mode == DEMOD_LSB
						        || vfo[VFO_A].Mode == DEMOD_CW_LSB)
							ads.a_buffer[i] = Data_Q[i] * ads.ddsbfo_q[i]
							        - Data_I[i] * ads.ddsbfo_i[i]; //����2��Ƶ������õ��±ߴ�LSB��Ƶ

						if (vfo[VFO_A].Mode == DEMOD_USB_DIG
						        || vfo[VFO_A].Mode == DEMOD_USB
						        || vfo[VFO_A].Mode == DEMOD_CW_USB)
							ads.a_buffer[i] = Data_Q[i] * ads.ddsbfo_q[i]
							        + Data_I[i] * ads.ddsbfo_i[i]; //����2��Ƶ����ӵõ��ϱߴ�LSB��Ƶ

//						if(vfo[VFO_A].Mode == DEMOD_LSB_DIG ||  vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_CW_LSB)
//							ads.a_buffer[i] =  Data_I[i] + Data_Q[i] ; //����2��Ƶ������õ��±ߴ�LSB��Ƶ
//						 
//						if(vfo[VFO_A].Mode == DEMOD_USB_DIG ||  vfo[VFO_A].Mode == DEMOD_USB ||  vfo[VFO_A].Mode == DEMOD_CW_USB)
//							ads.a_buffer[i] =  Data_I[i] - Data_Q[i] ;//����2��Ƶ����ӵõ��ϱߴ�LSB��Ƶ	
					}
					//arm_biquad_cascade_df1_f32(&IIR_AF, (float32_t *)ads.a_buffer, (float32_t *)ads.a_buffer, psize);//LP��ͨ�˲� Qͨ��
					//arm_scale_f32((float32_t *)ads.a_buffer, SSB_AF_ScaleValues, (float32_t *)ads.a_buffer,psize);
					//arm_copy_f32(ads.a_buffer,riss_buf ,psize);
					arm_scale_f32(ads.a_buffer, riss_gain, riss_buf, psize);

					for (i = 0; i < psize; i++)
					{
						ads.a_buffer[i] *= agc2_gain;
						calc = ads.a_buffer[i];
						if (calc < 0)
							calc *= -1;
						//����AGC����
						if (calc > AGC_THRESHOLD)
						{
							calc = calc - AGC_THRESHOLD;
							calc /= AGC_THRESHOLD;
							agc2_gain -= agc2_gain * calc * AGC_FAST;
							if (agc2_gain < 0.001f)
								agc2_gain = 0.001f;
						} else
						//if(calc < AGC_SSB_THRESHOLD)
						{
							if (++agc2_time >= ads.agc_constant)
							{
								calc = AGC_THRESHOLD - calc;
								calc /= AGC_THRESHOLD;
								agc2_gain += agc2_gain * calc * AGC_SLOW;
								if (agc2_gain > 1.0f)
									agc2_gain = 1.0f;
								agc2_time = 0;
							}
						}
					}
					//
					/*
					 *	�ź�ǿ�ȶ���ȡ������
					 */
					arm_mean_f32((float32_t*) riss_buf, psize,
					        (float32_t*) &calc);

					if (calc < 0)
						calc *= -1;
					if (calc > RISS_SSB_THRESHOLD)
					{
						calc = calc - RISS_SSB_THRESHOLD;
						calc /= RISS_SSB_THRESHOLD;
						riss_gain -= riss_gain * calc * RISS_FAST;
						if (riss_gain < 0.0000001f)
							riss_gain = 0.0000001f;
					} else
					{
						calc = RISS_SSB_THRESHOLD - calc;
						calc /= RISS_SSB_THRESHOLD;
						riss_gain += riss_gain * calc * RISS_SLOW;
						if (riss_gain > AGC_GAIN_MAX)
							riss_gain = AGC_GAIN_MAX;
					}
					//
					// DSP noise reduction using LMS (Least Mean Squared) algorithm
					//
					if (ks.NR_key == 1)	// && ( vfo[VFO_A].Mode == DEMOD_LSB ||  vfo[VFO_A].Mode == DEMOD_USB))	 	// Do this if enabled and "Pre-AGC" DSP NR enabled
					{
						audio_lms_noise_reduction(psize);
					}
					arm_biquad_cascade_df1_f32(&IIR_AF,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.a_buffer, psize);	//LP��ͨ�˲� Qͨ��
					arm_scale_f32((float32_t*) ads.a_buffer, SSB_AF_ScaleValues,
					        (float32_t*) ads.a_buffer, psize);

					//�ڲ�ָ���ԭ������������ͨ��
					arm_fir_interpolate_f32(&INTERPOLATE_I,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.b_buffer, psize);
					//
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						ads.out_buff[i * 2] = ads.b_buffer[i] * vol * 4.0f
						        * ads.Af_mute;			//  + DAC_OFFSET_LEVEL ;
						ads.out_buff[i * 2 + 1] = ads.b_buffer[i] * vol * 4.0f
						        * ads.Af_mute;			// + DAC_OFFSET_LEVEL ;
					}
				}
				//break;
				else
				//case DEMOD_AM:
				if (vfo[VFO_A].Mode == DEMOD_AM)
				{
					//arm_fir_decimate_f32(&DECIMATE_I, (float32_t *)Data_I, (float32_t *)Data_I, SAMPLE_SIZE);
					//arm_fir_decimate_f32(&DECIMATE_Q, (float32_t *)Data_Q, (float32_t *)Data_Q, SAMPLE_SIZE);

					arm_biquad_cascade_df1_f32(&IIR_LPF_I, (float32_t*) Data_I,
					        (float32_t*) Data_I, psize);		//LP��ͨ�˲� Iͨ��
					arm_biquad_cascade_df1_f32(&IIR_LPF_Q, (float32_t*) Data_Q,
					        (float32_t*) Data_Q, psize);		//LP��ͨ�˲� Qͨ��
					arm_scale_f32(Data_I, IIR_LP_Values, Data_I, psize);//Iͨ������AGC���ƣ�LP_Values��IIR�˲���ϵ��
					arm_scale_f32(Data_Q, IIR_LP_Values, Data_Q, psize);//Qͨ������AGC���ƣ�LP_Values��IIR�˲���ϵ��
					//arm_max_f32((float32_t *)riss_buf, psize, (float32_t *)&calc0, 0);/* AGC��Ʒ����ȡ���ֵ */
					//arm_min_f32((float32_t *)riss_buf, psize, (float32_t *)&calc1, 0);/* AGC��Ʒ����ȡ���ֵ */
					/*
					 *	AM���
					 */
					for (i = 0; i < psize; i++)
					{
						/*AM���*/
						arm_sqrt_f32(
						        (Data_Q[i] * Data_Q[i] + Data_I[i] * Data_I[i]),
						        (float32_t*) &ads.a_buffer[i]);			//AM���
						//ads.out_buff[i] = (u16)(ads.a_buffer[i]*sd.Vol * 0.05f + DAC_OFFSET_LEVEL);
					}
					arm_biquad_cascade_df1_f32(&RISS, ads.a_buffer, riss_buf,
					        psize);						//LP��ͨ�˲� Qͨ��
					arm_scale_f32(riss_buf, Riss_ScaleValues * riss_gain,
					        riss_buf, psize);
					//arm_scale_f32(ads.a_buffer, riss_gain, riss_buf ,psize);//�ź�ǿ�ȶ���ȡ

					for (i = 0; i < psize; i++)
					{
						ads.a_buffer[i] *= agc2_gain;
						calc = ads.a_buffer[i];
						if (calc < 0)
							calc *= -1;
						//����AGC����
						if (calc > AGC_THRESHOLD)
						{
							calc = calc - AGC_THRESHOLD;
							calc /= AGC_THRESHOLD;
							agc2_gain -= agc2_gain * calc * AGC_FAST;
							if (agc2_gain < 0.001f)
								agc2_gain = 0.001f;
						} else
						//if(calc < AGC_SSB_THRESHOLD)
						{
							if (++agc2_time >= ads.agc_constant)
							{
								calc = AGC_THRESHOLD - calc;
								calc /= AGC_THRESHOLD;
								agc2_gain += agc2_gain * calc * AGC_SLOW;
								if (agc2_gain > 1)
									agc2_gain = 1;
								agc2_time = 0;
							}
						}
					}
					//

					/*
					 *	�ź�ǿ�ȶ���ȡ������
					 */
					arm_mean_f32((float32_t*) riss_buf, psize,
					        (float32_t*) &calc);
					//arm_max_f32((float32_t *)riss_buf, psize+1, (float32_t *)&calc0, 0);/* AGC��Ʒ����ȡ���ֵ */
					//arm_min_f32((float32_t *)riss_buf, psize+1, (float32_t *)&calc1, 0);/* AGC��Ʒ����ȡ���ֵ */
					//calc1=calc0-calc1;
					if (calc < 0)
						calc *= -1;
					if (calc > RISS_AMFM_THRESHOLD)
					{
						calc = calc - RISS_AMFM_THRESHOLD;
						calc /= RISS_AMFM_THRESHOLD;
						riss_gain -= riss_gain * calc * RISS_FAST;
						if (riss_gain < 0.00001f)
							riss_gain = 0.00001f;
					} else
					{
						//if(++Riss_time>=ads.agc_constant )
						//{
						calc = RISS_AMFM_THRESHOLD - calc;
						calc /= RISS_AMFM_THRESHOLD;
						riss_gain += riss_gain * calc * RISS_SLOW;
						if (riss_gain > AGC_GAIN_MAX)
							riss_gain = AGC_GAIN_MAX;
						//Riss_time =0;
						//}
					}
					/*
					 *	AF��ͨ�˲�
					 */
					arm_biquad_cascade_df1_f32(&IIR_AF,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.a_buffer, psize);	//LP��ͨ�˲� Qͨ��
					arm_scale_f32((float32_t*) ads.a_buffer, AF_ScaleValues,
					        (float32_t*) ads.a_buffer, psize);
					//�ڲ�ָ���ԭ������������ͨ��
					arm_fir_interpolate_f32(&INTERPOLATE_I,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.b_buffer, psize);
					/*
					 *
					 */
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						/*AM���*/
						ads.out_buff[i * 2] = ads.b_buffer[i] * vol * 0.4f
						        * ads.Af_mute;			// + DAC_OFFSET_LEVEL ;
						ads.out_buff[i * 2 + 1] = ads.b_buffer[i] * vol * 0.4f
						        * ads.Af_mute;			//  + DAC_OFFSET_LEVEL ;
					}
				} else
				/*
				 *	case DEMOD_FM:
				 */
				if (vfo[VFO_A].Mode == DEMOD_FM)
				{
					/*�ز���ȡ*/
					for (i = 0; i < psize; i++)
					{
						arm_sqrt_f32(
						        (Data_Q[i] * Data_Q[i] + Data_I[i] * Data_I[i]),
						        (float32_t*) &ads.b_buffer[i]);				//
					}
					arm_biquad_cascade_df1_f32(&RISS, ads.b_buffer, riss_buf,
					        psize);						//LP��ͨ�˲� Qͨ��
					arm_scale_f32(riss_buf, Riss_ScaleValues * riss_gain,
					        riss_buf, psize);
					//arm_scale_f32(ads.b_buffer, riss_gain, riss_buf ,psize);//�ź�ǿ�ȶ���ȡ
					/*
					 *	����AGC
					 */
					for (i = 0; i < psize; i++)
					{
						Data_I[i] *= agc0;
						Data_Q[i] *= agc0;
						arm_sqrt_f32(
						        (Data_Q[i] * Data_Q[i] + Data_I[i] * Data_I[i]),
						        (float32_t*) &ads.b_buffer[i]);
						calc = ads.b_buffer[i];
						if (calc < 0)
							calc *= -1;
						if (calc > AGC_THRESHOLD)
						{
							calc = calc - AGC_THRESHOLD;
							calc /= AGC_THRESHOLD;
							agc0 -= agc0 * calc * AGC_SLOW;
							if (agc0 < 0.001f)
								agc0 = 0.001f;
						} else
						{
							//if(++agc2_time>=ads.agc_constant )
							//{
							calc = AGC_THRESHOLD - calc;
							calc /= AGC_THRESHOLD;
							agc0 += agc0 * calc * AGC_FAST;
							if (agc0 > AGC_GAIN_MAX_FM)
								agc0 = AGC_GAIN_MAX_FM;
							//	agc2_time =0;
							//}
						}
						arm_sqrt_f32(agc2_gain, &agc0);
						ads.a_buffer[i] *= agc0;
					}

					//
					/*
					 *	�ź�ǿ�ȶ���ȡ������
					 */
					arm_mean_f32((float32_t*) riss_buf, psize,
					        (float32_t*) &calc);
					if (calc < 0)
						calc *= -1;
					if (calc > RISS_AMFM_THRESHOLD)
					{
						calc = calc - RISS_AMFM_THRESHOLD;
						calc /= RISS_AMFM_THRESHOLD;
						riss_gain -= riss_gain * calc * RISS_FAST;
						if (riss_gain < 0.00001f)
							riss_gain = 0.00001f;
					} else
					{
						//if(++Riss_time>=ads.agc_constant )
						//{
						calc = RISS_AMFM_THRESHOLD - calc;
						calc /= RISS_AMFM_THRESHOLD;
						riss_gain += riss_gain * calc * RISS_MED;
						if (riss_gain > AGC_GAIN_MAX)
							riss_gain = AGC_GAIN_MAX;
						//Riss_time =0;
						//}
					}
					/*FM���*/
					/*******************************************************************************
					 ������ط�FM���  ����ʽ���õ��㷨��
					 ��ʽ��
					 X(n)=[Q(n)/I(n)-Q(n-1)/I(n-1)] / [Q(n)*Q(n)+I(n)*I(n)]
					 *******************************************************************************/
					/***������ط�FM���***/
					for (i = 0; i < psize; i++)
					{
						//	

						dp_I = Data_I[i] * Data_I[i] + Data_Q[i] * Data_Q[i];
						dp_Q = Data_Q[i] * FM_I[1] - Data_I[i] * FM_Q[1];
						//
						if (dp_Q > 0)
						{
							if (dp_I > 0) 				// Quadrant I
							{
								ratio = (dp_I - dp_Q) / (dp_I + dp_Q);
								ads.a_buffer[i] = (1 - ratio);
							} else 						// Quadrant II
							{
								ratio = (dp_I + dp_Q) / (dp_Q - dp_I);
								ads.a_buffer[i] = (3 - ratio);
							}
						} else
						{
							if (dp_I < 0)				 // Quadrant III
							{
								ratio = (dp_Q - dp_I) / (dp_I + dp_Q);
								ads.a_buffer[i] = (ratio - 3);
							} else 						 // Quadrant IV
							{
								ratio = (dp_I + dp_Q) / (dp_I - dp_Q);
								ads.a_buffer[i] = (ratio - 1);
							}
						}
						//
						if (FM_5K)
							ads.a_buffer[i] *= FM_RX_SCALING_2K5;
						else if (FM_10K)
							ads.a_buffer[i] *= FM_RX_SCALING_5K;

						FM_I[1] = Data_I[i];
						FM_Q[1] = Data_Q[i];
					}
					//
					/*******************************************************************************
					 ����С�ǶȽӽ���: (����δ���ã�ֻ����Ϊ�ο�)
					 ��ʽ��
					 X(n)=[Q(n)*I(n-1)-I(n)*Q(n-1)]
					 *******************************************************************************/
					//					else
					//					if(k_num.SPE_K1==1)
					//					{
					//						/***С�ǶȽӽ���FM���***/
					//						for(i=0;i<psize;i++)
					//						{
					//
					//							//dp_I = Data_I[i] * FM_I[1] + Data_I[i] * FM_I[1];//FM���
					//							ads.a_buffer[i] = Data_Q[i] * FM_I[1] - Data_I[i] * FM_Q[1];//FM���
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
					 *	AF��ͨ�˲�
					 */

					arm_biquad_cascade_df1_f32(&IIR_AF,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.a_buffer, psize);	//LP��ͨ�˲� Qͨ��
					arm_scale_f32((float32_t*) ads.a_buffer, AF_ScaleValues,
					        (float32_t*) ads.a_buffer, psize);
					//�ڲ�ָ���ԭ����������ȥ���ص�ͨ��
					arm_fir_interpolate_f32(&INTERPOLATE_I,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.b_buffer, psize);
					//���ص����
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						ads.out_buff[i * 2] = ads.b_buffer[i] * vol * 0.02f
						        * ads.Af_mute;	//  + DAC_OFFSET_LEVEL ;
						ads.out_buff[i * 2 + 1] = ads.b_buffer[i] * vol * 0.02f
						        * ads.Af_mute;	//  + DAC_OFFSET_LEVEL ;
						//ads.out_buff[i] =(u16)(Data_I[i]*sd.Vol * 0.01f + DAC_OFFSET_LEVEL);
					}
				}
				if (vfo[VFO_A].Mode > DEMOD_CW_USB && ads.tx_delay > 10)
				{
					vol = 0;
					//ads.Af_mute =0;
					for (i = 0; i < SAMPLE_SIZE; i++)
					{
						ads.b_buffer[i] = 0;			//+ DAC_OFFSET_LEVEL) ;
					}
				}
			}
		}
		/*
		 ***********************************************************************************************************************
		 *			TX���䴦��
		 ***********************************************************************************************************************
		 */

		if (ks.rt_rx == 0 && ks.tx_no != 1)	//&&(PTT_RT == 0 || cw_Auto_key.dit==0 || cw_Auto_key.dah==0))//&& vfo[VFO_A].Band_id != COUPLING_60M)/* �շ��Ż�ģʽ */
		{

			/* CWģʽ���� */
			if (vfo[VFO_A].Mode < DEMOD_LSB)/**/
			{
				MIC_POWER(1);
				/*
				 *	di/da��ת��
				 */
				if (ks.CW_AB == 1)
				{
					cw_Auto_key.dit = CW_K_DI;
					cw_Auto_key.dah = CW_K_DA;
				} else
				{
					cw_Auto_key.dit = CW_K_DA;
					cw_Auto_key.dah = CW_K_DI;
				}
				//softdds_runf(DDS_SI,ads.ddssi_i,ads.ddssi_q, SAMPLE_SIZE/2);
				//softdds_runf(DDS_LO1,ads.ddslo_i ,ads.ddslo_q, SAMPLE_SIZE/2);//CW��Ƶ
				/*
				 *	�ֶ���ģʽ
				 */
				if (ks.CW_keySW == 1 /*&& ks.PWR_key ==0*/)
				{
					if (PTT_RT == TX || cw_Auto_key.dit == 0)/* �ֶ������� */
					{
						ads.tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
						if (ks.Spk_key)
							ads.spk_vol = 0;
						else
							ads.spk_vol = 63;
						ads.hp_vol = 55;

						AMP_OFF;
						ATT_OFF;

						softdds_runf(DDS_SI, ads.ddssi_i, ads.ddssi_q,
						        SAMPLE_SIZE / 2);
						if (ks.CW_Exercise < 1)/* ���������ϰģʽ */
						{
							TR_CONTROL(CONTROL_TX);/* ���������ϰģʽ,���俪�� */
							softdds_runf(DDS_TX_LO, ads.ddslo_i_tx,
							        ads.ddslo_q_tx, SAMPLE_SIZE / 2);	//CW��Ƶ
						}
						/* ALC */
						calc = ads.pow;/* ALCȡ�� */
						if (calc < 0)
							calc *= -1;
						if (calc > sd.Pow)
						{
							calc = calc - sd.Pow;
							calc = calc / sd.Pow;
							arm_sqrt_f32(calc, (float32_t*) &calc);
							cw_alc_gain -= cw_alc_gain * calc * 0.1f;
							if (cw_alc_gain < 0.01f)
								cw_alc_gain = 0.01f;
						}
						if (calc < sd.Pow)
						{
							calc = sd.Pow - calc;
							calc = calc / sd.Pow;
							arm_sqrt_f32(calc, (float32_t*) &calc);
							cw_alc_gain += cw_alc_gain * calc * 0.001f;
							if (cw_alc_gain > 1.0f)
								cw_alc_gain = 1.0f;
						}

						for (i = 0; i < SAMPLE_SIZE / 2; i++)
						{
							ads.si_i[i] = ads.ddssi_i[i] * sd.CW_vol * 63;

							Data_I[i] = ads.ddslo_i_tx[i] * cw_alc_gain
							        * ads.cw_gain[1] / ads.cw_gain[2];//*ads.tx_amp ;//
							Data_Q[i] = ads.ddslo_q_tx[i] * cw_alc_gain
							        * ads.cw_gain[1] / ads.cw_gain[2];		//
						}
						ads.tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
					}
					//else 
					if (PTT_RT == RX && cw_Auto_key.dit == 1)		// �ֶ����ɿ�
					{
						if (ads.tx_delay)
						{
							for (i = 0; i < SAMPLE_SIZE / 2; i++)
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
				 *	�Զ���ģʽ
				 */
				if (ks.CW_keySW == 0)
				{
					if (PTT_RT == 1 && cw_Auto_key.dit == 0
					        && cw_Auto_key.dah == 0)/* �Զ���ģʽ di�μ����ռ������� */
					{
						cw_Auto_key.test_state = 3;
						//ads.tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
					} else if (PTT_RT == 1 && cw_Auto_key.dit == 0)/* �Զ���ģʽ di�μ����� */
					{
						cw_Auto_key.test_state = 2;
						//ads.tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
					} else if (PTT_RT == 1 && cw_Auto_key.dah == 0)/* �Զ���ģʽ da�ռ����� */
					{
						cw_Auto_key.test_state = 1;
						//ads.tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
					} else if (PTT_RT == 1 && cw_Auto_key.dit == 1
					        && cw_Auto_key.dah == 1)/* �Զ���ģʽ da�ռ����ռ����ɿ� */
					{
						cw_Auto_key.test_state = 0;
					}

					if (cw_Auto_key.now_state != cw_Auto_key.test_state)
						cw_Auto_key.now_state = cw_Auto_key.test_state;

					//softdds_runf(DDS_SI,ads.ddssi_i,ads.ddssi_q, SAMPLE_SIZE/2);
					//softdds_runf(DDS_LO1,ads.ddslo_i ,ads.ddslo_q, SAMPLE_SIZE/2);//CW��Ƶ
					if (cw_Auto_key.ms_time > 0)
					{
						if (ks.Spk_key)
							ads.spk_vol = 0;
						else
							ads.spk_vol = 63;
						ads.hp_vol = 55;
						AMP_OFF;
						ATT_OFF;

						//AMP_OFF;
						if (cw_Auto_key.ms_time > ads.cw_spe)
						{
							softdds_runf(DDS_SI, ads.ddssi_i, ads.ddssi_q,
							        SAMPLE_SIZE / 2);
							if (ks.CW_Exercise < 1)				//���������ϰģʽ
							{
								TR_CONTROL(CONTROL_TX);/* ���俪�� */
								softdds_runf(DDS_TX_LO, ads.ddslo_i_tx,
								        ads.ddslo_q_tx, SAMPLE_SIZE / 2);//CW��Ƶ
							}
							/*	ALC	*/
							calc = ads.pow;/* ALCȡ�� */
							if (calc < 0)
								calc *= -1;
							if (calc > sd.Pow)
							{
								calc = calc - sd.Pow;
								calc = calc / sd.Pow;
								arm_sqrt_f32(calc, (float32_t*) &calc);
								cw_alc_gain -= cw_alc_gain * calc * 0.1f;
								if (cw_alc_gain < 0.01f)
									cw_alc_gain = 0.01f;
							}
							if (calc < sd.Pow)
							{
								calc = sd.Pow - calc;
								calc = calc / sd.Pow;
								arm_sqrt_f32(calc, (float32_t*) &calc);
								cw_alc_gain += cw_alc_gain * calc * 0.001f;
								if (cw_alc_gain > 1.0f)
									cw_alc_gain = 1.0f;
							}
							//
							for (i = 0; i < SAMPLE_SIZE / 2; i++)
							{
								ads.si_i[i] = ads.ddssi_i[i] * sd.CW_vol * 63;
								//ads.a_buffer[i] = ads.ddssi_q[i] * cw_alc_gain*500;//
								//if(ks.CW_Exercise <1)//���������ϰģʽ
								//{

								Data_I[i] = ads.ddslo_i_tx[i] * cw_alc_gain
								        * ads.cw_gain[1] / ads.cw_gain[2];//*ads.tx_amp ;//
								Data_Q[i] = ads.ddslo_q_tx[i] * cw_alc_gain
								        * ads.cw_gain[1] / ads.cw_gain[2];	//
								//}
							}
							//
							ads.tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
							//arm_fir_f32(&Hilbert_I,ads.a_buffer,(float32_t*)Data_I,SAMPLE_SIZE/2);
							//arm_fir_f32(&Hilbert_Q,ads.a_buffer,(float32_t*)Data_Q,SAMPLE_SIZE/2);
						} else
						{
							if (ads.tx_delay)
							{
								for (i = 0; i < SAMPLE_SIZE / 2; i++)
								{
									Data_I[i] = 0;
									Data_Q[i] = 0;
									ads.si_i[i] *= 0;			//sd.CW_vol*500;
								}
							}

						}
						/* CW�Զ����ٶȼ�ʱ */
						cw_Auto_key.ms_time--;							//ʱ���Լ�
						//
						if (cw_Auto_key.mm_count == 0)
						{
							if ((cw_Auto_key.now_state > 0)
							        && (cw_Auto_key.now_state
							                != cw_Auto_key.out_state))
							{
								if (cw_Auto_key.now_state < 3)
								{
									cw_Auto_key.mm_state =
									        cw_Auto_key.now_state;
								} else
								{
									if (cw_Auto_key.out_state == 1)
										cw_Auto_key.mm_state = 2;
									if (cw_Auto_key.out_state == 2)
										cw_Auto_key.mm_state = 1;
								}
								cw_Auto_key.mm_count = 1;
							}
						}
					} else					//����Զ����������ˣ��ж���һ��Ӧ�÷�ʲô
					{
						if (cw_Auto_key.mm_count > 0)//�����һ�λ��д洢��������ȡ��ֵ
						{
							cw_Auto_key.out_state = cw_Auto_key.mm_state;
							cw_Auto_key.mm_count = 0;
						} else
						{
							cw_Auto_key.out_state = cw_Auto_key.now_state;
						}

						if (cw_Auto_key.out_state == 0)			//2���������ſ�
						{
							if (ks.CW_keySW == 2)						//Bģʽ
							{
								if (cw_Auto_key.last_one == 0
								        && cw_Auto_key.last_key_state == 3)	//��û�в���һ������
								{
									if (cw_Auto_key.last_auto_key == 1)
										cw_Auto_key.ms_time = ads.cw_spe * 4;//��DA
									else
										cw_Auto_key.ms_time = ads.cw_spe * 2;//��DI
								}
								cw_Auto_key.last_one = 1;
							}
							cw_Auto_key.last_key_state = 0;
							cw_Auto_key.last_auto_key = 0;
							//TX_sign =0;
						} else if (cw_Auto_key.out_state == 1)		//�� ����
						{
							if (ads.tx_delay)
								cw_Auto_key.ms_time = ads.cw_spe * 2;	//��di
							else
								cw_Auto_key.ms_time = ads.cw_spe * 2 + 10;//��di ��һ���ķ�ʱ�䲹��
							cw_Auto_key.last_auto_key = 1;
							cw_Auto_key.last_key_state = 1;
							cw_Auto_key.last_one = 0;
						} else if (cw_Auto_key.out_state == 2)			//� ����
						{
							if (ads.tx_delay)
								cw_Auto_key.ms_time = ads.cw_spe * 4;	//��da
							else
								cw_Auto_key.ms_time = ads.cw_spe * 4 + 10;//��da ��һ���ķ�ʱ�䲹��
							cw_Auto_key.last_auto_key = 2;
							cw_Auto_key.last_key_state = 2;
							cw_Auto_key.last_one = 0;
						} else if (cw_Auto_key.out_state == 3) //�� �� ������
						{
							if (cw_Auto_key.last_auto_key == 1) //�ϴ�=di
							{
								cw_Auto_key.ms_time = ads.cw_spe * 4; //��da
								cw_Auto_key.last_auto_key = 2;
							} else
							{
								cw_Auto_key.ms_time = ads.cw_spe * 2; //��di
								cw_Auto_key.last_auto_key = 1;
							}
							cw_Auto_key.last_key_state = 3;
							cw_Auto_key.last_one = 0;
						}
					}
				}

			}
			//else	
			/*
			 *	SSB���ߴ�ģʽ���䴦��
			 */
			if ((PTT_RT == TX || ks.cat_ptt == TX || ks.rit_tx == TX)
			        && (vfo[VFO_A].Mode >= DEMOD_LSB
			                && vfo[VFO_A].Mode <= DEMOD_USB_DIG))
			{
				ads.spk_vol = 0;
				ads.hp_vol = 0;
				if (ads.rx_delay > 0)
				{
					ads.tx_delay = 300;
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						ads.out_buff[i * 2] = 0;
						ads.out_buff[i * 2 + 1] = 0;
					}
					if (PTT_RT == TX || ks.rit_tx == TX)
						MIC_POWER(0);
					ads.rx_delay--;
					if (ads.rx_delay < 100)
						TR_CONTROL(CONTROL_TX);/* ���俪�� */
					if (ads.rx_delay > 10)
						mic_gain = 1.0f;
				}
				if (ads.rx_delay < 50)
				{
					if (ads.rx_delay < 1)
						ads.rx_delay = 0;
//					if(ads.rx_delay>0&&ads.rx_delay<10 )
//					{
//						mic_gain +=0.1f;
//					}
					/*
					 *	ALC
					 */

					calc = ads.pow;
					if (calc < 0)
						calc *= -1.0f;

					if (calc > sd.Pow)
					{
						calc = calc - sd.Pow;
						calc = calc / sd.Pow;
						arm_sqrt_f32(calc, (float32_t*) &calc);
						mic_gain -= mic_gain * calc * 0.1f;
						if (mic_gain < 0.01f)
							mic_gain = 0.01f;
					} else
					//if(calc < cw_gain[1])	
					{
						calc = sd.Pow - calc;
						calc = calc / sd.Pow;
						arm_sqrt_f32(calc, (float32_t*) &calc);
						mic_gain += mic_gain * calc * 0.0033f;
						if (mic_gain > 2.0f)
							mic_gain = 2.0f;
					}

					if (ks.bt_key == 1 && ks.cat_ptt == TX)
						arm_fir_decimate_f32(&DECIMATE_Q, (float32_t*) Data_Q1,
						        ads.a_buffer, SAMPLE_SIZE / 2);
					else
						arm_fir_decimate_f32(&DECIMATE_I, (float32_t*) Data_I1,
						        ads.a_buffer, SAMPLE_SIZE / 2);
					arm_biquad_cascade_df1_f32(&IIR_AF, ads.a_buffer,
					        ads.a_buffer, psize); //LP��ͨ�˲� Qͨ��
					arm_scale_f32(ads.a_buffer,
					        AF_ScaleValues * mic_gain * ads.cw_gain[0]
					                * sd.mic_gain / ads.cw_gain[2],
					        ads.a_buffer, psize);

					softdds_runf(DDS_BFO, ads.ddsbfo_i, ads.ddsbfo_q, psize); //BFO
					softdds_runf(DDS_TX_LO, ads.ddslo_i_tx, ads.ddslo_q_tx,
					        SAMPLE_SIZE / 2); //SSB����
					for (i = 0; i < psize; i++)
					{
						/* һ�ε�������90����λ���I Q�ź� */
						Data_I[i] = ads.a_buffer[i] * ads.ddsbfo_i[i];
						Data_Q[i] = ads.a_buffer[i] * ads.ddsbfo_q[i]; //LO2_Q;

					}
					arm_biquad_cascade_df1_f32(&IIR_LPF_I, Data_I, Data_I,
					        psize);
					arm_biquad_cascade_df1_f32(&IIR_LPF_Q, Data_Q, Data_Q,
					        psize);
					arm_scale_f32(Data_I, IIR_LP_Values, Data_I, psize);
					arm_scale_f32(Data_Q, IIR_LP_Values, Data_Q, psize);

					arm_fir_interpolate_f32(&INTERPOLATE_I, (float32_t*) Data_I,
					        (float32_t*) ads.a_buffer, psize);
					arm_fir_interpolate_f32(&INTERPOLATE_Q, (float32_t*) Data_Q,
					        (float32_t*) ads.b_buffer, psize);
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						if (vfo[VFO_A].Mode == DEMOD_LSB
						        || vfo[VFO_A].Mode == DEMOD_LSB_DIG)
						{
							Data_I1[i] = ads.b_buffer[i] * ads.ddslo_q_tx[i]
							        - ads.a_buffer[i] * ads.ddslo_i_tx[i]; //LSB�±ߴ�TX
						} else if (vfo[VFO_A].Mode == DEMOD_USB
						        || vfo[VFO_A].Mode == DEMOD_USB_DIG)
						{
							Data_I1[i] = ads.b_buffer[i] * ads.ddslo_q_tx[i]
							        + ads.a_buffer[i] * ads.ddslo_i_tx[i]; //USB�ϱߴ�TX
						}
					}
					/*
					 *	Hilbert Transform
					 */
					if (ads.rx_delay < 1)
					{
						arm_fir_f32(&Hilbert_I, Data_I1, Data_I,
						        SAMPLE_SIZE / 2);
						arm_fir_f32(&Hilbert_Q, Data_I1, Data_Q,
						        SAMPLE_SIZE / 2);
					}
					ads.tx_delay = 200;/* �����ӳ�ʱ������ */
				}
			}
			//else 

			/*
			 *	AM��������ģʽ����
			 */
			if ((PTT_RT == TX || ks.cat_ptt == TX || ks.rit_tx == TX)
			        && vfo[VFO_A].Mode == DEMOD_AM)
			{
				ads.spk_vol = 0;
				ads.hp_vol = 0;
				if (ads.rx_delay > 0)
				{
					ads.tx_delay = 300;
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						ads.out_buff[i * 2] = 0;
						ads.out_buff[i * 2 + 1] = 0;
					}
					if (PTT_RT == TX || ks.rit_tx == TX)
						MIC_POWER(0);
					ads.rx_delay--;
					if (ads.rx_delay < 150)
						TR_CONTROL(CONTROL_TX);/* ���俪�� */
					mic_gain = 1.0f;
				}
				if (ads.rx_delay < 100)
				{
					if (ads.rx_delay < 1)
						ads.rx_delay = 0;
//					if(ads.rx_delay>1)
//					{
//						mic_gain=1.0f;//(10-ads.rx_delay)*0.1f;
//					}
					/*��ȡ���Ͳ����� */
					if (ks.bt_key == 1 && ks.cat_ptt == TX)
						arm_fir_decimate_f32(&DECIMATE_Q, (float32_t*) Data_Q1,
						        ads.a_buffer, SAMPLE_SIZE / 2);
					else
						arm_fir_decimate_f32(&DECIMATE_I, (float32_t*) Data_I1,
						        ads.a_buffer, SAMPLE_SIZE / 2);

					arm_biquad_cascade_df1_f32(&IIR_AF,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.a_buffer, psize); //LP��ͨ�˲� Qͨ��
					arm_scale_f32(ads.a_buffer,
					        AF_ScaleValues * mic_gain * sd.mic_gain,
					        ads.a_buffer, psize);
					//arm_max_f32(ads.a_buffer, psize+1, (float32_t *)&calc, 0);/* AGC��Ʒ����ȡ���ֵ */
					arm_mean_f32(ads.a_buffer, psize, (float32_t*) &calc);
					/*
					 *	ALC
					 */

					if (calc < 0)
						calc *= -1.0f;
					calc *= 3;
					if (calc > ads.cw_gain[1])
					{
						calc = calc - ads.cw_gain[1];
						calc = calc / ads.cw_gain[1];
						mic_gain -= mic_gain * calc * 0.1f;
						if (mic_gain < 0.01f)
							mic_gain = 0.01f;
					} else
					//if(calc < cw_gain[1])	
					{
						calc = ads.cw_gain[1] - calc;
						calc = calc / ads.cw_gain[1];
						mic_gain += mic_gain * calc * 0.001f;
						if (mic_gain > 2.0f)
							mic_gain = 2.0f;
					}

					arm_fir_interpolate_f32(&INTERPOLATE_I,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.b_buffer, psize);
					/*	
					 *	AM���ƹ�ʽ  Sam(n) = Ac[1+m(n)]*Cos(2��*fc*n/k)
					 *					   =AF*AC*LO + AC*LO
					 *	Ac �ز������m(n) �����ź�
					 */
					softdds_runf(DDS_TX_LO, ads.ddslo_i_tx, ads.ddslo_q_tx,
					        SAMPLE_SIZE / 2); //SSB����
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						Data_I1[i] = (ads.b_buffer[i] + ads.cw_gain[1])
						        * ads.ddslo_i_tx[i] / ads.cw_gain[2]; //
						Data_Q1[i] = (ads.b_buffer[i] + ads.cw_gain[1])
						        * ads.ddslo_q_tx[i] / ads.cw_gain[2]; //ads.cw_gain[2]
					}
					/*
					 *	ALC
					 */
					calc = ads.pow;/* ALCȡ�� */
					if (calc < 0)
						calc *= -1;
					if (calc > sd.Pow)
					{
						calc = calc - sd.Pow;
						calc = calc / sd.Pow;
						arm_sqrt_f32(calc, (float32_t*) &calc);
						cw_alc_gain -= cw_alc_gain * calc * 0.01f;
						if (cw_alc_gain < 0.1f)
							cw_alc_gain = 0.1f;
					} else
					//if(calc < pow_v)	
					{
						calc = sd.Pow - calc;
						calc = calc / sd.Pow;
						arm_sqrt_f32(calc, (float32_t*) &calc);
						cw_alc_gain += cw_alc_gain * calc * 0.0001f;
						if (cw_alc_gain > 1.0f)
							cw_alc_gain = 1.0f;
					}
					if (ads.rx_delay < 1)
					{
						for (i = 0; i < SAMPLE_SIZE / 2; i++)
						{
							Data_I[i] = Data_I1[i] * cw_alc_gain; //
							Data_Q[i] = Data_Q1[i] * cw_alc_gain; //
						}
					}
					ads.tx_delay = 200;/* �����ӳ�ʱ������ */
				}
			}
			//else 

			/*
			 *	FM��Ƶ����ģʽ����
			 */
			if ((PTT_RT == TX || ks.cat_ptt == TX || ks.rit_tx == TX)
			        && vfo[VFO_A].Mode == DEMOD_FM)
			{
				ads.spk_vol = 0;/* ��Ƶ�ر� */
				ads.hp_vol = 0;
				//ads.spk_vol =0;
				if (ads.rx_delay > 0)
				{
					ads.tx_delay = 300;
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						ads.out_buff[i * 2] = 0;
						ads.out_buff[i * 2 + 1] = 0;
					}
					if (PTT_RT == TX || ks.rit_tx == TX)
						MIC_POWER(0);
					ads.rx_delay--;
					if (ads.rx_delay < 150)
						TR_CONTROL(CONTROL_TX);/* ���俪�� */
					mic_gain = 0.33f;
				}
				if (ads.rx_delay < 100)
				{
					if (ads.rx_delay < 1)
						ads.rx_delay = 0;
					/*��ȡ���Ͳ����� */
					if (ks.bt_key == 1 && ks.cat_ptt == TX)
						arm_fir_decimate_f32(&DECIMATE_Q, (float32_t*) Data_Q1,
						        ads.a_buffer, SAMPLE_SIZE / 2);
					else
						arm_fir_decimate_f32(&DECIMATE_I, (float32_t*) Data_I1,
						        ads.a_buffer, SAMPLE_SIZE / 2);

					arm_biquad_cascade_df1_f32(&IIR_AF,
					        (float32_t*) ads.a_buffer,
					        (float32_t*) ads.a_buffer, psize); //LP��ͨ�˲� Qͨ��
					arm_scale_f32((float32_t*) ads.a_buffer,
					        AF_ScaleValues * sd.mic_gain * fm_gain * 0.0001f,
					        (float32_t*) ads.a_buffer, psize);
					/*	Ƶƫ����	  */
					arm_max_f32(ads.a_buffer, SAMPLE_SIZE, &varI_1, 0);/* AGC��Ʒ����ȡ���ֵ */
					arm_min_f32(ads.a_buffer, SAMPLE_SIZE, &varI_0, 0);/* AGC��Ʒ����ȡ���ֵ */

					calc = (varI_1 - varI_0);

					if (calc > sd.fm_offset)
					{
						calc = calc - sd.fm_offset;
						calc = calc / sd.fm_offset;
						fm_gain -= fm_gain * calc * 0.1f;
						if (fm_gain < 0.1f)
							fm_gain = 0.1f;
					} else
					//if((u16)calc < sd.Pow )
					{
						calc = sd.fm_offset - calc;
						calc = calc / sd.fm_offset;
						fm_gain += fm_gain * calc * 0.001f;
						if (fm_gain > 1.0f)
							fm_gain = 1.0f;
					}
					//arm_fir_interpolate_f32(&INTERPOLATE_Q,(float32_t *)ads.a_buffer, (float32_t *)ads.b_buffer, psize);
					/*	
					 *	FM���ƹ�ʽ  S(n) = A*Cos[2��LO*n/k+sin[2��*AF*n/k]
					 *					= Cos[2��((X(n)+X(n-1))/k]*Cos[2��*LO1*n/k]
					 *				      -Sin[2��((X(n)+X(n-1))/k]*Sin[2��*LO1*n/k]
					 */
					softdds_runf(DDS_TX_LO, ads.ddslo_i_tx, ads.ddslo_q_tx,
					        SAMPLE_SIZE / 2); //SSB����
					for (i = 0; i < psize; i++)
					{
						A++;
						if (A > SAMPLE_SIZE / 2)
							A = 0;
						/* (X(n)+X(n-1) */
						Data_FM_I[i] = ads.a_buffer[i] + FM_I[1];

						Data_I1[i] = arm_cos_f32(
						        PI_CALC * Data_FM_I[i] * A / 8);
						Data_Q1[i] = arm_sin_f32(
						        PI_CALC * Data_FM_I[i] * A / 8);

						FM_I[1] = ads.a_buffer[i];
					}
					/* �ڲ��ͨ�˲������ƴ��� */
					arm_fir_interpolate_f32(&INTERPOLATE_I,
					        (float32_t*) Data_I1, (float32_t*) ads.a_buffer,
					        psize);
					arm_fir_interpolate_f32(&INTERPOLATE_Q,
					        (float32_t*) Data_Q1, (float32_t*) ads.b_buffer,
					        psize);
					for (i = 0; i < SAMPLE_SIZE / 2; i++)
					{
						Data_I1[i] = ads.a_buffer[i] * ads.ddslo_i_tx[i]
						        - ads.b_buffer[i] * ads.ddslo_q_tx[i];
						Data_I1[i] *= ads.cw_gain[1] * cw_alc_gain
						        / ads.cw_gain[2] * 6.0f;
					}
					/* 
					 *	Hilbert Transform
					 */
					if (ads.rx_delay < 1)
					{
						arm_fir_f32(&Hilbert_I, Data_I1, Data_I,
						        SAMPLE_SIZE / 2);
						arm_fir_f32(&Hilbert_Q, Data_I1, Data_Q,
						        SAMPLE_SIZE / 2);
					}
					/*
					 *	ALC
					 */
					calc = ads.pow;/* ALCȡ�� */
					if (calc < 0)
						calc *= -1;
					if (calc > sd.Pow)
					{
						calc = calc - sd.Pow;
						calc = calc / sd.Pow;
						arm_sqrt_f32(calc, (float32_t*) &calc);
						cw_alc_gain -= cw_alc_gain * calc * 0.01f;
						if (cw_alc_gain < 0.01f)
							cw_alc_gain = 0.01f;
					} else
					//if(calc < pow_v)	
					{
						calc = sd.Pow - calc;
						calc = calc / sd.Pow;
						arm_sqrt_f32(calc, (float32_t*) &calc);
						cw_alc_gain += cw_alc_gain * calc * 0.0001f;
						if (cw_alc_gain > 1.0f)
							cw_alc_gain = 1.0f;
					}
//					for(i=0;i<SAMPLE_SIZE/2;i++)
//					{	
//						Data_I[i] = Data_I1[i]*cw_alc_gain;//
//						Data_Q[i] = Data_Q1[i]*cw_alc_gain;//
//					}
					//Dac2_out_I[i] = (u16)agc_gain;
					ads.tx_delay = 200;/* �����ӳ�ʱ������ */
				}
			}
		}
		/*
		 *	TX I/Q������λ����
		 */
		if (TR_READ == CONTROL_TX)
		{
			arm_scale_f32(Data_I, ads.tx_amp[vfo[VFO_A].Band_id], Data_I,
			        SAMPLE_SIZE / 2);					//����Iͨ������
			arm_scale_f32(Data_I, ads.tx_phase[vfo[VFO_A].Band_id],
			        ads.b_buffer, SAMPLE_SIZE / 2);				//ȡһ���Iͨ���ź�

			arm_add_f32(Data_Q, ads.b_buffer, Data_Q, SAMPLE_SIZE / 2);	// ȡһ���Iͨ���ź���Q���
			//arm_sub_f32((float32_t *)Data_Q, (float32_t *)Data_Q, (float32_t *)ads.a_buffer, SAMPLE_SIZE/2);
			//arm_scale_f32(Data_I,ads.tx_amp[vfo[VFO_A].Band_id] , Data_I,SAMPLE_SIZE/2);//����Iͨ������

			for (i = 0; i < SAMPLE_SIZE / 2; i++)
			{
				ads.out_buff[i * 2] = Data_I[i];
				ads.out_buff[i * 2 + 1] = Data_Q[i];
			}
		}
		/*
		 *	���ݼ��ص�DAC
		 */

		if (DMA2->LISR & (1 << 27))	//�ȴ�DMA2_Steam3�������
		{
			DMA2->LIFCR |= 1 << 27;	//���DMA2_Steam3������ɱ�־

			if (DMA2_Stream3->CR & (1 << 19))
			{
				for (i = 0; i < SAMPLE_SIZE; i++)
				{
					DAC_Buff0[i] = (int32_t) ads.out_buff[i];
				}
			} else
			{
				for (i = 0; i < SAMPLE_SIZE; i++)
				{
					DAC_Buff1[i] = (int32_t) ads.out_buff[i];
				}
			}
			if (TR_READ == CONTROL_TX)
			{
				for (i = 0; i < SAMPLE_SIZE / 2; i++)
				{
					if (spe.fft_flang == 0)
					{
						spe.fft_num++;	//FFT���ݲ���������
						if (spe.fft_num >= FFT_LENGTH * 2)
						{
							spe.fft_num = 0;	//FFT���ݲ���������
							spe.fft_flang = 1;
						}
						fft_buf[spe.fft_num * 2] = (float32_t) ads.out_buff[i
						        * 2];
						fft_buf[spe.fft_num * 2 + 1] =
						        (float32_t) ads.out_buff[i * 2 + 1];
					}
				}
			}
		}

		if (DMA1->LISR & (1 << 11))	//�ȴ�DMA2_Steam0�������
		{
			DMA1->LIFCR |= 1 << 11;	//���DMA2_Steam1������ɱ�־

			if (DMA1_Stream1->CR & 1 << 19)
			{
				for (i = 0; i < SAMPLE_SIZE / 2; i++)
				{
					DAC2_Buff0[i] = (int16_t) ads.si_i[i] + DAC_OFFSET_LEVEL;
				}
			}
			else
			{
				for (i = 0; i < SAMPLE_SIZE / 2; i++)
				{
					DAC2_Buff1[i] = (int16_t) ads.si_i[i] + DAC_OFFSET_LEVEL;
				}
			}
		}

		if (DMA1->LISR & (1 << 5))	//�ȴ�DMA2_Steam0�������
		{
			float32_t temp0[ADC_SIZE];
			float32_t temp1[ADC_SIZE];
			float32_t temp2[ADC_SIZE];
			float32_t temp3[ADC_SIZE];

			DMA1->LIFCR |= 1 << 5;	//���DMA2_Steam1������ɱ�־

			if (DMA1_Stream0->CR & 1 << 19)
			{
				for (i = 0; i < ADC_SIZE; i++)
				{
					temp0[i] = (float32_t) ADC1_Buff0[i * 4];
					temp1[i] = (float32_t) ADC1_Buff0[i * 4 + 1];
					temp2[i] = (float32_t) ADC1_Buff0[i * 4 + 2];
					temp3[i] = (float32_t) ADC1_Buff0[i * 4 + 3];
				}
			}
			else
			{
				for (i = 0; i < ADC_SIZE; i++)
				{
					temp0[i] = ADC1_Buff1[i * 4];
					temp1[i] = ADC1_Buff1[i * 4 + 1];
					temp2[i] = ADC1_Buff1[i * 4 + 2];
					temp3[i] = ADC1_Buff1[i * 4 + 3];
				}
			}

			arm_mean_f32(temp3, ADC_SIZE, &ads.power_v);
			ads.power_v *= 0.0038f;/* 16λADC */

			arm_mean_f32(temp0, ADC_SIZE, &ads.fwd);
			ads.pow = ads.fwd * ads.pow_corre * ads.fwd * ads.pow_corre * 0.2f
			        * (3.0f - ads.cw_gain[2]);/* 16λADC */
			//ads.pow = ads.fwd * 0.01221f * ads.fwd *0.01221f * 0.2f;/* 12λADC */
			arm_mean_f32(temp1, ADC_SIZE, &ads.ref);

			ads.pwr_rev = ads.ref * 0.00068f * ads.ref * 0.00068f * 0.2f
			        * (3.0f - ads.cw_gain[2]);/* 16λADC */

			if (ads.pow > 1)
			{
				ads.swr = (u8) ((1 + ads.ref / ads.fwd) /
								(1 - ads.ref / ads.fwd) * 10);
			}
			
			arm_mean_f32(temp2, ADC_SIZE, &ads.vox);

			UART3_Task();
			UART_Task();
		}
	}
}
/*
 *	DMA1->Stream5�жϳ���
 *	DSP�ӹ������������ͨ��DMA���䵽DAC1��DAC1->DHR12R1;
 *	����˫�����Է����ݶ�ʧ��
 */
void DMA1_Stream0_IRQHandler(void)
{
	if (DMA1->LISR & (1 << 5))	//�ȴ�DMA2_Steam0�������
	{
		DMA1->LIFCR |= 1 << 5;	//���DMA2_Steam1������ɱ�־

		if (DMA1_Stream0->CR & 1 << 19)
		{
			ads.fwd     = (ADC1_Buff0[0] + ADC1_Buff0[4] + ADC1_Buff0[ 8] + ADC1_Buff0[12]) >> 2;
			ads.ref     = (ADC1_Buff0[1] + ADC1_Buff0[5] + ADC1_Buff0[ 9] + ADC1_Buff0[13]) >> 2;
			ads.vox     = (ADC1_Buff0[2] + ADC1_Buff0[6] + ADC1_Buff0[10] + ADC1_Buff0[14]) >> 2;
			ads.power_v = (ADC1_Buff0[3] + ADC1_Buff0[7] + ADC1_Buff0[11] + ADC1_Buff0[15]) >> 2;
		}
		else
		{
			ads.fwd     = (ADC1_Buff1[0] + ADC1_Buff1[4] + ADC1_Buff1[ 8] + ADC1_Buff1[12]) >> 2;
			ads.ref     = (ADC1_Buff1[1] + ADC1_Buff1[5] + ADC1_Buff1[ 9] + ADC1_Buff1[13]) >> 2;
			ads.vox     = (ADC1_Buff1[2] + ADC1_Buff1[6] + ADC1_Buff1[10] + ADC1_Buff1[14]) >> 2;
			ads.power_v = (ADC1_Buff1[3] + ADC1_Buff1[7] + ADC1_Buff1[11] + ADC1_Buff1[15]) >> 2;
		}

		ads.power_v *= 0.00383f;/* 16λADC */
		ads.pow = ads.fwd * 0.001f * ads.fwd * 0.001f * 0.2f;/* 16λADC */

		if (ads.pow > 1)
		{
			ads.swr =	(ads.fwd * ads.fwd + ads.ref * ads.ref * 1.4f) * 10 /
						(ads.fwd * ads.fwd - ads.ref * ads.ref * 1.4f);
		}

		UART3_Task();
		UART_Task();
	}
}
/*
 ********************************************************************************
 * Ƶ�׼��ٲ�ͼ��ʾ
 ********************************************************************************
 */
typedef struct
{
	u16 x;/* ���X*/
	u16 y;/* ���Y*/
	//u8 font;
	char *text;/* ��ʾ���ı� */
} SPE_Text_typedef;
const SPE_Text_typedef Spe_text[11] = { { 10, 222, "5" }, { 40, 222, "4" }, {
        70, 222, "3" }, { 100, 222, "2" }, { 130, 222, "1" }, { 160, 222, "0" },
        { 190, 222, "1" }, { 220, 222, "2" }, { 250, 222, "3" },
        { 280, 222, "4" }, { 310, 222, "5" }, };
/*
 *********************************************************************************************************
 *  �� �� ��: Power_Phase_Radians
 *  ����˵��: ����λ
 *  �� �Σ�_usFFTPoints FFT������ uiCmpValue ��ֵ
 *  �� �� ֵ: ��
 *********************************************************************************************************
 */
void Power_Phase_Radians(uint16_t _usFFTPoints, float32_t *intput_i,
        float32_t *intput_q, float32_t uiCmpValue)
{
	int16_t IX, IY, QX, QY;
	uint16_t i;
	float32_t phase_i, phase_q;
	float32_t mag_i, mag_q;
	for (i = 0; i < _usFFTPoints; i++)
	{
		IX = intput_i[i * 2]; /* ʵ�� */
		IY = intput_i[i * 2 + 1]; /* �鲿 */
		QX = intput_q[i * 2]; /* ʵ�� */
		QY = intput_q[i * 2 + 1]; /* �鲿 */
		phase_i = atan2(IY, IX); /* atan2���Ľ����Χ��(-pi, pi], ������ */
		phase_q = atan2(QY, QX); /* atan2���Ľ����Χ��(-pi, pi], ������ */
		arm_sqrt_f32((float32_t) (IX * IX + IY * IY), &mag_i); /* ��ģ */
		arm_sqrt_f32((float32_t) (QX * QX + QY * QY), &mag_q); /* ��ģ */
		if (uiCmpValue > mag_i)
		{
			spe.Phase_i[i] = 0;
		} else
		{
			spe.Phase_i[i] = phase_i * 180.0f / 3.1415926f; /* �����Ľ���ɻ���ת��Ϊ�Ƕ� */
		}
		if (uiCmpValue > mag_q)
		{
			spe.Phase_q[i] = 0;
		} else
		{
			spe.Phase_q[i] = phase_i * 180.0f / 3.1415926f; /* �����Ľ���ɻ���ת��Ϊ�Ƕ� */
		}
	}
}
/*
 * Ƶ����ʾ������
 * ʵ��Ƶ���Լ��ٲ�ͼ��ʾ��
 * ��ڲ�����
 * x yΪ��ʼ���꣬����y�ǵײ���������������ʾ���߶���spe_h+falls_h��
 * width����ȣ�
 * spe_h: Ƶ�׸߶ȣ�
 * falls_h���ٲ�ͼ�߶ȣ�
 * mode: ������
 */

float input[1024];
int16_t falls[60][320];
void Spectrum_Dispaly(u16 x, u16 y, u16 width, u16 falls_h)
{

	u16 x0, x1, y0;
	u16 fc, bc, fall_bc;
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
	static float32_t out_lsb[FFT_LENGTH * 2];
	//static float32_t out_usb[FFT_LENGTH*2];

	if (spe.fft_flang != 0)
	{
		for (i = 0; i < FFT_LENGTH; i++)
		{
			/* ������ */
			buff_lsb[i] = fft_buf[i * 2]
			        * arm_sin_f32( PI * 2 * i / FFT_LENGTH);
			//buff_usb[i] = fft_buf [i*2+1] ;//* arm_sin_f32( PI*2 *i/FFT_LENGTH);
			/*	Hanning ������ 1.36 */
			//spe_buf [i*2] = (float32_t)((1.0f-arm_cos_f32(PI*2.0f * (float32_t)i / 1023.0f)) * fft_buf [i]);
			/* Blackman ���������� 1.75 */
			//spe_buf [i*2] =(0.42659f - (0.49656f*arm_cos_f32((2.0f*PI*(float32_t)i)/FFT_LENGTH)) + (0.076849f*arm_cos_f32((4.0f*PI*(float32_t)i)/FFT_LENGTH))) * fft_buf [i];
			//spe_buf [i*2+1] =0;//fft_buf [i]*arm_cos_f32(2.0f*PI*0.01f*(float32_t)(i/96000));
		}
		arm_rfft_fast_f32(&FFT_LSB, buff_lsb, out_lsb, 0);	//FFT���㣨��4)
		//arm_rfft_fast_f32(&FFT_USB, buff_usb, out_usb,0 );	//FFT���㣨��4)
		/***ȡ��FFTģֵ***/
		arm_cmplx_mag_f32(out_lsb, buff_lsb, FFT_LENGTH / 2);
		//arm_cmplx_mag_f32(  out_usb,buff_usb,FFT_LENGTH/2);
		/*	��λ��ȡ */
		//Power_Phase_Radians(FFT_LENGTH/2,out_lsb,out_usb, 0.1f);
		//Power_Phase_Radians(FFT_LENGTH/2,out_usb,out_usb, 0.1f);
		/*
		 *	FFTȡģ����512�㣬����93.75Hz
		 *	1/93.75=0.010667f
		 *	LCD��320��,����125Hz������ϵ��1.3333f,
		 *	1/150=0.006667f
		 */
		for (i = 0; i < 384; i++)
		{
			spe_buf[i] = buff_lsb[(u16) (i * 1.3333f)];
		}
		//
		if (++h > falls_h - 1)
			h = 0;
		for (k = 0; k < width; k++)
		{

			x0 = sd.Dsp_Bfo * 0.008f + Fil_p[sd.spe_fil_id].spe_width / 2;
			x1 = sd.Dsp_Bfo * 0.008f - Fil_p[sd.spe_fil_id].spe_width / 2;
			if (sd.spe_fil_mode > 0)		//LSB
			{
				if ((k + x) >= (SPE_CENTER - x0) && (k + x) < (SPE_CENTER - x1))
				{
					bc = GREEN3;
					//fc=GREEN5 |(u16)input[k];//BLUE1 |YELLOW2;
					fc = GREEN3;
					fall_bc = GREEN3;
				} else
				{
					bc = BLACK;
					fc = BLUE | YELLOW2;
					fall_bc = 0;
				}
			} else if (sd.spe_fil_mode == 0)		//AM/FM/CW
			{
				if ((k + x) >= (SPE_CENTER - Fil_p[sd.spe_fil_id].spe_width / 2)
				        && (k + x)
				                <= (SPE_CENTER
				                        + Fil_p[sd.spe_fil_id].spe_width / 2))
				{
					bc = GREEN3;
					//fc=GREEN5 |(u16)input[k];//BLUE1 |YELLOW2;
					fc = GREEN3;
					fall_bc = GREEN3;
				} else
				{
					bc = BLACK;
					fc = BLUE | YELLOW2;
					fall_bc = 0;
				}
			} else if (sd.spe_fil_mode < 0)		//USB
			{
				if ((k + x) >= (SPE_CENTER + x1)
				        && (k + x) <= (SPE_CENTER + x0))
				{
					bc = GREEN3;
					//fc=GREEN5 |(u16)input[k];//BLUE1 |YELLOW2;
					fc = GREEN3;
					fall_bc = GREEN3;
				} else
				{
					bc = BLACK;
					fc = BLUE | YELLOW2;
					fall_bc = 0;
				}
			}

			/* �ɲ������ */
			for (i = (u16) input[k] & 0x3f; i > 0; i--)
			{
				Gui_DrawPoint(k + x, y - i - falls_h - 1, bc);
			}
			/* �²���������ȡ */
			input[k] = spe_buf[k + (u16) (sd.IF_1 * 0.008f) - 157] * 0.5f
			        + spe_buf[k - 1 + (u16) (sd.IF_1 * 0.008f) - 157] * 0.25f
			        + spe_buf[k + 1 + (u16) (sd.IF_1 * 0.008f) - 157] * 0.25f;
			input[k] *= (k / 30 + 5) * 0.1f;
			if (TR_READ == CONTROL_TX)
				falls[h][k] = (u32) (input[k]) >> 20;		//
			else
				falls[h][k] = (u32) (input[k]) >> (17 + sd.Riss / 16);		//
			//speput[k]=(u8)(log2((u32)input[k]))*8;
			input[k] *= 0.01f;
			arm_sqrt_f32(input[k], (float32_t*) &input[k]);
			arm_sqrt_f32(input[k], (float32_t*) &input[k]);
			input[k] -= 3;
			if (input[k] > 48)
				input[k] = 48;
			if (k > 250 && k < 320)
			{
				if (input[k] > spe.fft_max)
					spe.fft_max = (u8) input[k];
			}
			/* ���Ʋ��� */
			if (k > 0)
			{
				/* ���߱�ʶ��ʾ */
				//if(k==width/2 && i< (u16)input[k])fc=YELLOW3;
				//else
				//fc=BLUE;
				/* Ƶ����ʾ */
				//if(k==width/2 && (u16)input[k]< 40)input[k]=40;
				for (i = ((u16) input[k]); i > 0; i--)
				{
					//if(k==width/2 && i<= 40)fc=YELLOW;
					//else
					//fc=BLUE |YELLOW2;
					Gui_DrawPoint(k + x, y - i - falls_h - 1,
					        (BLUE << (i >> 2)) | fc);
				}
//				/* �ٲ�ͼ��ʾ */
				if (sd.amp_phase == 0 && ks.IF_shift == 0)
				{
					for (i = 0; i < falls_h; i++)
					{
						if (i <= h)
							Gui_DrawPoint(k + x, y + i - falls_h,
							        (falls[h - i][k] * falls[h - i][k])
							                | fall_bc);
						else if (i > h)
							Gui_DrawPoint(k + x, y + i - falls_h,
							        (falls[falls_h + h - i][k]
							                * falls[falls_h + h - i][k])
							                | fall_bc);
					}
					if ((k + x) > 1 && (k + x) < 20)
						LCD_ShowString(2, 182, GRAY3, BLUE3, 12, 1, "20k");
					if ((k + x) > 156 && (k + x) < 163)
						LCD_ShowString(157, 182, GRAY3, BLUE3, 12, 1, "0");
					if ((k + x) > 301 && (k + x) < 319)
						LCD_ShowString(302, 182, GRAY3, BLUE3, 12, 1, "20k");
				}
			}
			/* �ٲ�ͼ����ʱ�� */
			if (k == 0)
			{
				if (ads.vol_timer)
					sd.spe_fil_time = SPE_FIL_TIME;			//Ƶ�״�����ʾ�ظ���ʱ��־
				//if(sd.spe_fil_time>0)sd.spe_fil_time--;if(sd.spe_fil_time<1)sd.spe_fil_time=0;
				if (ads.vol_timer == 0)
				{
					ui_Spe_Filter_Display(sd.spe_fil_id, sd.spe_fil_mode,
					        sd.spe_fil_time, GREEN3, ks.rx_adjuset_key);

					for (y0 = 130; y0 < 182; y0 += 17)
					{
						for (i = 1; i < 317; i++)
							Gui_DrawPoint(i, y0, GRAY4);
					}
					for (i = 1; i < 320; i += 79)
					{
						for (y0 = 130; y0 < 180; y0 += 1)
							Gui_DrawPoint(i, y0, GRAY4);
					}

					if (sd.spe_fil_time > 0)
						sd.spe_fil_time--;
					if (sd.spe_fil_time < 1)
						sd.spe_fil_time = 0;

					LCD_ShowNum(265, 131, GREEN, BLACK, 3, 16, ads.agc_gain);
				}
				if (ks.IF_shift == 1)
				{
					LCD_ShowString(30, 182, YELLOW, BLUE1, 16, 0, "  IQ_BL   ");
					LCD_ShowNum(220, 182, GRAY1, BLUE1, 9, 16,
					        Rx_amp_user[ads.rx_amp_band]);
				}
//				if(clk0==0)
//				{
//					LCD_ShowString(257,130,YELLOW ,BLACK , 16, 0, "-");
//					LCD_ShowString(290,130,YELLOW ,BLACK , 16, 0, "db");
//					clk0=1;
//				}
			}
		}
		spe.fft_flang = 0;
	}
}

//�ⲿ�ж�0�������
void EXTI15_10_IRQHandler(void)
{
	//if(CW_K_DI==0)EXTI_D1->PR1=1<<10;  //���LINE0�ϵ��жϱ�־λ 
	//if(CW_K_DA==0)EXTI_D1->PR1=1<<11;  //���LINE0�ϵ��жϱ�־λ 	
	/*
	 *	di/da��ת��
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
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 && cw_Auto_key.dah ==0)/* �Զ���ģʽ di�μ����ռ������� */
//	{
//		cw_Auto_key.test_state =3;
//		//tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
//	}
//	else 
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 )//&& cw_Auto_key.dah ==1)/* �Զ���ģʽ di�μ����� */
//	{
//		cw_Auto_key.test_state =2;
//		//tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
//	}
//	else					
//	if(PTT_RT == 1 /*&& cw_Auto_key.dit == 1*/ && cw_Auto_key.dah ==0)/* �Զ���ģʽ da�ռ����� */
//	{
//		cw_Auto_key.test_state =1;
//		//tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
//	}
//	else
//	if(PTT_RT == 1 && cw_Auto_key.dit == 1 && cw_Auto_key.dah ==1)/* �Զ���ģʽ da�ռ����ռ����ɿ� */
//	{
//		cw_Auto_key.test_state =0;
//	}
//	if(cw_Auto_key.now_state != cw_Auto_key.test_state )cw_Auto_key.now_state = cw_Auto_key.test_state;
	//
	if (ks.rt_rx == 0)
	{
//		if((CW_K_DI==0 || CW_K_DA==0 )&&vfo[VFO_A].Mode < DEMOD_LSB)
//		{
//			//WM8978_MIC_Gain(0);
//			WM8978_Output_Cfg(1,1);
//		}
		if (PTT_RT == TX && vfo[VFO_A].Mode >= DEMOD_LSB)
		{
			ads.spk_vol = 0;
			ads.hp_vol = 0;
			WM8978_SPKvol_Set(0);
			WM8978_HPvol_Set(0, 0);
		}
	}
	//si5351aSetFrequency_LoA(sd.Tcxo, (vfo[VFO_A].Freq - ads.dsp_IF + sd.rit)*4);
	//if(CW_K_DI==0)
	//EXTI_D1->PR1=1<<10;  //���LINE0�ϵ��жϱ�־λ
	//if(CW_K_DA==0)
	//EXTI_D1->PR1=1<<11;  //���LINE0�ϵ��жϱ�־λ
	//if(PTT_RT == 0)
	EXTI_D1->PR1 |= 1 << 14;
//	EXTI_D1->PR1=1<<11;  //���LINE2�ϵ��жϱ�־λ   
//	/*
//	*	di/da��ת��
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
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 && cw_Auto_key.dah ==0)/* �Զ���ģʽ di�μ����ռ������� */
//	{
//		cw_Auto_key.now_state =3;
//		//tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
//	}
//	else 
//	if(PTT_RT == 1 && cw_Auto_key.dit == 0 && cw_Auto_key.dah ==1)/* �Զ���ģʽ di�μ����� */
//	{
//		cw_Auto_key.now_state =2;
//		//if(cw_Auto_key.now_state ==2)
//		//{
//		//	if(cw_Auto_key.dah ==0)
//		//		cw_Auto_key.mm_state =3;
//		//};
//		//tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
//	}
//	else					
//	if(PTT_RT == 1 && cw_Auto_key.dit == 1 && cw_Auto_key.dah ==0)/* �Զ���ģʽ da�ռ����� */
//	{
//		cw_Auto_key.now_state =1;
//		//if(cw_Auto_key.now_state ==1)
//		//{
//		//	if(cw_Auto_key.dit == 0)
//		//		cw_Auto_key.mm_state =3;
//		//};
//		//tx_delay = sd.CW_Delay;/* �����ӳ�ʱ������ */
//	}
//	else
//	if(PTT_RT == 1 && cw_Auto_key.dit == 1 && cw_Auto_key.dah ==1)/* �Զ���ģʽ da�ռ����ռ����ɿ� */
//	{
//		cw_Auto_key.now_state =0;
//	}	 
}

