#include "lcd.h"
#include "stdlib.h"
#include "font.h" 
//#include "usart.h"	  
#include "dwt.h"  
//#include "ltdc.h"
/***************************************************************************************
MCU ：STM32F407VGT6
晶振 ：8MHZ
主频 ：168MHZ
接线说明:
//-------------------------------------------------------------------------------------
#define LCD_CTRL   	  	GPIOB		//定义TFT数据端口
#define LCD_LED        	GPIO_Pin_9  //PB9 连接至TFT -LED
#define LCD_RS         	GPIO_Pin_10	//PB10连接至TFT --RS
#define LCD_CS        	GPIO_Pin_11 //PB11 连接至TFT --CS
#define LCD_RST     	GPIO_Pin_12	//PB12连接至TFT --RST
#define LCD_SCL        	GPIO_Pin_13	//PB13连接至TFT -- CLK
#define LCD_SDA        	GPIO_Pin_15	//PB15连接至TFT - SDI
#define
//VCC:可以接5V也可以接3.3V
//LED:可以接5V也可以接3.3V或者使用任意空闲IO控制(高电平使能)
//GND：接电源地
//说明：如需要尽可能少占用IO，可以将LCD_CS接地，LCD_LED接3.3V，LCD_RST接至单片机复位端，
//将可以释放3个可用IO
//接口定义在Lcd_Driver.h内定义，
//如需变更IO接法，请根据您的实际接线修改相应IO初始化LCD_GPIO_Init()
#define
//-----------------------------------------------------------------------------------------

**********************************************************************************************/
//管理LCD重要参数
//默认为竖屏
//_lcd_dev lcddev;

/****************************************************************************
* 名    称：void LCD_GPIO_Init(void)
* 功    能：STM32_模拟SPI所用到的GPIO初始化
* 入口参数：无
* 出口参数：无
* 说    明：初始化模拟SPI所用的GPIO
****************************************************************************/
//void LCD_GPIO_Init(void)
//{

//	GPIO_InitTypeDef  GPIO_InitStructure;
//	      
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB ,ENABLE);
//	
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10| GPIO_Pin_11| GPIO_Pin_12 | GPIO_Pin_13 |  GPIO_Pin_15;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
//      
//}
/****************************************************************************
* 名    称：void  SPIv_WriteData(u8 Data)
* 功    能：STM32_模拟SPI写一个字节数据底层函数
* 入口参数：Data
* 出口参数：无
* 说    明：STM32_模拟SPI读写一个字节数据底层函数
****************************************************************************/
void  SPIv_WriteData(u16 Data)
{
	unsigned char i=0;
	for(i=8;i>0;i--)
	{
		if(Data&0x80)	
	  LCD_SDA_SET; //输出数据
      else LCD_SDA_CLR;
	   
      LCD_SCL_CLR;       
      LCD_SCL_SET;
      Data<<=1; 
	}
}

/****************************************************************************
* 名    称：u8 SPI_WriteByte(SPI_TypeDef* SPIx,u8 Byte)
* 功    能：STM32_硬件SPI读写一个字节数据底层函数
* 入口参数：SPIx,Byte
* 出口参数：返回总线收到的数据
* 说    明：STM32_硬件SPI读写一个字节数据底层函数
****************************************************************************/
void SPI_WriteByte(u16 TxData)
{
	//u8 RxData=0;	
	SPI2->CR1|=1<<0;				//SPE=1,使能SPI2
	SPI2->CR1|=1<<9;  				//CSTART=1,启动传输
	
	while((SPI2->SR&1<<1)==0);		//等待发送区空 
	*(vu8 *)&SPI2->TXDR=TxData;		//发送一个byte,以传输长度访问TXDR寄存器   
	//while((SPI2->SR&1<<0)==0);		//等待接收完一个byte  
	//RxData=*(vu8 *)&SPI2->RXDR;		//接收一个byte,以传输长度访问RXDR寄存器	
	
	SPI2->IFCR|=3<<3;				//EOTC和TXTFC置1,清除EOT和TXTFC位 
	SPI2->CR1&=~(1<<0);				//SPE=0,关闭SPI2,会执行状态机复位/FIFO重置等操作
	//return RxData;					//返回收到的数据	
} 
/****************************************************************************
* 名    称：SPI2_Init(void)
* 功    能：STM32_SPI2硬件配置初始化
* 入口参数：无
* 出口参数：无
* 说    明：STM32_SPI2硬件配置初始化
****************************************************************************/
void SPI2_Init(void)	
{
	
	u32 tempreg=0;
	RCC->AHB4ENR|=1<<1;    	//使能PORTB时钟	
	RCC->AHB4ENR|=1<<2;    	//使能PORTB时钟	
	RCC->APB1LENR|=1<<14;		//SPI2时钟使能 
	GPIO_Set(GPIOB,PIN10|PIN11|PIN12, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PU);
	GPIO_Set(GPIOB,PIN13|PIN15,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	//GPIO_Set(GPIOB,PIN13|PIN15,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_HIGH,GPIO_PUPD_PU);
	//GPIO_Set(GPIOC,PIN5, GPIO_MODE_OUT, GPIO_OTYPE_PP, GPIO_SPEED_HIGH, GPIO_PUPD_PD);//背光
	//GPIO_AF_Set(GPIOB,12,5);	//PB12,AF5
 	GPIO_AF_Set(GPIOB,13,5);	//PB13,AF5
 	GPIO_AF_Set(GPIOB,15,5);	//PB15,AF5 
	//LCD_BL(1);
	//配置SPI的时钟源
	RCC->D2CCIP1R&=~(7<<12);	//SPI123SEL[2:0]=0,清除原来的设置
	RCC->D2CCIP1R|=0<<12;		//SPI123SEL[2:0]=1,选择pll1_q_ck作为SPI1/2/3的时钟源,一般为200Mhz
								//即:spi_ker_ck=200Mhz 
	//这里只针对SPI口初始化
	RCC->APB1LRSTR|=1<<14;		//复位SPI2
	RCC->APB1LRSTR&=~(1<<14);	//停止复位SPI2
	
	SPI2->CR1|=1<<12;			//SSI=1,设置内部SS信号为高电平	
	SPI2->CFG1|=0<<28;			//MBR[2:0]=0,设置spi_ker_ck为2分频.
	SPI2->CFG1|=7<<0;			//DSIZE[4:0]=7,设置SPI帧格式为8位,即字节传输
	tempreg=(u32)1<<31;			//AFCNTR=1,SPI保持对IO口的控制
	tempreg|=0<<29;				//SSOE=0,禁止硬件NSS输出
	tempreg|=1<<26;				//SSM=1,软件管理NSS脚
	tempreg|=1<<25;				//CPOL=1,空闲状态下,SCK为高电平
	tempreg|=0<<24;				//CPHA=0,数据采样从第2个时间边沿开始
	tempreg|=0<<23;				//LSBFRST=0,MSB先传输
	tempreg|=1<<22;				//MASTER=1,主机模式
	tempreg|=0<<19;				//SP[2:0]=0,摩托罗拉格式
	tempreg|=1<<17;				//COMM[1:0]=0,单工通信
	//tempreg|=2<<4;
	SPI2->CFG2=tempreg;			//设置CFG2寄存器	
	SPI2->I2SCFGR&=~(1<<0);		//选择SPI模式
	SPI2->CR1|=1<<0;			//SPE=1,使能SPI2
	//SPI2->CFG1|=2<<28;
	//SPI_WriteByte(0xff);	//启动传输		 
}

/****************************************************************************
* 名    称：Lcd_WriteIndex(u8 Index)
* 功    能：向液晶屏写一个8位指令
* 入口参数：Index   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
****************************************************************************/
void Lcd_WriteIndex(u8 Index)
{
   LCD_CS_CLR;
   LCD_RS_CLR;
//#if USE_HARDWARE_SPI   
   SPI_WriteByte(Index);
//#else
   //SPIv_WriteData(Index);
//#endif 
   LCD_CS_SET;
}

/****************************************************************************
* 名    称：Lcd_WriteData(u8 Data)
* 功    能：向液晶屏写一个8位数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，内部函数
****************************************************************************/
//void Lcd_WriteData(u16 Data)
void Lcd_WriteData(u8 Data)	
{
   LCD_CS_CLR;
   LCD_RS_SET;
//#if USE_HARDWARE_SPI  	
   SPI_WriteByte(Data);	
//#else
   //SPIv_WriteData(Data);
//#endif 
   LCD_CS_SET;
}

/****************************************************************************
* 名    称：void LCD_WriteReg(u8 Index,u16 Data)
* 功    能：写寄存器数据
* 入口参数：Index,Data
* 出口参数：无
* 说    明：本函数为组合函数，向Index地址的寄存器写入Data值
****************************************************************************/
void LCD_WriteReg(u8 Index,u8 Data)
{
	Lcd_WriteIndex(Index);
  	Lcd_WriteData(Data);
}

/****************************************************************************
* 名    称：void Lcd_WriteData_16Bit(u16 Data)
* 功    能：向液晶屏写一个16位数据
* 入口参数：Data
* 出口参数：无
* 说    明：向控制器指定地址写入一个16位数据
****************************************************************************/
void Lcd_WriteData_16Bit(u16 Data)
{	
	Lcd_WriteData(Data>>8);
	Lcd_WriteData(Data);	
}
/*******************************************************************************
* Function Name  : LCD_RD_DATA     ＹＺ
* Description    : 读LCD数据函数
* Input          : None
* Output         : None
* Return         : 读到的值
*******************************************************************************/
u16 LCD_RD_DATA(void)  //库函数读
{
	vu16 ram;			//防止被优化
//	ram=LCD->LCD_RAM;	
	return ram;	 
}
/****************************************************************************
* 名    称：void Lcd_Reset(void)
* 功    能：液晶硬复位函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化前需执行一次复位操作
****************************************************************************/
void Lcd_Reset(void)
{
	//LCD_RST_SET;
	//delay_ms(100);
	LCD_RST_CLR;
	delay_ms(200);
	LCD_RST_SET;
	delay_ms(100);
}
/****************************************************************************
* 名    称：void Lcd_Init(void)
* 功    能：液晶初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化_ILI9225_176X220
****************************************************************************/
void Lcd_Init(void)
{	
	SPI2_Init();
	Lcd_Reset(); //Reset before LCD Init.

//	lcddev.width=320;    //LCD 宽度
//	lcddev.height=240;   //LCD 高度
//	lcddev.setxcmd=0X2A;  //设置x坐标指令
//	lcddev.setycmd=0X2B;  //设置y坐标指令
//	lcddev.wramcmd=0X2C;  //开始写gram指令
	//delay_ms(120);                //ms

	delay_ms(100);
	/* Sleep Out */
	Lcd_WriteIndex(0x11);
	delay_ms(120);    //Delay 120ms
	/* Memory Data Access Control */
	//LCD_WriteReg(0x36,0x60);//横屏 从左到右 从上到下
	 Lcd_WriteIndex(0x36);			
	Lcd_WriteData(0x70);
	/* RGB 5-6-5-bit  */
	//LCD_WriteReg(0x3A,0x05);
	 Lcd_WriteIndex(0x3A);			
	Lcd_WriteData(0x05);

	 Lcd_WriteIndex(0xB2);			
	Lcd_WriteData(0x0C);
	Lcd_WriteData(0x0C); 
	Lcd_WriteData(0x00); 
	Lcd_WriteData(0x33); 
	Lcd_WriteData(0x33); 			

	 Lcd_WriteIndex(0xB7);			
	Lcd_WriteData(0x35);

	 Lcd_WriteIndex(0xBB);			
	Lcd_WriteData(0x32); //Vcom=1.35V
					
	 Lcd_WriteIndex(0xC2);
	Lcd_WriteData(0x01);

	 Lcd_WriteIndex(0xC3);			
	Lcd_WriteData(0x15); //GVDD=4.8V  颜色深度
				
	 Lcd_WriteIndex(0xC4);			
	Lcd_WriteData(0x20); //VDV, 0x20:0v

	 Lcd_WriteIndex(0xC6);			
	Lcd_WriteData(0x0F); //0x0F:60Hz        	

	 Lcd_WriteIndex(0xD0);			
	Lcd_WriteData(0xA4);
	Lcd_WriteData(0xA1); 

	 Lcd_WriteIndex(0xE0);
	Lcd_WriteData(0xD0);   
	Lcd_WriteData(0x08);   
	Lcd_WriteData(0x0E);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x05);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x33);   
	Lcd_WriteData(0x48);   
	Lcd_WriteData(0x17);   
	Lcd_WriteData(0x14);   
	Lcd_WriteData(0x15);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x34);   

	Lcd_WriteIndex(0xE1);     
	Lcd_WriteData(0xD0);   
	Lcd_WriteData(0x08);   
	Lcd_WriteData(0x0E);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x09);   
	Lcd_WriteData(0x15);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x33);   
	Lcd_WriteData(0x48);   
	Lcd_WriteData(0x17);   
	Lcd_WriteData(0x14);   
	Lcd_WriteData(0x15);   
	Lcd_WriteData(0x31);   
	Lcd_WriteData(0x34);
	
	Lcd_WriteIndex(0x21);
	Lcd_WriteIndex(0x11);
	delay_ms(120);
	Lcd_WriteIndex(0x29);
	//Lcd_WriteIndex(0x11);
	//delay_ms(120);    //Delay 120ms
//	LCD_WriteReg(0xB2, 0x0C);
//    LCD_WriteReg(0xB2, 0x0C); 
//    LCD_WriteReg(0xB2, 0x00);   
//    LCD_WriteReg(0xB2, 0x33);   
//    LCD_WriteReg(0xB2, 0x33);   

//    LCD_WriteReg(0xB7, 0x35);   //VGH=13.26V, VGL=-10.43V

//    LCD_WriteReg(0xBB, 0x32);   //VCOM

//   // LCD_WriteReg(0xC0, 0x2C);   

//    LCD_WriteReg(0xC2, 0x01);   

//    LCD_WriteReg(0xC3, 0x15); //VAP  //5V

//    LCD_WriteReg(0xC4, 0x20);   

//    LCD_WriteReg(0xC6, 0x0F);   

//    LCD_WriteReg(0xD0, 0xA4);   
//    LCD_WriteReg(0xD0, 0xA1);   

//    LCD_WriteReg(0xE0 , 0xD0);   
//    LCD_WriteReg(0xE0, 0x08);   
//    LCD_WriteReg(0xE0, 0x0E);   
//    LCD_WriteReg(0xE0, 0x09);   
//    LCD_WriteReg(0xE0 , 0x09);   
//    LCD_WriteReg(0xE0 , 0x05);   
//    LCD_WriteReg(0xE0, 0x31);   
//    LCD_WriteReg(0xE0, 0x33);   
//    LCD_WriteReg(0xE0, 0x48);   
//    LCD_WriteReg(0xE0, 0x17);   
//    LCD_WriteReg(0xE0 , 0x14);   
//    LCD_WriteReg(0xE0, 0x15);   
//    LCD_WriteReg(0xE0 , 0x31);   
//    LCD_WriteReg(0xE0 , 0x34);   


//    LCD_WriteReg(0xE1, 0xD0);   
//    LCD_WriteReg(0xE1 , 0x08);   
//    LCD_WriteReg(0xE1, 0x0E);   
//    LCD_WriteReg(0xE1 , 0x09);   
//    LCD_WriteReg(0xE1, 0x09);   
//    LCD_WriteReg(0xE1, 0x15);   
//    LCD_WriteReg(0xE1 , 0x31);   
//    LCD_WriteReg(0xE1, 0x33);   
//    LCD_WriteReg(0xE1, 0x48);   
//    LCD_WriteReg(0xE1 , 0x17);   
//    LCD_WriteReg(0xE1 , 0x14);   
//    LCD_WriteReg(0xE1, 0x15);   
//    LCD_WriteReg(0xE1, 0x31);   
//    LCD_WriteReg(0xE1 , 0x34);
//	
//	LCD_WriteReg(0x21, 0x00);
//    LCD_WriteReg(0x29,0x00);      

//    LCD_WriteReg(0x2A, 0x00);   
//    //LCD_WriteReg(0x2A , 0);   
//    LCD_WriteReg(0x2A , 0x00);   
//    LCD_WriteReg(0x2A , 0xEF);   
//     
//    LCD_WriteReg(0x2B, 0x00);   
//    //LCD_WriteReg(0x2B , 0);   
//    LCD_WriteReg(0x2B , 0x00);   
//    LCD_WriteReg(0x2B , 0xEF);   

//	Lcd_WriteIndex(0x13);
//	delay_ms(20);
//	Lcd_WriteIndex(0x29);
//	delay_ms(20);
//	Lcd_WriteIndex(0x36);
//	delay_ms(20);
//	Lcd_WriteData(0x70);
	//LCD_BL(1);
	//Lcd_SetRegion(0, 0,319,239);
}



/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(u16 Xpos, u16 Ypos)
{	
	Lcd_WriteIndex(0x2A);
	Lcd_WriteData_16Bit( Xpos);
	Lcd_WriteIndex(0x2B);
	Lcd_WriteData_16Bit( Ypos);
	Lcd_WriteIndex(0x2C);		
} 
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
//设置显示窗口
void Lcd_SetRegion(u16 xStart, u16 yStart,u16 xEnd,u16 yEnd)
{
	Lcd_WriteIndex(0x2A);
	Lcd_WriteData_16Bit(xStart);	
	Lcd_WriteData_16Bit(xEnd);
	
	Lcd_WriteIndex(0x2B);
	Lcd_WriteData_16Bit(yStart);	
	Lcd_WriteData_16Bit(yEnd);
	
	Lcd_WriteIndex(0x2C);	
}

	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：xy坐标和颜色数据
返回值：无
*************************************************/
void Gui_DrawPoint(u16 x,u16 y,u16 Data)
{
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);
}    

/*****************************************
 函数功能：读TFT某一点的颜色                          
 出口参数：color  点颜色值                                 
******************************************/
u16 Lcd_ReadPoint(u16 x,u16 y)
{
  unsigned int Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
//void Lcd_Clear(u16 x0, u16 y0,u16 x1,u16 y1,u16 Color)               
//{	
//   unsigned int i,m;
//   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
//   for(i=0;i<X_MAX_PIXEL;i++)
//    for(m=0;m<Y_MAX_PIXEL;m++)
//    {	
//	  	Lcd_WriteData_16Bit(Color);
//    }  

//}
void Lcd_Color(u16 x0, u16 y0,u16 x1,u16 y1,u16 Color)               
{	
 static  u16 i,m;
   Lcd_SetRegion(x0,y0,x1,y1);
   for(i=x0;i<(x1+1);i++)
    for(m=y0;m<(y1+1);m++)
    {	
	  	Lcd_WriteData_16Bit(Color);
		//Lcd_WriteData(Color);
    } 
	//Lcd_SetRegion(0,0,319,239);	
}

//从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式。
//通过该函数转换
//c:GBR格式的颜色值
//返回值：RGB格式的颜色值
u16 LCD_BGR2RGB(u16 c)
{
  u16  r,g,b,rgb;   
  b=(c>>0)&0x1f;
  g=(c>>5)&0x3f;
  r=(c>>11)&0x1f;	 
  rgb=(b<<11)+(g<<5)+(r<<0);		 
  return(rgb);

}



// 画圆函数
void Gui_Circle(u16 X,u16 Y,u16 R,u16 fc) 
{//Bresenham算法 
    unsigned short  a,b; 
    int c; 
    a=0; 
    b=R; 
    c=3-2*R; 
    while (a<b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc);     //        7 
        Gui_DrawPoint(X-a,Y+b,fc);     //        6 
        Gui_DrawPoint(X+a,Y-b,fc);     //        2 
        Gui_DrawPoint(X-a,Y-b,fc);     //        3 
        Gui_DrawPoint(X+b,Y+a,fc);     //        8 
        Gui_DrawPoint(X-b,Y+a,fc);     //        5 
        Gui_DrawPoint(X+b,Y-a,fc);     //        1 
        Gui_DrawPoint(X-b,Y-a,fc);     //        4 

        if(c<0) c=c+4*a+6; 
        else 
        { 
            c=c+4*(a-b)+10; 
            b-=1; 
        } 
       a+=1; 
    } 
    if (a==b) 
    { 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y+b,fc); 
        Gui_DrawPoint(X+a,Y-b,fc); 
        Gui_DrawPoint(X-a,Y-b,fc); 
        Gui_DrawPoint(X+b,Y+a,fc); 
        Gui_DrawPoint(X-b,Y+a,fc); 
        Gui_DrawPoint(X+b,Y-a,fc); 
        Gui_DrawPoint(X-b,Y-a,fc); 
    } 
	
} 
//画线函数，使用Bresenham 画线算法
void Gui_DrawLine(u16 x1, u16 y1,u16 x2, u16 y2,u16 Color)   
{
u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; //设置单步方向 
	else if(delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )//画线输出 
	{  
		 Gui_DrawPoint(uRow,uCol,Color);//画点 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/*
**************************************************************************
* 不等号字符
**************************************************************************
*/
void Gui_Unequal_character(u16 x,u16 y,u16 fc)
{
	Gui_DrawLine(x,y+4,x+6,y+4,fc);
	Gui_DrawLine(x,y+7,x+6,y+7,fc);
	Gui_DrawLine(x+5,y+2,x+1,y+9,fc);
}
/*
***************************************************************************
*
***************************************************************************
*/
void Gui_box(u16 x, u16 y, u16 w, u16 h,u16 bc)
{
	Gui_DrawLine(x,y,x+w,y,0xEF7D);
	Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
	Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
	Gui_DrawLine(x,y,x,y+h,0xEF7D);
    Gui_DrawLine(x+1,y+1,x+1+w-2,y+1+h-2,bc);
	
}
/*
********************************************************************
*
********************************************************************
*/
void Gui_box2(u16 x,u16 y,u16 w,u16 h, u8 mode)
{
	if (mode==0)	{
		Gui_DrawLine(x,y,x+w,y,0xEF7D);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0x2965);
		Gui_DrawLine(x,y+h,x+w,y+h,0x2965);
		Gui_DrawLine(x,y,x,y+h,0xEF7D);
		}
	if (mode==1)	{
		Gui_DrawLine(x,y,x+w,y,0x2965);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xEF7D);
		Gui_DrawLine(x,y+h,x+w,y+h,0xEF7D);
		Gui_DrawLine(x,y,x,y+h,0x2965);
	}
	if (mode==2)	{
		Gui_DrawLine(x,y,x+w,y,0xffff);
		Gui_DrawLine(x+w-1,y+1,x+w-1,y+1+h,0xffff);
		Gui_DrawLine(x,y+h,x+w,y+h,0xffff);
		Gui_DrawLine(x,y,x,y+h,0xffff);
	}
}


/*
*************************************************************************************
* 功能描述: 在屏幕显示一凸起的按钮框
* 输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
* 输    出: 无
*************************************************************************************
*/
void DisplayButtonDown(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, GRAY2);  //H
	Gui_DrawLine(x1+1,y1+1,x2,y1+1, GRAY1);  //H
	Gui_DrawLine(x1,  y1,  x1,y2, GRAY2);  //V
	Gui_DrawLine(x1+1,y1+1,x1+1,y2, GRAY1);  //V
	Gui_DrawLine(x1,  y2,  x2,y2, WHITE);  //H
	Gui_DrawLine(x2,  y1,  x2,y2, WHITE);  //V
}

/*
*************************************************************************************
* 功能描述: 在屏幕显示一凹下的按钮框
* 输    入: u16 x1,y1,x2,y2 按钮框左上角和右下角坐标
* 输    出: 无
*************************************************************************************
*/
void DisplayButtonUp(u16 x1,u16 y1,u16 x2,u16 y2)
{
	Gui_DrawLine(x1,  y1,  x2,y1, WHITE); //H
	Gui_DrawLine(x1,  y1,  x1,y2, WHITE); //V
	
	Gui_DrawLine(x1+1,y2-1,x2,y2-1, GRAY1);  //H
	Gui_DrawLine(x1,  y2,  x2,y2, GRAY2);  //H
	Gui_DrawLine(x2-1,y1+1,x2-1,y2, GRAY1);  //V
    Gui_DrawLine(x2  ,y1  ,x2,y2, GRAY2); //V
}
/*
*************************************************************************************
* 功能描述: 在屏幕显示一圆角矩形
* 输    入: u16 x,y, 起点坐标
*			w:宽度；h:高度；fc:矩形边线颜色；
* 输    出: 无
*************************************************************************************
*/
void LCD_RectangleWindow(u16 x, u16 y, u16 w, u16 h, u16 fc)
{
	u16 x0,y0;
	for(y0=0;y0<h;y0 += h-1)
	{
		for(x0=2;x0<w-2;x0 +=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
	for(x0=0;x0<w;x0 +=w-1)
	{
		for(y0=2;y0<h-2;y0 +=1)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
	for(x0=1;x0<w-1;x0 +=w-3)
	{
		for(y0=1;y0<h-1;y0 +=h-3)
		{
			Gui_DrawPoint(x0+x,y0+y,fc);
		}
	}
}
//******************************************************************
//函数名：  LCD_ShowChar
//功能：    显示单个英文字符
//输入参数：(x,y):字符显示位置起始坐标
//        	fc:前置画笔颜色
//			bc:背景颜色
//			num:字符
//			size:字体大小
//			mode:模式  0,填充模式;1,叠加模式
//返回值：  无
//修改记录：无
//******************************************************************  
void LCD_ShowChar(u16 x,u16 y,u16 fc,u16 bc,u8 size,u8 mode,u8 num)
{  							  
    u8 temp,t1,t;
	u16 y0=y;
	u8 csize=(size/8+((size%8)?1:0))*(size/2);	//得到字体一个字符对应点阵集所占的字节数
	//if(size==48)
 	num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	//else num=num-' ';//得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）
	for(t=0;t<csize;t++)
	{   
		if(size==12)temp=asc2_1206[num][t]; 	 	//调用1206字体
		else if(size==16)temp=asc2_1608[num][t];	//调用1608字体
		else if(size==24)temp=asc2_2412[num][t];	//调用2412字体
		else if(size==32)temp=asc2_3216[num][t];	//调用3216字体
		//else if(size==40)temp=asc2_4020[num][t];	//调用3216字体
		else return;								//没有的字库
		for(t1=0;t1<8;t1++)
		{			    
			if(temp&0x80)Gui_DrawPoint(x,y,fc);
			else if(mode==0)Gui_DrawPoint(x,y,bc);
			temp<<=1;
			y++;
			if(y>=240)return;		//超区域了
			if((y-y0)==size)
			{
				y=y0;
				x++;
				if(x>=320)return;	//超区域了
				break;
			}
		}  	 
	}  	    	   	 	  
}   
//******************************************************************
//函数名：  LCD_ShowString
//作者：    xiao冯@全动电子
//日期：    2013-02-22
//功能：    显示英文字符串
//输入参数：x,y :起点坐标	 
//width,height:区域大小  
//size:		字体大小
//mode:		0：填充，1：叠加；
//*p:字符串起始地址
//返回值：  无
//修改记录：无
//******************************************************************  	  
void LCD_ShowString(u16 x,u16 y,u16 fc,u16 bc,u8 size,u8 mode,char *p)
{         
	u8 x0=x;
	
    while((*p<='~')&&(*p>=' '))//判断是不是非法字符!
    {       
        if(x>=319){x=x0;y+=size;}
        if(y>=239)break;//退出
        LCD_ShowChar(x,y,fc,bc,size,mode,*p);
        x+=size/2;
        p++;
    }  
}

//m^n函数
//返回值:m^n次方.
u32 LCD_Pow(u8 m,u8 n)
{
	u8 i;
	u32 result=1;
	for(i=0;i<n;i++)
	{
		result*=m;
	}		
	return result;
}
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum_Step(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num,u8 step)
{         	
	u8 t,temp;
	u8 enshow=0;
	u16 fc0;
	//len=(u8)(log10(num));	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		//temp=(u8)log10(num);
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,' ');
				continue;
			}else enshow=1;  
		}
		if(step==len-1-t)fc0=RED;
		else fc0=fc;
			
	 	LCD_ShowChar(x+(size/2)*t,y,fc0,bc,size,0,temp+'0'); 
	}
}
//显示数字,高位为0,则不显示
//x,y :起点坐标	 
//len :数字的位数
//size:字体大小
//color:颜色 
//num:数值(0~4294967295);	 
void LCD_ShowNum(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num)
{         	
	u8 t,temp;
	u8 enshow=0;
	//len=(u8)(log10(num));	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		//temp=(u8)log10(num);
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,' ');
				continue;
			}else enshow=1;
	
		}
	 	LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,temp+'0'); 
	}
}
void LCD_Showvol(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u32 num)
{         	
	u8 t,temp;
	u8 enshow=0;
	//len=(u8)(log10(num));	
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		//temp=(u8)log10(num);
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,' ');
				continue;
			}else enshow=1;
	
		}
	 	LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,0,temp+'0'); 
	}
}
/*
*********************************************************************
* 显示数字,高位为0,还是显示  YZ
* x,y:起点坐标
* num:数值(0~999999999);	 
* len:长度(即要显示的位数)
* size:字体大小
* mode:
* [7]:0,不填充;1,填充0.
* [6:1]:保留
* [0]:0,非叠加显示;1,叠加显示.
*********************************************************************
*/
void LCD_ShowxNum(u16 x,u16 y,u16 fc,u16 bc,u8 len,u8 size,u8 mode,u32 num)
{  
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,mode&0X01,'0');  
				else LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,mode&0X01,' ');  
 				continue;
			}else enshow=1; 
		 	 
		}
	 	LCD_ShowChar(x+(size/2)*t,y,fc,bc,size,mode&0X01,temp+'0'); 
	}
}
/*
******************************************************************************
* 频率显示函数  
* x,y:起点坐标
* freq:数值(0~999999999);
* fc: 字体颜色
* bc: 底色
* size:字体大小
******************************************************************************
*/
void LCD_Frequency_Display(u16 x, u16 y, u16 fc, u16 bc, u8 size, u32 freq)
{
	u16 a, b, c;
	a = freq/1000000 ;
	b = freq%1000000/1000 ;
	c = freq%1000/10 ;
	LCD_ShowNum(x, y, fc, bc, 3, size, a);
	LCD_ShowChar(x+(size/2)*3, y, fc, bc,size,0,'.');
	LCD_ShowxNum(x+(size/2)*4, y, fc, bc, 3, size, 0x80, b);
	LCD_ShowChar(x+(size/2)*7,y, fc, bc,size,0,'.');
	LCD_ShowxNum(x+(size/2)*8, y, fc, bc, 2, size, 0x80, c);
}





