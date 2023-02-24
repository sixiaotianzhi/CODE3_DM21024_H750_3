#ifndef _LED_H
#define _LED_H
#include "sys.h"

#define LED1(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8)) //LED1输出电平翻转

void LED_Init(void); //LED初始化函数

#endif
