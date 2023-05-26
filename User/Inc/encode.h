#ifndef _ENCODE_H
#define _ENCODE_H
//#include "stm32f4xx.h"
#include "sys.h"

#define ENCODER_MIN     	0X000F		//最小值
#define ENCODER_DEFAULT		0x0FFF	//设定值
#define ENCODER_MAX			0x1FFF		//最大值

// Divider to create non linearity
#define ENCODER_LOG_D	1

// Audio Gain public structure
typedef struct EncoderSelection
{
    // pot values
    //
    uint32_t	value_old;			// previous value
    uint32_t	value_new;			// most current value
    uint32_t	de_detent;			// sw de-detent flag
    TIM_TypeDef* tim;

} EncoderSelection;

enum EncoderId
{
    ENCFREQ = 0,
    ENC1,
    ENC2,
    ENC_MAX // this needs to be the last entry
};
//
extern __IO EncoderSelection	encSel[4];
//
void Encode_Init(u8 dir);
int EncoderRead(const u8 encId,u8 mode);
#endif
