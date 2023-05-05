/*
  ����� û�����úͶ�ȡ �˲���״̬������ (N��W)
*/

#include "cat_kenwood.h"
#include "KEY_Control.h"
//#include "sys.h"
//#include "rtc.h"
//#include "dwt.h"
//#include "led.h"  
//#include "mpu.h"   
//#include "lcd.h" 
//#include "timer.h" 
//#include "arm_math.h" 
//#include "key.h" 
//#include "adc.h"
#include "ui_Driver.h"
#include "dsp_filter.h"
#include "gpio_set.h"
#include "Audio_driver.h"
//#include "encode.h"
//#include "24cxx.h"
//#include "Si5351A_I2C.h"
//#include "malloc.h"
//#include "exti.h"
#include "usart.h"

/**
 * itoa() ������ֵת��Ϊ�ַ���
 * itoa() ��������ֵת��Ϊ�ַ���
 * ultoa() ���޷��ų�����ֵת��Ϊ�ַ��� 
*/
uint32_t StringToDec(uint8_t *pBuf, uint8_t len)
{
  uint32_t result = 0;
  uint8_t i;
  
  for(i=0; i<len-1; i++)
  {
    result += (pBuf[i] - 0x30); 
    result *= 10;
  }
  result += (pBuf[len-1] - 0x30);
  
  return result;
}

void DecToString(uint8_t *pBuf, uint8_t len, int32_t dat)
{
  int32_t sign;  
  uint8_t i;
  if( (sign=dat) < 0)
  {
    dat = -dat;
  }
  
  for( i=0; i<len; i++)
  {
    pBuf[len-1-i] = dat % 10 + '0'; 
    dat /= 10;
  }
   
  if(sign < 0) 
    pBuf[0] = '-'; 
}
//����ת�ַ���,����(��󳤶� 10 λ) + (����λ 1 λ [-]) + (�ַ�������'\0' [1λ])
void Myitoa(int32_t n, uint8_t str[]) 
{ 
    int32_t i, sign; 
    uint8_t s[12]; 
    uint8_t j=0;
    
    if((sign=n) < 0) 
        n=-n; 
    i=0; 
    do 
    { 
        s[i++] = n % 10 + '0'; 
    }while ((n/=10) > 0); 

    if(sign < 0) 
        s[i++] = '-'; 

    s[i] = '\0'; 

    //for (; i>0; i-- )
    do{
        str[j++] = s[i-1];
    }while(--i > 0);
}


void Kenwood_BaudSet(uint8_t baud)
{
	if ( TR_READ==CONTROL_TX )
	{
		return;
	}
	if(baud <10)
	{
		if(baud <2)vfo[VFO_A].Band_id  = 0;
		else vfo[VFO_A].Band_id  = baud;
	}
	else if(baud ==10)vfo[VFO_A].Band_id  = 1;
	if(vfo[VFO_A].Band_id <= COUPLING_40M )vfo[VFO_A].Mode = DEMOD_LSB;
	else vfo[VFO_A].Mode = DEMOD_USB;
	ks.Band_key  =0;
}

void Kenwood_BuildTransceiverStatus()
{
	uint8_t buf[40];
	uint8_t len = 0;

	buf[len ++] = 'I';
	buf[len ++] = 'F';

	// P1 ��ʾƵ��
	buf[len ++] = '0';
	buf[len ++] = '0';
	buf[len ++] = '0';
	DecToString(&buf[len], 8, (int32_t)vfo[VFO_A].Freq );
	len += 8;
	//buf[len ++] = '0';

	// P2 Spaces(5)
	buf[len ++] = ' ';
	buf[len ++] = ' ';
	buf[len ++] = ' ';
	buf[len ++] = ' ';
	buf[len ++] = ' '; 
	
  // P3 RIT/XIT frequency (5) BG2IXD
    DecToString(&buf[len], 5, (int32_t)sd.rit );
	len += 5;

	// P4 RIT 0:OFF / 1:ON
	if (ks.RIT_key  ==1)
	{
	buf[len ++] = '1';
	}
	else
	{
	buf[len ++] = '0';
	}

	// P5 XIT
	if (ks.RIT_key  ==1)
	buf[len ++] = '1';          
	else
	buf[len ++] = '0';

	// P6 P7 Memory channel number
	buf[len ++] = '1';
	buf[len ++] = (sd.num_a % 100 / 10) + 0x30;
	buf[len ++] = (sd.num_a % 10) + 0x30;

	// P8 0:TX  1:RX
	if( TR_READ ==CONTROL_RX )
	{
		buf[len ++] = '0';
	}
	else
	{
		buf[len ++] = '1';
	}

	// P9 Operating mode 1:LSB 2:USB 3:CW
	if( vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_LSB_DIG)
	{
		buf[len ++] = '1';
	}
	else
	if(vfo[VFO_A].Mode == DEMOD_USB || vfo[VFO_A].Mode == DEMOD_USB_DIG)
	{
		buf[len ++] = '2';
	}
	else
	if( vfo[VFO_A].Mode == DEMOD_CW_LSB)
	{
		buf[len ++] = '3';
	}
	else
	if( vfo[VFO_A].Mode == DEMOD_FM)
	{
		buf[len ++] = '4';
	}
	else
	if(vfo[VFO_A].Mode == DEMOD_AM)
	{
		buf[len ++] = '5';
	}
	else
	if( vfo[VFO_A].Mode < DEMOD_CW_USB)
	{
		buf[len ++] = '7';
	}
	// P10 Function (refer to the FR/FT commands) 0:VFOA 1:VFOB 2:Mem
	if(ks.ch_vfo ==0)//VFOģʽ
	{
		if( ks.A_B ==0 )//VFO_A
		{
		  buf[len ++] = '0';
		}
		else//VFO_B
		{
		  buf[len ++] = '1';
		}
	}
	else//�ŵ�ģʽ
	{
		buf[len ++] = '2';
	}

	// P11 Scan status (refer to the SC command) 0: Scan OFF
	buf[len ++] = '0';

	// P12 0: Simplex operation 1: Split operation
	buf[len ++] = '0';

	// P13 
	buf[len ++] = '0';

	// P14 
	buf[len ++] = '0';
	buf[len ++] = '0';

	// P15
	buf[len ++] = '0';

	buf[len ++] = ';';

	UartTx(buf, len);
}

void Kenwood_MemoryDataAnswer()
{
	uint8_t buf[40];
	uint8_t len = 0;

	buf[len ++] = 'M';
	buf[len ++] = '4';

	// P1 0: Simplex  1: Split
	buf[len ++] = '0';

	// P2 P3 Channel number (refer to the MC command)
	buf[len ++] = '1';
	buf[len ++] = (sd.num_a % 100 / 10) + 0x30;
	buf[len ++] = (sd.num_a % 10) + 0x30;

	// P4 frequency 
	buf[len ++] = '0';
	buf[len ++] = '0';
	buf[len ++] = '0';
	DecToString(&buf[len], 8, (int32_t)vfo[VFO_A].Freq );
	len += 8;
	//buf[len ++] = '0';

	// P5 Operating mode 1:LSB 2:USB 3:CW
	if( vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_LSB_DIG)
	{
		buf[len ++] = '1';
	}
	if(vfo[VFO_A].Mode == DEMOD_USB || vfo[VFO_A].Mode == DEMOD_USB_DIG)
	{
		buf[len ++] = '2';
	}
	if( vfo[VFO_A].Mode == DEMOD_CW_LSB)
	{
		buf[len ++] = '3';
	}
	if( vfo[VFO_A].Mode == DEMOD_FM)
	{
		buf[len ++] = '4';
	}
	if(vfo[VFO_A].Mode == DEMOD_AM)
	{
		buf[len ++] = '5';
	}
	if( vfo[VFO_A].Mode < DEMOD_CW_USB)
	{
		buf[len ++] = '7';
	}
	// P6 Data mode
	buf[len ++] = '0';

	// P7
	buf[len ++] = '0';

	// P8  Tone frequency
	buf[len ++] = '0';
	buf[len ++] = '0';

	// P9  CTCSS frequency
	buf[len ++] = '0';
	buf[len ++] = '0';

	// P10
	buf[len ++] = '0';
	buf[len ++] = '0';
	buf[len ++] = '0';

	// P11
	buf[len ++] = '0';

	// P12
	buf[len ++] = '0';

	// P13
	for(uint8_t i=0; i<9; i++)
	{
	buf[len ++] = '0';
	}

	// P14
	buf[len ++] = '0';
	buf[len ++] = '0';

	// P15
	buf[len ++] = '0';

	// P16
	for(uint8_t i=0; i<8; i++)
	{
	buf[len ++] = '0';
	}

	buf[len ++] = ';';

	UartTx(buf, len);
}

//void Kenwood_VMSwithced(FREQ_ItemDef item)
//{
//  if ( TR_READ==1)
//  {
//    return;
//  }
//  
//  if (item & VFOAItem)
//  {
//    FREQ.item &= ~MEMItem;
//    FREQ.item |= VFOItem;
//    FREQ.item &= ~VFOBItem;
//    FREQ.item |= VFOAItem;
//    oled_vfo("VFO/A ");
//    FREQ.numOfbaud = VFO.CurrBaudOfVfoA;
//    VFO_Get();
//  }
//  else if(item & VFOBItem)
//  {
//    FREQ.item &= ~MEMItem;
//    FREQ.item |= VFOItem;
//    FREQ.item &= ~VFOAItem;
//    FREQ.item |= VFOBItem;
//    oled_vfo("VFO/B ");
//    FREQ.numOfbaud = VFO.CurrBaudOfVfoB;
//    VFO_Get();
//  }
//  else
//  {
//    FREQ.item &= ~VFOItem;
//    FREQ.item |= MEMItem;
//    MEM_Get();
//  }

//  DRMgr.item |= RADIOItem;  // ����ģʽ
//  DRMgr.item |= FREQItem;   // ������Ƶ�Ͳ���
//  DRMgr.item |= FILItem;
//}
//
void Kenwood_VMAnswer()
{
  uint8_t buf[15];
  uint8_t len = 0;
  
  buf[ len++ ] = 'F';
  buf[ len++ ] = 'R';
  if (ks.ch_vfo ==0&& ks.A_B ==0)
  {
    buf[ len++ ] = '0';
  }
  else  if (ks.ch_vfo ==0&& ks.A_B ==1)
  { 
    buf[ len++ ] = '1';
  }
  else  if (ks.ch_vfo ==1)
  {
    buf[ len++ ] = '2';
  }
  buf[ len++ ] = ';';
  UartTx(buf, len);
}

u8 Kenwood_ModeSwithced(u8 mode)
{
//	static u8 dig_l,dig_u;

	if (TR_READ==CONTROL_TX || mode==0|| mode >6)
	{
		return 0;
	}
	if( mode == 1 )
	{
/*		dig_l =(~dig_l)&0x1;
		if(dig_l ==0)vfo[VFO_A].Mode = DEMOD_LSB;
		else */
		vfo[VFO_A].Mode = DEMOD_LSB_DIG;
	}
	if( mode == 2 )
	{
/*		dig_u = (~dig_u)&0x1;
		if(dig_u ==0)vfo[VFO_A].Mode = DEMOD_USB;
		else */
		vfo[VFO_A].Mode = DEMOD_USB_DIG;
	}
	if( mode == 3 )
	{
		vfo[VFO_A].Mode = DEMOD_CW_LSB;
	}
	if( mode == 4 )
	{
		vfo[VFO_A].Mode = DEMOD_FM;
	}
	if( mode == 5 )
	{
		vfo[VFO_A].Mode = DEMOD_AM;
	}
	if( mode == 7 )
	{
		vfo[VFO_A].Mode = DEMOD_CW_USB;
	}

	return 1;
}

//RIT frequency down     
void Kenwood_RITSetDown(int32_t val)
{
	if ( TR_READ==CONTROL_TX)
	{
		return;
	}
  
	if(ks.RIT_key  ==0)
	{
		sd.rit -=val;
	}
}

//RIT frequency up     
void Kenwood_RITSetUp(int32_t val)
{
  if ( TR_READ==CONTROL_TX)
	{
		return;
	}
  
	if(ks.RIT_key  ==0)
	{
		sd.rit +=val;
	}
}

//XIT frequency down      
void Kenwood_XITSetDown(int32_t val)
{
  if ( TR_READ==CONTROL_TX)
	{
		return;
	}
  
	if(ks.RIT_key  ==0)
	{
		sd.rit -=val;
	}
}

//XIT frequency up     
void Kenwood_XITSetUp(int32_t val)
{
  if ( TR_READ==CONTROL_TX)
	{
		return;
	}
  
	if(ks.RIT_key  ==0)
	{
		sd.rit +=val;
	}
}
//
void Kenwodd_MemSet(uint8_t mem)
{
	//if ((AIMgr.item & MENUSETItem) || (FREQ.item & MEMItem))
	//{
		sd.num_a = mem; 
  
	//}
}

void Kenwood_VFOAAnswer()
{
	uint8_t buf[15];
	uint8_t len = 0;

	buf[ len++ ] = 'F';
	buf[ len++ ] = 'A';
	buf[ len++ ] = '0';
	buf[ len++ ] = '0';
	buf[ len++ ] = '0';
	DecToString(&buf[len], 8, (int32_t)vfo[VFO_A].Freq );
	len += 8;    
	//buf[ len++ ] = '0';
	buf[ len++ ] = ';';
	UartTx(buf, len);
}

void Kenwood_VFOBAnswer()
{
	uint8_t buf[15];
	uint8_t len = 0;

	buf[ len++ ] = 'F';
	buf[ len++ ] = 'B';
	buf[ len++ ] = '0';
	buf[ len++ ] = '0';
	buf[ len++ ] = '0';
	DecToString(&buf[len], 8, (int32_t)vfo[VFO_B].Freq);
	len += 8;    
	//buf[ len++ ] = '0';
	buf[ len++ ] = ';';
	UartTx(buf, len);
}

u8 Kenwood_VFOSet(u8 vfo_name,int32_t freq)
{
	uint8_t m,i;
  
//	if ( TR_READ==1)
//	{
//		return 0;
//	}
	//if(vfo[VFO_A].Freq != freq)return 1;
	for(i=0; i<11; i++)
	{
		if(i <10)
		{
			if(i <2)m =0;
			else m = i;
		}
		else if(i ==10)m=1;
		if(freq >= Band_Info[m].start && freq <= Band_Info[m].end)	//�±߽�
		{
			vfo[vfo_name].Band_id =m;
			vfo[vfo_name].Freq = freq;
			
			return 1;
		}
	}
	return 0;
}

void Kenwood_AFGainSet(uint8_t vol)
{
	if ( TR_READ==CONTROL_TX)
	{
		return;
	}
	// �������0-100����������0-255
	// ����0-60
	uint16_t tmp = vol * 100 / 255;
	if(tmp >= 60)
	{
		tmp = 60;
	}
	sd.Vol = tmp;
}

void Kenwood_AFGainAnswer()
{
	uint8_t buf[15];
	uint8_t len = 0;

	buf[ len++ ] = 'A';
	buf[ len++ ] = 'G';
	buf[ len++ ] = '0';
	uint16_t tmp = sd.Vol * 255 / 100;
	DecToString(&buf[len], 3, (int32_t)tmp);
	len += 3;    
	buf[ len++ ] = ';';
	UartTx(buf, len);
}

void Kenwood_MemoryChannelAnswer()
{
	uint8_t buf[15];
	uint8_t len = 0;

	buf[ len++ ] = 'M';
	buf[ len++ ] = 'C';
	buf[ len++ ] = '1';
	DecToString(&buf[len], 2, (int32_t)sd.num_a );
	len += 2;
	buf[ len++ ] = ';';
	UartTx(buf, len);
}

void Kenwood_ModeStauteAnswer()
{
	uint8_t buf[15];
	uint8_t len = 0;
	uint8_t mode;

	buf[ len++ ] = 'M';
	buf[ len++ ] = 'D';

	if(vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_LSB_DIG)mode =1;
	if(vfo[VFO_A].Mode == DEMOD_USB || vfo[VFO_A].Mode == DEMOD_USB_DIG)mode =2;
	if(vfo[VFO_A].Mode == DEMOD_CW_LSB)mode =3;
	if(vfo[VFO_A].Mode == DEMOD_FM)mode =4;
	if(vfo[VFO_A].Mode == DEMOD_AM)mode =5;
	if(vfo[VFO_A].Mode == DEMOD_CW_USB)mode =7;
	
	DecToString(&buf[len], 1, (int32_t)mode );
	len += 1;
	buf[ len++ ] = ';';
	UartTx(buf, len);
}

void Kenwodd_SMeterAnswer()
{
	uint8_t buf[15];
	uint8_t len = 0;

	buf[ len++ ] = 'S';
	buf[ len++ ] = 'M';
	buf[ len++ ] = '0';
  
    DecToString(&buf[len], 4, (int32_t)(ads.Riss*0.28f ));

	len += 4;
	buf[ len++ ] = ';';
	UartTx(buf, len);
}

void Kenwood_SWRMeterAnswer()
{
	uint8_t buf[15];
	uint8_t len = 0;

	buf[ len++ ] = 'R';
	buf[ len++ ] = 'M';
	buf[ len++ ] = '1'; // 1: SWR
	DecToString(&buf[len], 4, (int32_t)ads.swr);
	len += 4;
	buf[ len++ ] = ';';
	UartTx(buf, len);
}

void Kenwodd_KeyingSpeedAnswer()
{
  uint8_t buf[15];
  uint8_t len = 0;
  
  buf[ len++ ] = 'K';
  buf[ len++ ] = 'S';
  DecToString(&buf[len], 3, (int32_t)sd.CW_Speed);
  len += 3;
  buf[ len++ ] = ';';
  UartTx(buf, len);
}
//len : ָ�����, ĩβ�ķֺŲ�������len��    BG6RDF
void Kenwood_CmdParser(uint8_t *pBuf, uint8_t len)
{ 
	//static u32 freqA,freqB;
	u32 tmp = 0;
	uint8_t buf[15];
  
  // ���úͶ�ȡ��Դ����״̬
	if(pBuf[0] == 'P' && pBuf[1] == 'S')
	{
		if( len > 2)
		{
			// 0: Power OFF ; 1: Power ON ; 9: Power OFF (low current mode)
			UartTx("PS1;", 4);        //
		}
		else
		{
			UartTx("PS1;", 4);
		}
	}
  // 
	if(pBuf[0] == 'A' && pBuf[1] == 'I')
	{
		if( len > 2)
		{
    
		}
		else
		{
			UartTx("AI0;", 4); // AI OFF
		}
	}
	// ��ȡ��������״̬
	if(pBuf[0] == 'I' && pBuf[1] == 'F')
	{
		Kenwood_BuildTransceiverStatus();
	}
	// ����Ƶ�ʲ���
	if((pBuf[0] == 'B' && pBuf[1] == 'D') || (pBuf[0] == 'B' && pBuf[1] == 'U'))
	{
		//Sets a frequency band.
		if (len >= 2)
		{
			ks.Band_key =0;
			tmp = StringToDec(&pBuf[2], 2);
			Kenwood_BaudSet(tmp);
		}
	}
	//
	if(pBuf[0] == 'B' && pBuf[1] == 'Y')
	{
	// Reads the busy signal status.
	// δʹ��
		if( len > 2)
		{

		}
		else
		{
			UartTx("BY10;", 5);       //BG6RDF
		}
	}
	//
	if(pBuf[0] == 'C' && pBuf[1] == 'H')
	{
   // 0: Move the MULTI/CH encoder 1 step up ;1: Move the MULTI/CH encoder 1 step down
		if (len > 1)
		{
			if(pBuf[2] == '1')
			{
     
			}
			else
			{
      
			}
		}
	}
  // ���û��ȡVFOA/VFOB��Ƶ��
	if(pBuf[0] == 'F' && pBuf[1] == 'A')
	{
    // Sets or reads the VFO A/ VFO B frequency
		if( len > 2)
		{
			tmp = StringToDec(&pBuf[2], 11);// Frequency (11 digits in Hz); eg.00014195000 for 14.195 MHz
			
			//if(Kenwood_VFOSet(VFO_A,tmp ))Kenwood_VFOAAnswer();
			//ks.A_B =0;
			//ks.vfo_read =0; 
			Kenwood_VFOSet(VFO_A,tmp );
		}
		else
		{
			//ks.A_B =0;
			Kenwood_VFOAAnswer();
			//ks.vfo_read =0;
		}
	}
	
	//
	if(pBuf[0] == 'F' && pBuf[1] == 'B')
	{
    // Sets or reads the VFO A/ VFO B frequency
		if( len > 2)
		{
			tmp = StringToDec(&pBuf[2], 11);// Frequency (11 digits in Hz); eg.00014195000 for 14.195 MHz
			
			//if(Kenwood_VFOSet(VFO_B,tmp ))
			Kenwood_VFOSet(VFO_B,tmp );
			//ks.A_B =1;
			//ks.vfo_read =1;
		}
		else
		{
			Kenwood_VFOBAnswer();
			//ks.A_B =1;
			//ks.vfo_read =1;
		}
	}
//		if(freqA != vfo[VFO_A].Freq )
//		{
//			Kenwood_VFOAAnswer();
//			freqA = vfo[VFO_A].Freq;
//		}
	
  // ѡ����ȡVFO �� �Ĵ���ͨ��
	if(pBuf[0] == 'F' && pBuf[1] == 'R')
	{
		// Selects or reads the VFO or Memory channel
		if (len > 2)
		{
			if (pBuf[2] == '0') // VFOA
			{
				//Kenwood_VMSwithced(VFOAItem);
				ks.A_B =0;
				ks.ch_vfo =0;
				ks.vfo_read =0;
				//Kenwood_VFOAAnswer();
			}
			else if (pBuf[2] == '1')  // VFOB
			{
				//Kenwood_VMSwithced(VFOBItem);
				ks.A_B =1;
				ks.ch_vfo =0;
				ks.vfo_read =0;
				//Kenwood_VFOBAnswer();
			}
			else  // MEM
			{
				//Kenwood_VMSwithced(MEMItem);
				ks.ch_vfo =1;
				ks.vfo_read =0;
			}
	  
		}
		else
		{
			Kenwood_VMAnswer();
		}
	}
	//
	if(pBuf[0] == 'F' && pBuf[1] == 'T')
	{
		// Selects or reads the VFO or Memory channel
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("FT0;", 4); // VFO A
		}
	 }
	//
	if(pBuf[0] == 'F' && pBuf[1] == 'L')
	{
    // δʹ��
		if( len > 2)
		{
    
		}
		else
		{
			UartTx("FL1;", 4); // 1: IF Filter A
		}
	}
	//
	if(pBuf[0] == 'F' && pBuf[1] == 'V')
	{
		// δʹ��
		if( len > 2)
		{
    
		}
		else
		{
			UartTx("FV1.00;", 7); // 1: Firmware Version
		}
	}
	//
	if(pBuf[0] == 'D' && pBuf[1] == 'A')
	{
		// δʹ��
		if( len > 2)
		{
    
		}
		else
		{
			UartTx("DA0;", 4); // 0: DATA mode OFF
		}
	}
	//
	if(pBuf[0] == 'F' && pBuf[1] == 'S')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("FS0;", 4); // 0: Fine Tuning Funciton Off
		}
	}
	//
	if(pBuf[0] == 'N' && pBuf[1] == 'B')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("NB0;", 4); // 0: NB OFF
		}
	}
	//
	if(pBuf[0] == 'N' && pBuf[1] == 'R')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("NR0;", 4); // 0: NR OFF
		}
	}
	//
	if(pBuf[0] == 'P' && pBuf[1] == 'A')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("PA00;", 5); // 0: Pre-amp Off
		}
	}
	//
	if(pBuf[0] == 'P' && pBuf[1] == 'R')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("PR0;", 4); // 0: Speech OFF
		}
	}
	//
	if(pBuf[0] == 'R' && pBuf[1] == 'A')
	{
		// Sets and reads the RF Attenuator status. δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("RA0000", 7);  // 00: ATT OFF
		}
	}
	//
	if(pBuf[0] == 'V' && pBuf[1] == 'X')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("VX0;", 4); // 0: VOX OFF
		}
	}
	//
	if(pBuf[0] == 'B' && pBuf[1] == 'C')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("BC0;", 4); // 0: Beat Cancel OFF
		}
	}
	//
	if(pBuf[0] == 'C' && pBuf[1] == 'A')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("CA0;", 4); // 0: Cancels CW TUNE/ Inactive
		}
	}
	//
	if(pBuf[0] == 'I' && pBuf[1] == 'D')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
		  UartTx("ID020;", 6); // TS-590S
		}
	}
	//
	if(pBuf[0] == 'T' && pBuf[1] == 'O')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("TO0;", 4); // 0: Tone OFF
		}
	}
	//
	if(pBuf[0] == 'T' && pBuf[1] == 'N')
	{
		// Sets and reads the Tone frequency. δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("TN00;", 5); // 
		}
	}
	//
	if(pBuf[0] == 'C' && pBuf[1] == 'T')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
		  UartTx("CT0;", 4); // 0: CTCSS OFF
		}
	}
	//
	if(pBuf[0] == 'C' && pBuf[1] == 'N')
	{
		// Sets and reads the CTCSS frequency δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("CN00;", 5); // 0: CTCSS OFF
		}
	}
	//
	if(pBuf[0] == 'T' && pBuf[1] == 'S') 
	{
		// Sets and reads the TF-Set status
		if( len > 2 )
		{
//      	if( pBuf[2] == '1') // 0: TF-Set OFF ; 1: TF-Set ON
//      	{
//       		 KEY_SoftwareSet(5, keyLong);
//      	}
//      	else
//      	{
//       		 KEY_SoftwareSet(5, keyShot);
//      	} 
		}
		else
		{
			buf[0] = 'T';
			buf[1] = 'S';
//      	if( FREQ.item & TSItem)
//      	{
//        		buf[2] = '1';
//      	}
//      	else
//      	{
//        		buf[2] = '0';
//      	}
			buf[2] = '0';
			buf[3] = ';';
			UartTx(buf, 4);
		}
	}
	//
	if(pBuf[0] == 'M' && pBuf[1] == 'F')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("MF0;", 4); // 0: Menu A
		}
	}
	//
	if(pBuf[0] == 'A' && pBuf[1] == 'C')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("AC000;", 6); // 0:
		}
	}
	//
	if(pBuf[0] == 'N' && pBuf[1] == 'T')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("NT00;", 5); // 0: Notch OFF  0: Normal
		}
	}
	// ��������
	if(pBuf[0] == 'A' && pBuf[1] == 'G')
	{
		// Sets or reads the AF gain
		if( len > 3)
		{
			tmp = StringToDec(&pBuf[3], 3);
			Kenwood_AFGainSet(tmp);
		}
		else
		{
			Kenwood_AFGainAnswer(); // 
		}
	}
	//
	if(pBuf[0] == 'R' && pBuf[1] == 'G')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
		  UartTx("RG000;", 6); // 
		}
	}
	//
	if(pBuf[0] == 'G' && pBuf[1] == 'C')
	{
		// Sets or reads the AGC δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("GC0;", 4); // 
		}
	}
	//
	if(pBuf[0] == 'P' && pBuf[1] == 'C')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("PC005;", 6); // 005 ~ 100: SSB/ CW/ FM/ FSK 
		}
	}
  //
	if(pBuf[0] == 'M' && pBuf[1] == 'G')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
		  UartTx("MG000;", 6); // The microphone gain
		}
	}
	// ��ȡ�Ĵ���ͨ��������
	if(pBuf[0] == 'M' && pBuf[1] == 'R')
	{
		// Reads the Memory channel data.
		if( pBuf[2] = '0' && pBuf[3] == '1')
		{
		  Kenwood_MemoryDataAnswer();
		}
	}
	//
	if(pBuf[0] == 'N' && pBuf[1] == 'L')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("NL001;", 6); // 001 ~ 010
		}
	}
	//
	if(pBuf[0] == 'V' && pBuf[1] == 'G')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("VG000;", 6); // the VOX Gain 
		}
	}
	if(pBuf[0] == 'V' && pBuf[1] == 'D')
	{
		// δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("VD0000;", 7); // the VOX Gain 
		}
	}
   // ��ȡRX��TX�ĵ�ƽָʾ����
	if(pBuf[0] == 'S' && pBuf[1] == 'M')
	{
		// Reads the S-meter value. 
		// The SM command reads the S-meter during reception and the RF (power) meter during transmission.
		if( pBuf[2] == '0' )
		{
			Kenwodd_SMeterAnswer(); // 0000 ~ 0030: S-meter value
		}
	}
	//
	if(pBuf[0] == 'S' && pBuf[1] == 'Q')
	{
		// δʹ��
		if( len > 3)
		{
		
		}
		else
		{
		  UartTx("SQ0000;", 7); // the squelch value
		}
	}
	//
	if(pBuf[0] == 'Q' && pBuf[1] == 'R')
	{
		// δʹ��
		if( len > 3)
		{
		
		}
		else
		{
			UartTx("QR00;", 5); // Quick Memory channel data
		}
	}
	//
	if(pBuf[0] == 'I' && pBuf[1] == 'S')
	{
		// Sets and reads the DSP Filter Shift δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("IS 0000;", 8); // 
		}
	}
	// ���úͶ�ȡ�������
	if(pBuf[0] == 'K' && pBuf[1] == 'S')
	{
		// Sets and reads the Keying speed
		if( len > 2 )
		{
		  tmp = StringToDec(&pBuf[2], 3); // 004 ~ 060 (in steps of 1)
		  sd.CW_Speed = tmp;
		}
		else
		{
		  Kenwodd_KeyingSpeedAnswer();
		}
	}
	//
	if(pBuf[0] == 'S' && pBuf[1] == 'D')
	{
		// Sets and reads the CW break-in time delay. δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("SD0000;", 7); // 
		}
	}
	//
	if(pBuf[0] == 'A' && pBuf[1] == 'N')
	{
		// Selects the antenna connector ANT1/ ANT2 δʹ��
		if( len > 2)
		{
		
		}
		else
		{
			UartTx("AN000;", 6); // 
		}
	}
	//
	if(pBuf[0] == 'K' && pBuf[1] == 'Y')
	{
		// Converts the entered characters into morse code while keying.
		if( len == 2 )
		{
			 buf[0] = 'K';
			 buf[1] = 'Y';
			 buf[1] = '0'; //0: Character buffer space
			 buf[5] = ';';
			 UartTx(buf, 4);
		}
		else if( len > 3 )
		{
		  
		}
	}
	// ���úͶ�ȡ�Ĵ���ͨ����
	if(pBuf[0] == 'M' && pBuf[1] == 'C')
	{
		// Sets and reads the Memory Channel number
		if( len > 2 )
		{
			if( pBuf[1] == '1' )
			{
				tmp = StringToDec(&pBuf[2], 2); //00 ~ 99: Two digit channel number
				Kenwodd_MemSet(tmp);
			}
		}
		else
		{
			Kenwood_MemoryChannelAnswer();
		}
	}
	// ���úͶ�ȡ����ģʽ
	if(pBuf[0] == 'M' && pBuf[1] == 'D')
	{
		// Sets and reads the operating mode status.
		if( len > 2 )
		{
			if (Kenwood_ModeSwithced(pBuf[2] - '0' ))
			{
				Kenwood_ModeStauteAnswer();
			}
		}
		else
		{
			Kenwood_ModeStauteAnswer();
		}
	}
	//
	if(pBuf[0] == 'R' && pBuf[1] == 'C')
	{
    // Clears the RIT/XIT frequency. -ע������RIT/XITģʽʱ��������
//    if (FREQ.item & RITItem)    //BG6RDF
//      VFO.RIT=0;
//    if (FREQ.item & XITItem)
//      VFO.XIT=0;
//    DRMgr.item |= FREQItem;
	}
	// ��ȡ
	if(pBuf[0] == 'R' && pBuf[1] == 'M')
	{
		// Sets and reads the Meter function.
		if( len > 2 )
		{
		}
		else
		{
		  Kenwood_SWRMeterAnswer(); // 0000 ~ 0030: Meter value in dots
		}
	}
	//
	if(pBuf[0] == 'R' && pBuf[1] == 'D')
	{
		// Sets and reads the RIT/XIT frequency Down
		if( len > 2 )
		{
			tmp = StringToDec(&pBuf[2], 5); // 00000 ~ 99999: Frequency (in Hz)
			if(ks.RIT_key  ==0)
			{
				Kenwood_RITSetDown(tmp);      //BG6RDF
		  
				Kenwood_XITSetDown(tmp);
			}
		}
		else
		{
			buf[0] = 'R';
			buf[1] = 'D';
			buf[2] = '1';
			buf[3] = ';';
			UartTx(buf, 4);           //
		}
	}
	//
	if(pBuf[0] == 'R' && pBuf[1] == 'U')
	{
		// Sets and reads the RIT/XIT frequency Up
		if( len > 2 )
		{
			tmp = StringToDec(&pBuf[2], 5); // 00000 ~ 99999: Frequency (in Hz)
			if(ks.RIT_key  ==0)
			{
				Kenwood_RITSetUp(tmp);      //
				Kenwood_XITSetUp(tmp);
			}
		}
		else
		{
		  buf[0] = 'R';
		  buf[1] = 'U';
		  buf[2] = '1';
		  buf[3] = ';';
		  UartTx(buf, 4);           //
		}
	}
	//
	if(pBuf[0] == 'R' && pBuf[1] == 'T') 
	{
		// Sets and reads the RIT function status.
		if( len > 2 )
		{
			if( pBuf[2] == '1') // 0: RIT OFF ; 1: RIT ON
			{
				ks.RIT_key =0;
			}
			else
			{
				ks.RIT_key =1;
			}
		}
		else
		{
			buf[0] = 'R';
			 buf[1] = 'T';
			 if( ks.RIT_key ==0)
			 {
				buf[2] = '1';
			 }
			 else
			 {
				buf[2] = '0';
			 }
			 buf[3] = ';';
			 UartTx(buf, 4);
		}
	}
	//RX
	if(pBuf[0] == 'R' && pBuf[1] == 'X') 
	{
		// Sets the receiver function status.
		//if( vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_USB )
		//{
			ks.cat_ptt =1;
			//TR_CONTROL(CONTROL_RX);
		//}
	}
	//TX
	if(pBuf[0] == 'T' && pBuf[1] == 'X') 
	{
		// Sets the transmission mode.
		if (pBuf[2]=='0'||pBuf[2]=='1'||pBuf[2]=='2')
		{
			//if( vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_USB )
			//{
				ks.cat_ptt =0;
				
			//TR_CONTROL(CONTROL_TX);
			//}
		}
	}
	//
	if(pBuf[0] == 'X' && pBuf[1] == 'T')     //BG6RDF
	{
		// Sets and reads the XIT function status.
		if( len > 2 )
		{
			if( pBuf[2] == '1') // 0: XIT OFF ; 1: XIT ON
			{
				if( vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_USB )
				{    
					ks.RIT_key =0;//����XIT
				}
			}
			else
			{
				if( vfo[VFO_A].Mode == DEMOD_LSB || vfo[VFO_A].Mode == DEMOD_USB )
				{    
					ks.RIT_key =1;//�˳�XIT
				}
			}
		}
		else
		{
			 buf[0] = 'X';
			 buf[1] = 'T';
			 if( ks.RIT_key ==0)
			 {
				buf[2] = '1';
			 }
			 else
			 {
				buf[2] = '0';
			 }
			 buf[3] = ';';
			 UartTx(buf, 4);
		}
	}
  
}
