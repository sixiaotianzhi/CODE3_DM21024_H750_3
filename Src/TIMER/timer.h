#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//��ʱ����������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2017/8/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
extern TIM_HandleTypeDef TIM4_Handler;      //��ʱ��3PWM��� 
extern TIM_OC_InitTypeDef TIM4_CH2Handler; 	//��ʱ��3ͨ��4���

void TIM4_Init(u16 arr,u16 psc);    		//��ʱ����ʼ��
void TIM4_PWM_Init(u16 arr,u16 psc);
void TIM_SetTIM4Compare2(u32 compare);
#endif

