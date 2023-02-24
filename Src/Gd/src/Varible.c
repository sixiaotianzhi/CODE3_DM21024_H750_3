#include "Varible.h"
#include "Sensor.h"
//#include "MeasureInterface.h"

SPI_Reg_Data SPI_Reg_Data120[21]={
																						{0x10,0x2f}, 
																						{0x11,0x00}, 
																						{0x12,0x00}, 
																						{0x13,0x00}, 
																						{0x14,0x00}, 
																						{0x15,0x01},
																						{0x16,0xc9},
																						{0x17,0x80},
																						{0x18,0x1d},
																						{0x19,0x34}, //===0x34 x2=1>>test x1x0=0>>8bit+6bit低位在前  //7bit 7bit //0x32
																						{0x1a,0x38},
																						{0x1b,0x30},
																						{0x1c,0x0c},//===x3x2=11>>8dummy
																						{0x20,0xfa},//===0x00fa   	//int		 320 --100  190 --50									
																						{0x21,0x00},
																						{0x22,0xd0},//===0x07d0  //TLINE //d0
																						{0x23,0x07},					//07   5M																																						
																						{0x29,0x5F},    //NUC level 5F
																						{0x2A,0x30},    //30
																						{0xf0,0x6d},
																						{0xf1,0x24},					
																		};

Loop_Data Loop_Data120={												
												.GFID_High=15000, 
												.GFID_Low=1000,
												.RES_High=15000,
												.RES_Low=1000,
												.NUC_High=6000,
												.NUC_Low=4000,
												.ONTime=2,
												.OFFTime=2,
												.HoldTime=2,
												.us_avg_shutter =0,
												};

uint8_t shutter_id=0,history_shutter_id=0;		
uint8_t ImageReady=0;		
int16_t*pImage;
uint16_t ImageY8[120][90]={0};

ShutterPara Para120={300,1500,750,5};
uint8_t MeasureMem[42*1024];	
uint8_t AlgMem[132*1024]; //__attribute__((at(0x30000000)));
AlgPara AlgPara120={120,90,1,1,0,0};

MeasureParamC measureParam120 = 
{
		      .MeasureRealGear = 1,
			    .MeasureDegree  = 1,
					.emiss = 98,
					.distance =5.0,
			    .fliter[1] = 
			    {
						  .kj  = 3000,
							.kf   = 10000,
							.b    =  0,
							.kf_2 = 10000, 
							.b2   =  0,
							.kf_2_distance =12,
							.kf_2_base_distance =5,	
				      .m_usExpectBright = 110, //??
				      .m_usExpectContrast = 470, //???						
			     },
				  .fliter[0] = 
			    {
						  .kj  = 3000,
						  .kf    = 10000,
						  .b     =  0,
						  .kf_2 = 10000, 
							.b2   =  0,
							.kf_2_distance = 12,
							.kf_2_base_distance =5,				
				      .m_usExpectBright = 70, //??
				      .m_usExpectContrast = 210, //???						
				  },
};

IRInfo IRInfo120={
			 .Width=120,
			 .height=90,
			 .PostXCursor=60,
			 .PostYCursor=45,
};		
