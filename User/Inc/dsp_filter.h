#ifndef _DSP_FRLTER_H
#define _DSP_FRLTER_H
#include "sys.h"
#include "arm_math.h"
#include "filter.h"
/*
*
*/
#define DECIMATION_RATE_8KHZ		12		//
#define DECIMATION_RATE_12KHZ		8		// Decimation/Interpolation rate in receive function for 10 kHz sample rate
#define DECIMATION_RATE_24KHZ		4		// Decimation/Interpolation rate in receive function for 15 kHz sample rate
#define	DECIMATION_RATE_32KHZ		3		// Decimation/Interpolation rate in receive function for 20 kHz sample rate
#define	DECIMATION_RATE_48KHZ		2		// Decimation/Interpolation rate in receive function for 40 kHz sample rate
#define DECIMATION_RATE_96KHZ		1		// Deimcation/Interpolation rate in receive function for 80 kHz sample rate (e.g. no decimation!)
//
enum
{
	DEMOD_CW_LSB=0,
	DEMOD_CW_USB,
	DEMOD_LSB,
	DEMOD_LSB_DIG,
	DEMOD_USB,
	DEMOD_USB_DIG,
	DEMOD_AM,
	DEMOD_FM
};
enum
{
    CW_50HZ = 1,
    CW_100HZ,
    CW_200HZ,
    CW_300HZ,
    CW_500HZ,
	CW_800HZ,
	SSB_1K5,
    SSB_1K8,
    SSB_2K1,
    SSB_2K4,
    SSB_2K7,
	SSB_3K,
    AM_6K,
    AM_9K,
	FM_5K,  	
    FM_10K,	
	//NFM_5K,  	
    //NFM_10K,
	//AUDIO_HPF,
    AUDIO_OFF,
    AUDIO_FILTER_NUM
};
enum
{
    FILTER_MODE_CW = 0,
    FILTER_MODE_SSB=4,
    FILTER_MODE_AM=8,
	//FILTER_MODE_NFM=12,
    FILTER_MODE_FM=12,
    FILTER_MODE_MAX
};
//
#define AUDIO_DEFAULT_FILTER        AUDIO_2P4KHZ	//默认滤波器
#define AUDIO_MIN_FILTER        0					//定义最低滤波器编号
#define AUDIO_MAX_FILTER        (AUDIO_FILTER_NUM)//定义最高使用的过滤器编号-1
//
typedef struct FilterDescriptor_s
{
    const uint8_t id;
    char* name;
    const uint16_t width;
} FilterDescriptor;

extern const FilterDescriptor FilterInfo[AUDIO_FILTER_NUM];
extern uint16_t filterpath_mode_map[FILTER_MODE_MAX];

typedef struct FilterPathDescriptor_s
{
    const uint8_t id;				//滤波器ID
    const char* name;				//屏幕显示的滤波器名称
    const uint16_t mode;			//工作模式
    const uint8_t filter_select_id; //各种模式下滤波器数量
	const uint8_t sample_rate;	//抽取和内插的系数：1，2，4，6
	const arm_fir_decimate_instance_f32* Decimate_LP;//抽取滤波器 I
	const arm_fir_interpolate_instance_f32* Interpolate_LP;//内插滤波器 I
	//const arm_fir_instance_f32* FIR_LP_instance;
    //const arm_iir_lattice_instance_f32* Q_instance;	//IIR田字格滤波器指针 Q通道
	const arm_biquad_casd_df1_inst_f32* LP_instance; 
//	const arm_biquad_casd_df1_inst_f32* Q_instance; 
	float32_t Lpf_ScaleValues;
//	const arm_biquad_casd_df1_inst_f32* HPF_I_instance;	//IIR田字格滤波器指针 I通道
	const arm_biquad_casd_df1_inst_f32* AF_instance;	//IIR田字格滤波器指针 I通道
	const float32_t AF_ScaleValues;
	//const arm_biquad_casd_df1_inst_f32* RISS_BPF_instance;	//IIR田字格滤波器指针 I通道
	//const float32_t Riss_ScaleValues;
//	const arm_biquad_casd_df1_inst_f32* I_AA_instance; 
//	const arm_biquad_casd_df1_inst_f32* Q_AA_instance; 
	//const arm_iir_lattice_instance_f32* TX_instance;	//IIR滤波器指针 I通道
	//const float32_t TX_ScaleValues;
} FilterPathDescriptor;
#define FILTER_PATH_NUM 19
extern const FilterPathDescriptor FilterPathInfo[FILTER_PATH_NUM];

uint8_t DSP_ApplicableFilterPath(const uint8_t filter_mode, const uint8_t current_path);
//uint8_t DSP_ApplicableFilterPath(const uint8_t filter_mode, const uint8_t current_path);

#endif
