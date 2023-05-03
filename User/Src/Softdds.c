
#include "dds_table.h"
#include "softdds.h"
//#include "arm_math.h"
// Software DDS public
__IO 	SoftDds	softdds[DDS_END];

// Step in the public structure - don't work for some crazy reason
//u32  	step;
//int16_t Dds_tab[1024];
//void Dds_Table_init(void)
//{
//	u16 i;
//	for(i=0;i<1024;i++)
//	{
//		 Dds_tab[i] = arm_sin_f32(2*PI*i/1024);
//	}
//}
//*----------------------------------------------------------------------------
//* Function Name       : softdds_setfreq
//* Object              :
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void softdds_setfreq(u8 dds_name,float freq,u32 samp_rate,u8 smooth)
{
	
    float f = (float)(samp_rate);

    //printf("freq %d\n\r",(int)freq);

    // Reset accumulator, if need smooth tone
    // transition, do not reset it (e.g. wspr)
    if(!smooth)
        softdds[dds_name].acc = 0;

    // Calculate new step
    f /=65536.0f;
    softdds[dds_name].step = (u32)(freq / f);

   // Dds_Table_init();
}

//*----------------------------------------------------------------------------
//* Function Name       : softdds_runf
//* Object              : use two float buffer
//* Object              :
//* Input Parameters    :
//* Output Parameters   :
//* Functions called    :
//*----------------------------------------------------------------------------
void softdds_runf(u8 dds_name,float *i_buff,float *q_buff,u16  size/*,float *dds_tab*/)
{
    u16  k;
	u16 i;
	
    for(i = 0; i < size; i++)
    {
        // Calculate next sample计算下一个样本 
        softdds[dds_name].acc += softdds[dds_name].step;
        k = softdds[dds_name].acc >> DDS_ACC_SHIFT;

        // Fix ptr overload
        k &= (DDS_TBL_SIZE - 1);

        // Load I value
        *i_buff = (float)( DDS_TABLE[k]);
		*i_buff *=0.000030517578125f;
        // 90 degrees shift
        k += (DDS_TBL_SIZE>>2);
		
        // Fix ptr overload
        k &= (DDS_TBL_SIZE - 1);

        // Load Q value
        *q_buff = (float)( DDS_TABLE[k]);
		*q_buff *=0.000030517578125f;
        // Next ptr
        i_buff++;
        q_buff++;
    }
}
