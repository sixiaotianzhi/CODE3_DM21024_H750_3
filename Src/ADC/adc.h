#ifndef __ADC_H
#define __ADC_H
#include "sys.h"

void MY_ADC_Init(void);
u32  Get_Adc(u8 ch);
u32  Get_Adc3(u8 ch);
u32 Get_Adc_Average(u8 ch,u8 times);//得到某个通道给定次数采样的平均值
u32 Get_Adc3_Average(u8 ch,u8 times);


#endif 
