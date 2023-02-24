/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif
#include "stm32h7xx_hal.h"

#define USB_EN          1
#define USB_FLASH_EN    0
// #define CAL_PARAM_EN    1

#define SPI1_DMA_EN     1
#define SD_EN           1
#define PWM_EN          0
#define KEY_EN          1
#define KEY_MENU_EN     0
#define KEY_PIC_EN      1
#define RTOS_EN         1
#define ADC_EN          1

#define AUTO_SLEEP_5MIN             5*60
#define AUTO_SLEEP_15MIN            15*60
#define AUTO_SLEEP_30MIN            30*60
#define AUTO_SLEEP_ON               1
#define AUTO_SLEEP_OFF              0

#define COMP_VTEMP                  -30//320

#define LCD_W                       320
#define LCD_H                       240

#define COLOR_PALETTE_HSV           0
#define COLOR_PALETTE_JET           1
#define COLOR_PALETTE_Grayscale     2
#define COLOR_PALETTE_HOT           3
// #define COLOR_PALETTE_CLUT1         4
// #define COLOR_PALETTE_Grayscale2    5


// #define ADC_RDY_Pin GPIO_PIN_2
// #define ADC_RDY_GPIO_Port GPIOE
// #define ADC_RDY_EXTI_IRQn EXTI2_IRQn

#define DRV8837_SLEEP_Pin GPIO_PIN_1
#define DRV8837_SLEEP_GPIO_Port GPIOB
#define SHUTTER_INTA_Pin GPIO_PIN_5
#define SHUTTER_INTA_GPIO_Port GPIOC
#define SHUTTER_INTB_Pin GPIO_PIN_0
#define SHUTTER_INTB_GPIO_Port GPIOB


#define CAL_DATAVALID_Pin GPIO_PIN_3
#define CAL_DATAVALID_GPIO_Port GPIOB
#define TCAL_EN_Pin GPIO_PIN_7
#define TCAL_EN_GPIO_Port GPIOD
#define SENSOR_RESET_Pin GPIO_PIN_3
#define SENSOR_RESET_GPIO_Port GPIOE

// #define LED1_Pin GPIO_PIN_2
// #define LED1_GPIO_Port GPIOA
// #define LED2_Pin GPIO_PIN_3
// #define LED2_GPIO_Port GPIOA

//===POWER
#define POWE_EN_Pin GPIO_PIN_15
#define POWE_EN_GPIO_Port GPIOE

#define P_EN_1_Pin GPIO_PIN_10
#define P_EN_1_GPIO_Port GPIOA
#define P_EN_2_Pin GPIO_PIN_9
#define P_EN_2_GPIO_Port GPIOA
#define P_EN_3_Pin GPIO_PIN_9
#define P_EN_3_GPIO_Port GPIOE

#define ON_OFF_Pin GPIO_PIN_8
#define ON_OFF_GPIO_Port GPIOE

#define USB_IN_Pin GPIO_PIN_10
#define USB_IN_GPIO_Port GPIOE
//+++

//===LCD 
#define ST7789_RST_PORT     GPIOB
#define ST7789_RST_PIN      GPIO_PIN_15

#define ST7789_DC_PORT      GPIOD
#define ST7789_DC_PIN       GPIO_PIN_8

#define ST7789_CS_PORT      GPIOD
#define ST7789_CS_PIN       GPIO_PIN_9

#define ST7789_LCDBL_PORT      GPIOD
#define ST7789_LCDBL_PIN       GPIO_PIN_13

//===QSPI
/* QSPI引脚和时钟相关配置宏定义 */
#define QSPI_CLK_ENABLE()              __HAL_RCC_QSPI_CLK_ENABLE()
#define QSPI_CLK_DISABLE()             __HAL_RCC_QSPI_CLK_DISABLE()
#define QSPI_CS_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_CLK_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOB_CLK_ENABLE()
#define QSPI_BK1_D0_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_BK1_D1_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOD_CLK_ENABLE()
#define QSPI_BK1_D2_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOE_CLK_ENABLE()
#define QSPI_BK1_D3_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()

#define QSPI_MDMA_CLK_ENABLE()         __HAL_RCC_MDMA_CLK_ENABLE()
#define QSPI_FORCE_RESET()             __HAL_RCC_QSPI_FORCE_RESET()
#define QSPI_RELEASE_RESET()           __HAL_RCC_QSPI_RELEASE_RESET()

#define QSPI_CS_PIN                    GPIO_PIN_10
#define QSPI_CS_GPIO_PORT              GPIOB

#define QSPI_CLK_PIN                   GPIO_PIN_2
#define QSPI_CLK_GPIO_PORT             GPIOB

#define QSPI_BK1_D0_PIN                GPIO_PIN_11
#define QSPI_BK1_D0_GPIO_PORT          GPIOD

#define QSPI_BK1_D1_PIN                GPIO_PIN_12
#define QSPI_BK1_D1_GPIO_PORT          GPIOD

#define QSPI_BK1_D2_PIN                GPIO_PIN_2
#define QSPI_BK1_D2_GPIO_PORT          GPIOE

#define QSPI_BK1_D3_PIN                GPIO_PIN_1
#define QSPI_BK1_D3_GPIO_PORT          GPIOA

void Error_Handler(void);

extern void UserInitUsb(char mode);
extern void MX_SPI1_Init(void);
extern void MX_SPI1_Init2(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
