/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32h7xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
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

#include "main.h"

extern DMA_HandleTypeDef hdma_dcmi;
extern MDMA_HandleTypeDef hmdma;

void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* System interrupt init*/
  /* PendSV_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
}

void HAL_DCMI_MspInit(DCMI_HandleTypeDef* hdcmi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hdcmi->Instance==DCMI)
  {
    __HAL_RCC_DCMI_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    // hdma_dcmi.Instance = DMA1_Stream0;
    // hdma_dcmi.Init.Request = DMA_REQUEST_DCMI;
    // hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
    // hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
    // hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
    // hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    // hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    // hdma_dcmi.Init.Mode = DMA_CIRCULAR;
    // hdma_dcmi.Init.Priority = DMA_PRIORITY_MEDIUM;
    // hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    // if (HAL_DMA_Init(&hdma_dcmi) != HAL_OK)
    // {
    //   Error_Handler();
    // }
    // __HAL_LINKDMA(hdcmi,DMA_Handle,hdma_dcmi);

    // HAL_NVIC_SetPriority(DCMI_IRQn, 5, 0);
    // HAL_NVIC_EnableIRQ(DCMI_IRQn);
  }
}

void HAL_DCMI_MspDeInit(DCMI_HandleTypeDef* hdcmi)
{
  if(hdcmi->Instance==DCMI)
  {
  /* USER CODE BEGIN DCMI_MspDeInit 0 */

  /* USER CODE END DCMI_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_DCMI_CLK_DISABLE();
  
    /**DCMI GPIO Configuration    
    PE5     ------> DCMI_D6
    PE6     ------> DCMI_D7
    PI5     ------> DCMI_VSYNC
    PI4     ------> DCMI_D5
    PH14     ------> DCMI_D4
    PH12     ------> DCMI_D3
    PH11     ------> DCMI_D2
    PH10     ------> DCMI_D1
    PH9     ------> DCMI_D0
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK 
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_5|GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_5|GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_14|GPIO_PIN_12|GPIO_PIN_11|GPIO_PIN_10 
                          |GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_4|GPIO_PIN_6);

    /* DCMI DMA DeInit */
    HAL_DMA_DeInit(hdcmi->DMA_Handle);

    /* DCMI interrupt DeInit */
    HAL_NVIC_DisableIRQ(DCMI_IRQn);
  /* USER CODE BEGIN DCMI_MspDeInit 1 */

  /* USER CODE END DCMI_MspDeInit 1 */
  }

}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspInit 0 */

  /* USER CODE END I2C2_MspInit 0 */
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I2C2 GPIO Configuration    
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Peripheral clock enable */
    __HAL_RCC_I2C2_CLK_ENABLE();
  /* USER CODE BEGIN I2C2_MspInit 1 */

  /* USER CODE END I2C2_MspInit 1 */
  }

}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if(hi2c->Instance==I2C2)
  {
  /* USER CODE BEGIN I2C2_MspDeInit 0 */

  /* USER CODE END I2C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I2C2_CLK_DISABLE();
  
    /**I2C2 GPIO Configuration    
    PB10     ------> I2C2_SCL
    PB11     ------> I2C2_SDA 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

  /* USER CODE BEGIN I2C2_MspDeInit 1 */

  /* USER CODE END I2C2_MspDeInit 1 */
  }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hspi->Instance==SPI1)
  {
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    
    /**SPI1 GPIO Configuration    
    PA5     ------> SPI1_SCK
    PA7     ------> SPI1_MOSI 
    PB4     ------> SPI1_MISO
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);    
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  else if(hspi->Instance==SPI2)
  {
    __HAL_RCC_SPI2_CLK_ENABLE();
  
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

		GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }
  else if(hspi->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspInit 0 */

  /* USER CODE END SPI4_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_SPI4_CLK_ENABLE();
  
    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**SPI4 GPIO Configuration    
    PE13     ------> SPI4_MISO
    PE11     ------> SPI4_NSS
    PE14     ------> SPI4_MOSI
    PE12     ------> SPI4_SCK 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;//GPIO_MODE_AF_OD;//GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI4;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN SPI4_MspInit 1 */

  /* USER CODE END SPI4_MspInit 1 */
  }

}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if(hspi->Instance==SPI1)
  {
  /* USER CODE BEGIN SPI1_MspDeInit 0 */

  /* USER CODE END SPI1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();
  
    /**SPI1 GPIO Configuration    
    PB5     ------> SPI1_MOSI
    PB4 (NJTRST)     ------> SPI1_MISO
    PB3 (JTDO/TRACESWO)     ------> SPI1_SCK
    PA15 (JTDI)     ------> SPI1_NSS 
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_15);

  /* USER CODE BEGIN SPI1_MspDeInit 1 */

  /* USER CODE END SPI1_MspDeInit 1 */
  }
  else if(hspi->Instance==SPI4)
  {
  /* USER CODE BEGIN SPI4_MspDeInit 0 */

  /* USER CODE END SPI4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SPI4_CLK_DISABLE();
  
    /**SPI4 GPIO Configuration    
    PE13     ------> SPI4_MISO
    PE11     ------> SPI4_NSS
    PE14     ------> SPI4_MOSI
    PE12     ------> SPI4_SCK 
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_13|GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_12);

  /* USER CODE BEGIN SPI4_MspDeInit 1 */

  /* USER CODE END SPI4_MspDeInit 1 */
  }

}
#if 0
void HAL_QSPI_MspInit(QSPI_HandleTypeDef *hqspi)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* 使能QPSI时钟  */
	QSPI_CLK_ENABLE();
	
	/* 复位时钟接口 */
	QSPI_FORCE_RESET();
	QSPI_RELEASE_RESET();
	
	/* 使能GPIO时钟 */
	QSPI_CS_GPIO_CLK_ENABLE();
	QSPI_CLK_GPIO_CLK_ENABLE();
	QSPI_BK1_D0_GPIO_CLK_ENABLE();
	QSPI_BK1_D1_GPIO_CLK_ENABLE();
	QSPI_BK1_D2_GPIO_CLK_ENABLE();
	QSPI_BK1_D3_GPIO_CLK_ENABLE(); 

	/* QSPI CS GPIO 引脚配置 */
	GPIO_InitStruct.Pin       = QSPI_CS_PIN;
	GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull      = GPIO_PULLUP;
	GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_CS_GPIO_PORT, &GPIO_InitStruct);
	
	/* QSPI CLK GPIO 引脚配置 */
	GPIO_InitStruct.Pin       = QSPI_CLK_PIN;
	GPIO_InitStruct.Pull      = GPIO_NOPULL;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_CLK_GPIO_PORT, &GPIO_InitStruct);

	/* QSPI D0 GPIO pin 引脚配置 */
	GPIO_InitStruct.Pin       = QSPI_BK1_D0_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_BK1_D0_GPIO_PORT, &GPIO_InitStruct);

	/* QSPI D1 GPIO 引脚配置 */
	GPIO_InitStruct.Pin       = QSPI_BK1_D1_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_BK1_D1_GPIO_PORT, &GPIO_InitStruct);

	/* QSPI D2 GPIO 引脚配置 */
	GPIO_InitStruct.Pin       = QSPI_BK1_D2_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_BK1_D2_GPIO_PORT, &GPIO_InitStruct);

	/* QSPI D3 GPIO 引脚配置 */
	GPIO_InitStruct.Pin       = QSPI_BK1_D3_PIN;
	GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
	HAL_GPIO_Init(QSPI_BK1_D3_GPIO_PORT, &GPIO_InitStruct);

	/* 使能QSPI中断 */
	HAL_NVIC_SetPriority(QUADSPI_IRQn, 0x0F, 0);
	HAL_NVIC_EnableIRQ(QUADSPI_IRQn);
	
	/* 配置MDMA时钟 */
	QSPI_MDMA_CLK_ENABLE();

	hmdma.Instance = MDMA_Channel1;                         /* 使用MDMA的通道1 */
	hmdma.Init.Request = MDMA_REQUEST_QUADSPI_FIFO_TH;      /* QSPI的FIFO阀值触发中断 */
	hmdma.Init.TransferTriggerMode = MDMA_BUFFER_TRANSFER;  /* 使用MDMA的buffer传输 */
	hmdma.Init.Priority = MDMA_PRIORITY_HIGH;               /* 优先级高 */
	hmdma.Init.Endianness = MDMA_LITTLE_ENDIANNESS_PRESERVE;/* 小端格式 */
	hmdma.Init.SourceInc = MDMA_SRC_INC_BYTE;               /* 源地址字节递增 */
	hmdma.Init.DestinationInc = MDMA_DEST_INC_DISABLE;      /* 目的地址无效自增 */
	hmdma.Init.SourceDataSize = MDMA_SRC_DATASIZE_BYTE;     /* 源地址数据宽度字节 */
	hmdma.Init.DestDataSize = MDMA_DEST_DATASIZE_BYTE;      /* 目的地址数据宽度字节 */
	hmdma.Init.DataAlignment = MDMA_DATAALIGN_PACKENABLE;   /* 小端，右对齐 */
	hmdma.Init.BufferTransferLength = 128;                  /* 每次传输128个字节 */
	hmdma.Init.SourceBurst = MDMA_SOURCE_BURST_SINGLE;      /* 源数据单次传输 */
	hmdma.Init.DestBurst = MDMA_DEST_BURST_SINGLE;          /* 目的数据单次传输 */
	
	hmdma.Init.SourceBlockAddressOffset = 0;                /* 用于block传输，buffer传输用不到 */
	hmdma.Init.DestBlockAddressOffset = 0;                  /* 用于block传输，buffer传输用不到 */

	/* 关联MDMA句柄到QSPI句柄里面  */
	__HAL_LINKDMA(hqspi, hmdma, hmdma);

	/* 先复位，然后配置MDMA */
	HAL_MDMA_DeInit(&hmdma);  
	HAL_MDMA_Init(&hmdma);

	/* 使能MDMA中断，并配置优先级 */
	HAL_NVIC_SetPriority(MDMA_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(MDMA_IRQn);
}
#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
