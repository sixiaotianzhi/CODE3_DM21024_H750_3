//#ifndef __MEASUREINERFACE_H
//#define __MEASUREINERFACE_H

//#include <stdint.h>

#ifndef __MEASUREINERFACE_H
#define __MEASUREINERFACE_H

#include <stdint.h>


////测温分档位参数
typedef struct MEASURE_Fitler_PARAM 
{
	int kj;      //+1     
	short b;     //B     
	short b2;    //B2
	short kf;    //K
	short kf_2;  //KF_2
	short kf_2_distance;//kf_2 ??????????
	short kf_2_base_distance;//kf_2 ??????????	
	int Reserved4;
	
	int Reserved5;
	int Reserved6;
	int Reserved7;
	int Reserved8;
	int Reserved9;
	int Reserved10;
	//END
	short Reserved11;//????????

	float Reserved12;
	float Reserved13;
	float Reserved14;
	float Reserved15;
	float Reserved16;
	float Reserved17;
	unsigned short m_usExpectBright; //??
	unsigned short m_usExpectContrast; //???
}Msr_Flt_PRM;


typedef struct MEASURE_PARAM {
	short realTimeTs;//实时快门温  
	short realTimeTfpa; // 实时焦温
	short realTimeTlen; //实时镜筒温
	short Reserved1;  
	short Reserved2;
	short Reserved3;
	short Reserved4;       
	short MeasureRealGear;  //实际档位,1,2
	short MeasureDegree;   //要求测温档位,1,2,3 
  //START
	short emiss;      //发射率   1   100
	short Reserved5;
	short distance;   //距离  0.3  5  3   50
	short reflectT;    //反射温度    
	short Reserved6; //

	Msr_Flt_PRM fliter[2];

}MeasureParamC;


typedef struct{
	unsigned short Width;
	unsigned short height;
	unsigned short PostXMin;
	unsigned short PostYMin;
	unsigned short PostXMax;
	unsigned short PostYMax;
	unsigned short PostXCursor;
	unsigned short PostYCursor;
	float MinValue;
	float MaxValue;
	float CursorValue;
} IRInfo;

typedef struct{
	uint32_t CompensateNUCVtemp; //*1000
	uint32_t CompensateShuttterVtemp; //*1000
	uint32_t AutoShutterInterval;  //场数
	uint32_t MC_Value;
}ShutterPara;

typedef struct {

	uint16_t m_usHeight;
	uint16_t m_usWidth;
	
	/** 算法开关 **/	
  uint8_t uWeightTemporalFilterSwitch;
	uint8_t uGrayFilterSwitch;
	uint8_t uLaplaceSharpenSwitch;
	uint8_t uLinearDrtSwitch;
}AlgPara;

extern uint8_t shutter_id;

extern short GetavgShutter(void);

extern void DealShutterInterface( short jpmT, short realTimeJpmTemp,short realTimelensTemp,short realTimeShutterTemp);


extern void GetTempInfo(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,short*yData,short us_avg_shutter,IRInfo*info,MeasureParamC*measureParam);

extern void DeGreeSwitch(uint8_t MeasureDegreeSet,uint8_t MeasureDegreeSwitch,uint32_t CurrentMaxTemp,uint32_t CurrentMinTemp);

//待实现接口
extern void  MEASURE_ALLOC_AssignMemory(void * p, uint32_t NumBytes);
extern void InitMeasurePara(ShutterPara*Para);
extern char SetMeasureDegree(uint8_t degree);

//算法相关
extern void AlgInit(AlgPara AlgParaValue);
extern void DrtSetPara(uint16_t Bright,uint16_t contrast);

extern void ALG_ALLOC_AssignMemory(void * p, uint32_t NumBytes);
extern void InfraredProcessInterface(uint16_t *uspSrcImage);
extern void LinearDrtInterface(uint8_t *ucpDstImage, uint16_t *uspSrcImage);
extern void y8_to_rgb(uint8_t *y8_buf, uint16_t *rgb_buf, uint8_t color_bar_index);
extern void YToRGB565(unsigned char * pSrc,unsigned short * pDst,unsigned char index);
uint64_t GetMeasureLibVersion(void);
#endif
