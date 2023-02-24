#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

#define KEY1        HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)  
#define KEY2        HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_0) 
#define KEY3        HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_8)
#define KEY4        HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_15)

#define KEY5        HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_8)//===ON_OFF
#define USB_IN      HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_10)

#define KEY1_PRES 	1  	
#define KEY2_PRES	2	
#define KEY3_PRES	3	
#define KEY4_PRES   4
#define KEY5_PRES   5

#define KEY1_OK 	KEY1_PRES  	
#define KEY2_DOWN 	KEY2_PRES	
#define KEY3_UP	    KEY3_PRES	
#define KEY4_BACK   KEY4_PRES
#define KEY5_POWER  KEY5_PRES

void KEY_Init(void);  //按键IO初始化函数
u8 KEY_Scan(u8 mode); //按键扫描函数


#endif
