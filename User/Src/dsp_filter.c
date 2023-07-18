#include "dsp_filter.h"
//#include "filter.h"
//#include "IIR.h"

enum
{
    FILTER_MASK_CW   = 1 << FILTER_MODE_CW,
    FILTER_MASK_SSB  = 1 << FILTER_MODE_SSB,
    FILTER_MASK_AM   = 1 << FILTER_MODE_AM,
    FILTER_MASK_FM   = 1 << FILTER_MODE_FM,
};

#define FILTER_MASK_ALL (FILTER_MASK_CW|FILTER_MASK_SSB|FILTER_MASK_AM|FILTER_MASK_FM)
#define FILTER_MASK_NOFM (FILTER_MASK_CW|FILTER_MASK_SSB|FILTER_MASK_AM)
#define FILTER_MASK_SSBAM (FILTER_MASK_SSB|FILTER_MASK_AM)
#define FILTER_MASK_SSBAMCW (FILTER_MASK_SSB|FILTER_MASK_AM|FILTER_MASK_CW)
#define FILTER_MASK_SSBSAM (FILTER_MASK_SSB|FILTER_MASK_AM|FILTER_MASK_SAM)
#define FILTER_MASK_SSBSAMCW (FILTER_MASK_SSB|FILTER_MASK_AM|FILTER_MASK_SAM|FILTER_MASK_CW)
#define FILTER_MASK_AMSAM (FILTER_MASK_AM|FILTER_MASK_SAM)
#define FILTER_MASK_SSBCW (FILTER_MASK_SSB|FILTER_MASK_CW)
#define FILTER_MASK_AMFM (FILTER_MASK_AM|FILTER_MASK_FM)
#define FILTER_MASK_NONE (0)
#define FILTER_MASK_SSBAMFM (FILTER_MASK_SSB|FILTER_MASK_AM|FILTER_MASK_FM)

const FilterDescriptor FilterInfo[AUDIO_FILTER_NUM] =
{
    // 	id ,	name	 ,  width
	{0,	0,	0,},
    {  CW_50HZ,   "50Hz",    50},
    {  CW_100HZ,  "100Hz",   100},
    {  CW_200HZ,  "200Hz",   200},
    {  CW_300HZ,  "300Hz",   300},
    {  CW_500HZ,  "500Hz",   500},
	{  CW_800HZ,  "800Hz",   800},
	{  SSB_1K5,	 "1.5k ",  	1500},
    {  SSB_1K8,	 "1.8k ", 	 1800},
    {  SSB_2K1, 	 "2.1k ",  2100},
    {  SSB_2K4, 	 "2.4k ",  2400},
    {  SSB_2K7, 	 "2.7k ",  2700},
	{  SSB_3K, 	 "3.0k ",  3000},
    {  AM_6K,  	 "6.0k ",  6000},
    //{  AM_7K,  	 "7.0k ",  8000},
	//{  AM_8K,  	 "8.0k ",  6000},
    {  AM_9K,  	 "9.0k ",  8000},
	{  FM_5K,  	 "5.0k ",  5000},
    {  FM_10K,	 "10.0k", 10000},
	//{  NFM_5K,   "5.0k ",  5000},
   // {  NFM_10K,	 "10.0k", 10000}
};
uint16_t filterpath_mode_map[FILTER_MODE_MAX];

const FilterPathDescriptor FilterPathInfo[ FILTER_PATH_NUM] =
{
// ID, mode, name (for display), filter_select_ID, &I_interpolation filter,&Q_interpolation filter 
// 0
    {
        AUDIO_OFF, "", FILTER_MASK_NONE, 0
    },

//###################################################################################################################################
// CW filters	
//###################################################################################################################################
// 1
    {
        CW_50HZ, "CW", FILTER_MASK_CW, 1, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_50hz_LPF,
//	Scale Values:		
		0.22865253254888526*0.086597131450981246*0.0050097711973540893*2.0f,
		
		//0.14992503039720381*0.14992503039720381*0.019267446597677453,  
		&IIR_dfI_AF,
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217	
    },
// 2
    {
        CW_100HZ, "CW", FILTER_MASK_CW, 2, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_100hz_LPF,
//	Scale Values:                                                         
		0.1940213957516872*                                                    
		0.079500071195902994*                                                  
		0.0063850663562343224*2.0f, 
		&IIR_dfI_AF,
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217,                                               
    },
// 3
	{
        CW_200HZ, "CW", FILTER_MASK_CW, 3, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_200hz_LPF,
//	Scale Values:                                                         
		0.22713830493699824*                                                   
		0.088356189032175972*                                                  
		0.005002255354327665*2.0f, 	
		&IIR_dfI_AF,
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217
		
    },
// 4
    {
        CW_300HZ , "CW", FILTER_MASK_CW, 4, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_300hz_LPF,
//	Scale Values:                                                         
		0.22689564282837235*                                                   
		0.091649432487851443*                                                  
		0.0050051535738409652*2.0f,  
		
		&IIR_dfI_AF,
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217
			
    },
// 5
    {
        CW_500HZ , "CW", FILTER_MASK_CW, 4, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_500hz_LPF,
//	Scale Values:                                                         
		0.56332279747029357*                                                   
		0.43787272046055464*                                                   
		0.23161940634261466*                                                   
		0.069746857209678839*                                                  
		0.002848556512949422*2.0f ,  
//		0.48087696386937467*                                                    
//		0.48087696386937467*                                                    
//		0.10162693564511283*                                                    
//		0.10162693564511283,   
		&IIR_dfI_AF,
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217
			
    },
// 5
    {
        CW_800HZ , "CW", FILTER_MASK_CW, 4, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_800hz_LPF,
//	Scale Values:                                                         
		0.5639891456415701*                                                     
		0.43903820317812314*                                                    
		0.23616555986166773*                                                    
		0.086692897666303129*                                                   
		0.0028573637691126528*2.0f,  
		
		&IIR_dfI_AF,
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217
			
    },
//###################################################################################################################################
//  SSB filters:
//###################################################################################################################################
// 
// 6
    {
        SSB_1K5, "SSB", FILTER_MASK_SSB, 1, DECIMATION_RATE_12KHZ, &FirDecimateMinLPF, &FirInterpolate_4_5k,
		&IIR_dfI_1k5_LPF,
//	Scale Values:                                                         

		0.7323796221826846*                                                     
		0.60795582198138376*                                                    
		0.37693758841566261*                                                    
		0.2084759519461549*                                                     
		0.0057477609758495697,

		&IIR_dfI_AF,
		//&AF_ScaleValues
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217
		
    },	
    {
        SSB_1K8, "SSB", FILTER_MASK_SSB, 1, DECIMATION_RATE_12KHZ, &FirDecimateMinLPF, &FirInterpolate_4_5k,
		&IIR_dfI_1k8_LPF,
//	Scale Values:                                                         

		0.73600722675034247*                                                    
		0.61460325000478799*                                                    
		0.39109749742523769*                                                    
		0.25364832887501043*                                                    
		0.005772908859553675,

		&IIR_dfI_AF,
		//&AF_ScaleValues
		0.67935598386160001*                                                      
		1.2462320642375104*                                                       
		0.016091064308973217
		
    },
// 7
	
    {
        SSB_2K1, "SSB", FILTER_MASK_SSB, 2,DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_2k1_LPF,
//	Scale Values:                                                         

		0.74091332616327554*                                                    
		0.62350251997482808 *                                                   
		0.4087156282645496*                                                     
		0.30662299266017068*                                                    
		0.005803757276544164,
 		
		&IIR_dfI_AF,
		0.76300423116151217*                                                    
		1.5601633271081594*                                                     
		0.01715808858806368

 
    },
	
// 8
	{
        SSB_2K4, "SSB", FILTER_MASK_SSB, 3, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_2k4_LPF,
//  Scale Values:
		
		0.74706063776397147*                                                    
		0.63459625552127963*                                                    
		0.42973035274583565*                                                    
		0.36737078840633774*                                                    
		0.0058400874110211077,
//		0.63577256355964773*                                                     
//		0.63577256355964773*                                                     
//		0.62954070393536188*                                                     
//		0.62954070393536188*                                                     
//		0.76056655401480011, 		
		&IIR_dfI_AF,
		0.76300423116151217*                                                    
		1.5601633271081594*                                                     
		0.01715808858806368

    },
// 9
    {
        SSB_2K7, "SSB", FILTER_MASK_SSB, 4, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_2k7_LPF,
//	  Scale Values:                                                         
		
		0.75182808020422687*                                                    
		0.64318378526485642*                                                    
		0.44560744478015385*                                                    
		0.41221537659713048*                                                    
		0.0058672305760204112*2,
			
		&IIR_dfI_AF,
		0.98719733645828944*                                                   
		2.6061122430990649*                                                    
		0.01306596683552737
    },
// 10
    {
        SSB_3K, "SSB", FILTER_MASK_SSB, 4, DECIMATION_RATE_12KHZ,&FirDecimateMinLPF,&FirInterpolate_4_5k,
		&IIR_dfI_3k_LPF,
//	  Scale Values:                                                         
		
		0.80040328485834733*                                                    
		0.81460720761359029*                                                    
		0.62896767431981948*                                                    
		0.58539694674055875*                                                    
		0.0062406429338838429 *2,
			
		&IIR_dfI_AF,
		0.98719733645828944*                                                   
		2.6061122430990649*                                                    
		0.01306596683552737
    },	
//###################################################################################################################################
//  AM filters:
//###################################################################################################################################
// 9
    {
        AM_6K, "AM", FILTER_MASK_SSB, 1, DECIMATION_RATE_12KHZ,&FirDecimate_5k,&FirInterpolate_5k,
		&IIR_dfI_6k_LPF,
//	Scale Values:                                                          
		0.80939984424905298*                                                     
		0.92678502009320951*                                                     
		2.0577857621395332 *                                                     
		0.01134957823716925,                                                     
 
		&IIR_dfI_AF,
		0.76300423116151217*                                                    
		1.5601633271081594*                                                     
		0.01715808858806368
    },
//10
//    {
//        AM_7K, "AM", FILTER_MASK_SSB, 2, DECIMATION_RATE_24KHZ,&FirDecimate_5k,&FirInterpolate_5k,
//		&IIR_dfI_7k_LPF,
////	Scale Values:                                                          
////		0.73531289928378241*                                                   
////		0.61333738900229606*                                                   
////		0.38849533841739647*                                                   
////		0.24557560319638763*                                                   
////		0.0057683162440576906*2,
//		
//		0.61753337414854281*                                                    
//		0.53833185621730606*                                                    
//		0.73651898386900061*                                                    
//		0.0064271308938245236,
//		&IIR_dfI_AF,
//		0.76300423116151217*                                                    
//		1.5601633271081594*                                                     
//		0.01715808858806368
//    },
////11
//    {
//        AM_8K, "AM", FILTER_MASK_SSB, 3, DECIMATION_RATE_24KHZ,&FirDecimate_5k,&FirInterpolate_5k,
//		&IIR_dfI_8k_LPF,
////	Scale Values: 		
////		0.73913811647279493*                                                    
////		0.62028947452989702*                                                    
////		0.40246297132447545*                                                    
////		0.2881015833020219*                                                     
////		0.0057928539197680271*2,
//		
//		0.65622050460390446*                                                     
//		0.6080515095839637*                                                      
//		0.9285821144837062*                                                      
//		0.0066880681125934035,
//		&IIR_dfI_AF,
//		0.76300423116151217*                                                    
//		1.5601633271081594*                                                     
//		0.01715808858806368
//    },
//12
    {
        AM_9K, "AM", FILTER_MASK_SSB, 4, DECIMATION_RATE_12KHZ,&FirDecimate_5k,&FirInterpolate_5k,
		&IIR_dfI_9k_LPF,
//	Scale Values: 		
//		0.74383432872337896*                                                    
//		0.62877858814198317*                                                    
//		0.41880133384480095*                                                    
//		0.33602342165111937*                                                    
//		0.005821252055365312*2,
		
		1.2204767372057976*                                                    
		1.9103121848822839*                                                    
		5.6224672420246486*                                                    
		0.0097640358223228824,
		&IIR_dfI_AF,
		0.76300423116151217*                                                    
		1.5601633271081594*                                                     
		0.01715808858806368
    },
//###################################################################################################################################
//  FM filters:
//###################################################################################################################################
//13
    {
        FM_5K, "NFM", FILTER_MASK_SSB, 1, DECIMATION_RATE_12KHZ,&FirDecimate_5k,&FirInterpolate_5k,
		&IIR_dfI_9k_LPF, 
//	Scale Values:                                                         
		0.69957527658403473*                                                     
		0.68754124445198339*                                                     
		1.149769303618468*                                                       
		0.0069721232073967632,    
		&IIR_dfI_AF,
	//Scale Values:                                                          
		1.6597303577170603*                                                     
		5.8032376261853447*                                                     
		0.017902721095396971 
    },
//14
    {
        FM_10K, "FM", FILTER_MASK_SSB, 2, DECIMATION_RATE_12KHZ,&FirDecimate_10k,&FirInterpolate_10k,
		&IIR_dfI_9k_LPF, 
//	Scale Values:                                                         
		0.69957527658403473*                                                     
		0.68754124445198339*                                                     
		1.149769303618468*                                                       
		0.0069721232073967632,  
		&IIR_dfI_AF,
	//Scale Values:                                                          
		1.6597303577170603*                                                     
		5.8032376261853447*                                                     
		0.017902721095396971 		
    },

};
//
/*
**************************************************************************************************************************************
*	读取工作模式，从而取得滤波器在各模式下用于计算滤波器路径ID的起点
*	
**************************************************************************************************************************************
*/
uint8_t GetFilterModeFromDemodMode(const uint8_t dmod_mode)
{
    uint8_t filter_mode;
    switch(dmod_mode)
    {
    case DEMOD_CW_LSB:
	case DEMOD_CW_USB:	
        filter_mode = FILTER_MODE_CW;
        break;  
    case DEMOD_LSB:
	case DEMOD_USB:	
        filter_mode = FILTER_MODE_SSB;
        break;
    case DEMOD_AM:
        filter_mode = FILTER_MODE_AM;
        break;
	case DEMOD_FM:
        filter_mode = FILTER_MODE_FM;
        break;
    // case DEMOD_LSB:
    // case DEMOD_USB:
    // case DEMOD_DIGI:
    default:
        filter_mode = FILTER_MODE_SSB;
    }
    return filter_mode;
}
/*
**************************************************************************************************************************************
*	查找适当的滤波器	
**************************************************************************************************************************************
*/
uint8_t DSP_ApplicableFilterPath(const uint8_t filter_mode, const uint8_t current_path)
{
	int idx = 0;
	idx = GetFilterModeFromDemodMode(filter_mode) + current_path;
	 return  idx;	
}
/*** END *****************************************************************************************/
