#ifndef _FRLTER_h_
#define _FRLTER_h_
#include "sys.h"
#include "arm_math.h"
//#include "arm_math.h"
//#include "arm_const_structs.h"

/*
************************************************************************************************************************

************************************************************************************************************************
*/
//#define IIR_HPF_numStages 4
//#define IIR_LATTICE_FILTER// arm_iir_lattice_f32
//#define IIR_DIRECTFORM_I_FILTER // arm_biquad_cascade_df1_f32
/*
*	RX Hilbert Transform
*/
extern const arm_fir_instance_f32 Hilbert_Transform_I;
extern const arm_fir_instance_f32 Hilbert_Transform_Q;
/*
*	TX Hilbert Transform
*/
extern const arm_fir_instance_f32 Hilbert_tx_I;
extern const arm_fir_instance_f32 Hilbert_tx_Q;
/*
*	Decimate Filter
*/
extern const arm_fir_decimate_instance_f32 FirDecimateMinLPF;
extern const arm_fir_interpolate_instance_f32 FirInterpolate_4_5k;
extern const arm_fir_decimate_instance_f32 FirDecimate_5k;
extern const arm_fir_decimate_instance_f32 FirDecimate_10k;
extern const arm_fir_interpolate_instance_f32 FirInterpolate_5k;
extern const arm_fir_interpolate_instance_f32 FirInterpolate_10k;

//extern const arm_fir_instance_f32 FIR_200hz_LPF;
//extern const arm_fir_instance_f32 FIR_500hz_LPF;
//extern const arm_fir_instance_f32 FIR_800hz_LPF;
//extern const arm_fir_instance_f32 FIR_1k3_LPF;

//extern const arm_fir_instance_f32 FIR_1k8_LPF;
//extern const arm_fir_instance_f32 FIR_2k1_LPF;
//extern const arm_fir_instance_f32 FIR_2k4_LPF;
//extern const arm_fir_instance_f32 FIR_2k7_LPF;
/*
*	instance Filter
*/
extern const arm_fir_instance_f32 FIR_3k_LPF;
extern const arm_fir_instance_f32 FIR_3k5_LPF;
extern const arm_fir_instance_f32 FIR_4k_LPF;
extern const arm_fir_instance_f32 FIR_4k5_LPF;
extern const arm_fir_instance_f32 FIR_12k_BPF;

extern const arm_biquad_casd_df1_inst_f32 IIR_sql_HP;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_AF;
extern const arm_biquad_casd_df1_inst_f32 SSB_dfI_AF;
/*
*	Riss  Filter
*/
extern const arm_biquad_casd_df1_inst_f32 RISS_BPF; 
extern const float32_t Riss_ScaleValues;
/*
*	CW Filter
*/
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_50hz_LPF; 
extern const float32_t ScaleValues_50hz;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_100hz_LPF; 
extern const float32_t ScaleValues_100hz;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_200hz_LPF; 
extern const float32_t ScaleValues_200hz;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_300hz_LPF; 
extern const float32_t ScaleValues_300hz;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_500hz_LPF; 
extern const float32_t ScaleValues_500hz;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_800hz_LPF; 
extern const float32_t ScaleValues_800hz;
/*
*	RX_SSB Filter
*/
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_1k5_LPF; 
extern const float32_t ScaleValues_1k5;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_1k8_LPF; 
extern const float32_t ScaleValues_1k8;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_2k1_LPF; 
extern const float32_t ScaleValues_2k1;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_2k4_LPF; 
extern const float32_t ScaleValues_2k4;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_2k7_LPF; 
extern const float32_t ScaleValues_2k7;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_3k_LPF; 
extern const float32_t ScaleValues_3k;
/*
*	TX_SSB Filter
*/
extern const arm_biquad_casd_df1_inst_f32 IIR_TX_1k5_BPF; 
extern const float32_t ScaleTX_1k5;
extern const arm_biquad_casd_df1_inst_f32 IIR_TX_1k8_BPF; 
extern const float32_t ScaleTX_1k8;
extern const arm_biquad_casd_df1_inst_f32 IIR_TX_2k1_BPF; 
extern const float32_t ScaleTX_2k1;
extern const arm_biquad_casd_df1_inst_f32 IIR_TX_2k4_BPF; 
extern const float32_t ScaleTX_2k4;
extern const arm_biquad_casd_df1_inst_f32 IIR_TX_2k7_BPF; 
extern const float32_t ScaleTX_2k7;
extern const arm_biquad_casd_df1_inst_f32 IIR_TX_3k_BPF; 
extern const float32_t ScaleTX_3k;
extern const arm_biquad_casd_df1_inst_f32 IIR_30k_LPF; 
extern const float32_t ScaleTX_30k;
/*
*	AM Filter
*/
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_6k_LPF; 
extern const float32_t ScaleValues_6k;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_7k_LPF; 
extern const float32_t ScaleValues_7k;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_8k_LPF; 
extern const float32_t ScaleValues_8k;
extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_9k_LPF; 
extern  float32_t ScaleValues_9k;

extern const arm_biquad_casd_df1_inst_f32 IIR_dfI_12k5_LPF; 
extern const float32_t ScaleValues_12k5;
extern const float AF_ScaleValues ;
extern const float SSB_AF_ScaleValues ;
 
#endif


