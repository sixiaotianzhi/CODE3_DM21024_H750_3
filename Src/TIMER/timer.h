#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//定时器驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/8/12
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
extern TIM_HandleTypeDef TIM4_Handler;      //定时器3PWM句柄 
extern TIM_OC_InitTypeDef TIM4_CH2Handler; 	//定时器3通道4句柄

void TIM4_Init(u16 arr,u16 psc);    		//定时器初始化
void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM_SetTIM4Compare2(u32 compare);
#endif

