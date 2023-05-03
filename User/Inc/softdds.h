#ifndef __SOFTDDS_H
#define __SOFTDDS_H

//#include "arm_math.h"
#include "sys.h"
// 256 points
//#define DDS_ACC_SHIFT		8

// 512 points
//#define DDS_ACC_SHIFT		7

// 1024 points
#define DDS_ACC_SHIFT		6

typedef enum
{
	DDS_LO1=0,
	DDS_BFO,
	DDS_SI,
	DDS_TX_LO,
	DDS_END
}DdsName_Type;
// Soft DDS public structure
typedef struct //SoftDds
{
    // DDS accumulator
    uint32_t 	acc;

    // DDS step - not working if part of the structure
    uint32_t 	step;

} SoftDds;
//void Dds_Table_init(void);
void 	softdds_setfreq(u8 dds_name,float freq, uint32_t samp_rate, uint8_t  smooth);
void 	softdds_runf(u8 dds_name,float *i_buff, float *q_buff, uint16_t  size/*,float *dds_tab*/);

#endif
