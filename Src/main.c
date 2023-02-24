/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "MeasureInterface.h"
#include "Sensor.h"
#include "Varible.h"
#include "W25QXX.h"
#include "qspi.h"
#include "usart.h"
#include "key.h"
	
// #include "mpu.h"
// #include "malloc.h"
// #include "sdmmc_sdcard.h"  
// #include "ff.h"  
// #include "exfuns.h" 

//===
#include "stdio.h" 
#include "sdmmc_sdcard.h" 
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h"


USBD_HandleTypeDef USBD_Device;
extern PCD_HandleTypeDef hpcd;
extern vu8 USB_STATUS_REG;			//USB状态
extern vu8 bDeviceState;			//USB连接 情况
//+++
DCMI_HandleTypeDef hdcmi;
DMA_HandleTypeDef hdma_dcmi;
SPI_HandleTypeDef hspi2;
I2C_HandleTypeDef hi2c2;
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;
DMA_HandleTypeDef hdma_spi1_rx;
SPI_HandleTypeDef hspi4;
MDMA_HandleTypeDef hmdma; 

osThreadId defaultTaskHandle;
osThreadId irCameraTaskHandle;
osThreadId lcdTaskHandle;

// QueueHandle_t MutexSemaphore;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_DCMI_Init(void);
//static 
void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
// static void MX_I2C2_Init(void);
// static void MX_SPI4_Init(void);

void StartDefaultTask(void const * argument);
void StartIRCameraTask(void const * argument);
void StartIRCameraTask_LoadParam(void const * argument);
void StartLcdCameraTask(void const * argument);

extern void SPI_DMAXferCplt(DMA_HandleTypeDef *hdma_spi1_tx);
extern void W25QXX_Init(void);

void CPU_CACHE_Enable(void)
{
	SCB_EnableDCache();
	SCB_EnableICache();
}

void MPU_Config( void )
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    HAL_MPU_Disable();

    #if 1
    /* Configure the MPU attributes as WT for SRAM */
    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x24000000;
    MPU_InitStruct.Size             = MPU_REGION_SIZE_512KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER0;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    #endif
		
    #if 1
    /* Configure the MPU attributes as WT for SRAM */
    MPU_InitStruct.Enable           = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress      = 0x30000000;
    MPU_InitStruct.Size             = MPU_REGION_SIZE_256KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable     = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable      = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable      = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number           = MPU_REGION_NUMBER2;
    MPU_InitStruct.TypeExtField     = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec      = MPU_INSTRUCTION_ACCESS_DISABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    #endif

    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

void UserInitUsb(char mode)
{
#if USB_EN
    USBD_Init(&USBD_Device,&MSC_Desc,0);                        //初始化USB
	USBD_RegisterClass(&USBD_Device,USBD_MSC_CLASS);            //添加类
	if(mode==0)
        USBD_MSC_RegisterStorage(&USBD_Device,&USBD_DISK_fops);     //为MSC类添加回调函数
    else 
        USBD_MSC_RegisterStorage(&USBD_Device,&USBD_DISK_fops2);     //为MSC类添加回调函数
	USBD_Start(&USBD_Device);                                   //开启USB
	HAL_PWREx_EnableUSBVoltageDetector(); 
    // HAL_Delay(3000);
#endif     
}

#if 0
//时钟设置函数
//Fvco=Fs*(plln/pllm);
//Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp));
//Fq=Fvco/pllq=Fs*(plln/(pllm*pllq));

//Fvco:VCO频率
//Fsys:系统时钟频率,也是PLL1的p分频输出时钟频率
//Fq:PLL1的q分频输出时钟频率
//Fs:PLL输入时钟频率,可以是HSI,CSI,HSE等. 

//plln:PLL1倍频系数(PLL倍频),取值范围:4~512.
//pllm:PLL1预分频系数(进PLL之前的分频),取值范围:2~63.
//pllp:PLL1的p分频系数(PLL之后的分频),分频后作为系统时钟,取值范围:2~128.(且必须是2的倍数)
//pllq:PLL1的q分频系数(PLL之后的分频),取值范围:1~128.

//CPU频率(rcc_c_ck)=sys_d1cpre_ck=400Mhz 
//rcc_aclk=rcc_hclk3=200Mhz
//AHB1/2/3/4(rcc_hclk1/2/3/4)=200Mhz  
//APB1/2/3/4(rcc_pclk1/2/3/4)=100Mhz  
//FMC时钟频率=pll2_r_ck=((25/25)*512/2)=256Mhz

//外部晶振为25M的时候,推荐值:plln=160,pllm=5,pllp=2,pllq=2.
//得到:Fvco=25*(160/5)=800Mhz
//     Fsys=800/2=400Mhz
//     Fq=800/2=400Mhz
//返回值:0,成功;1,失败。
u8 Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
	HAL_StatusTypeDef ret=HAL_OK;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;
  
	MODIFY_REG(PWR->CR3,PWR_CR3_SCUEN, 0);
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while ((PWR->D3CR & (PWR_D3CR_VOSRDY)) != PWR_D3CR_VOSRDY) {}
  
	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState=RCC_HSE_ON;
	RCC_OscInitStruct.HSIState=RCC_HSI_OFF;
	RCC_OscInitStruct.CSIState=RCC_CSI_OFF;
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;

	RCC_OscInitStruct.PLL.PLLN=plln;
	RCC_OscInitStruct.PLL.PLLM=pllm;
	RCC_OscInitStruct.PLL.PLLP=pllp;
	RCC_OscInitStruct.PLL.PLLQ=pllq;

	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
	ret=HAL_RCC_OscConfig(&RCC_OscInitStruct);
	if(ret!=HAL_OK) return 1;
		
	// QSPI_Enable_Memmapmode();				//QSPI内存映射模式,需要在时钟初始化之前开启,否则会有各种问题
  
	RCC_ClkInitStruct.ClockType=(RCC_CLOCKTYPE_SYSCLK|\
                                RCC_CLOCKTYPE_HCLK |\
                                RCC_CLOCKTYPE_D1PCLK1 |\
                                RCC_CLOCKTYPE_PCLK1 |\
                                RCC_CLOCKTYPE_PCLK2 |\
                                RCC_CLOCKTYPE_D3PCLK1);

	RCC_ClkInitStruct.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider=RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider=RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB1CLKDivider=RCC_APB1_DIV2; 
	RCC_ClkInitStruct.APB2CLKDivider=RCC_APB2_DIV2; 
	RCC_ClkInitStruct.APB3CLKDivider=RCC_APB3_DIV2;  
	RCC_ClkInitStruct.APB4CLKDivider=RCC_APB4_DIV4; 
	ret=HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);
	if(ret!=HAL_OK) return 1;

	__HAL_RCC_CSI_ENABLE() ;
	__HAL_RCC_SYSCFG_CLK_ENABLE() ;  
	// HAL_EnableCompensationCell();
	
	return 0;
}
#endif 

int main(void)
{
	CPU_CACHE_Enable( );
    HAL_Init();
	MPU_Config( );
    SystemClock_Config();
    // Stm32_Clock_Init(192,5,2,4);

    MX_GPIO_Init();
    MX_DMA_Init();
    MX_DCMI_Init();
    MX_SPI2_Init();    
    MX_SPI1_Init();
    //MX_I2C2_Init();
    //MX_SPI4_Init(); 
   
	uart_init(115200);
    KEY_Init();	

	MemoryAlloc(); //sensor��������ռ� ��֤�ѿռ�66KByte����
	ALG_ALLOC_AssignMemory(AlgMem, 132*1024);
	AlgInit(AlgPara120);
	MEASURE_ALLOC_AssignMemory(MeasureMem, 1024*42); //Measure�ڴ����
	InitMeasurePara(&Para120);
	//===SetMeasureDegree(1);//===1>>-20~150 2>>100-400

#if RTOS_EN        
    if((KEY1==0)&&(KEY2==0)&&(USB_IN==1))
    {
        osThreadDef(irCameraTask, StartIRCameraTask_LoadParam, osPriorityHigh, 0, 128*2);
        irCameraTaskHandle = osThreadCreate(osThread(irCameraTask), NULL);
    }
    else
    {
        osThreadDef(irCameraTask, StartIRCameraTask, osPriorityHigh, 0, 128*2);
        irCameraTaskHandle = osThreadCreate(osThread(irCameraTask), NULL);
    }
    
    osKernelStart();
  
    while (1)
    {
        osDelay(1);
    }
#else
    extern void StartIRCameraTask2(void);
    StartIRCameraTask2();
    while(1)
    {
        ;
    }
#endif

}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

    // RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;//===Fs=25M ????PLLM PLLN 
    // RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    // RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    // RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    //===1229 使能HSI48MHz给USB使用
	RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState=RCC_HSE_ON;
	RCC_OscInitStruct.HSI48State=RCC_HSI48_ON;
	RCC_OscInitStruct.HSIState=RCC_HSI_OFF;
	RCC_OscInitStruct.CSIState=RCC_CSI_OFF;
	RCC_OscInitStruct.PLL.PLLState=RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource=RCC_PLLSOURCE_HSE;

    //===192>>480M 160>>400M
    RCC_OscInitStruct.PLL.PLLM = 5;
    RCC_OscInitStruct.PLL.PLLN = 192;//===Fvco=Fs*(plln/pllm)>>25M/5*192=960M
    //+++
    RCC_OscInitStruct.PLL.PLLP = 2;//===Fp=Fsys=Fvco/pllp=Fs*(plln/(pllm*pllp))>>960M/2=480M
    RCC_OscInitStruct.PLL.PLLQ = 4;//===Fq=Fvco/pllq=Fs*(plln/(pllm*pllq))>>960M/4=240M
    RCC_OscInitStruct.PLL.PLLR = 2;//===Fr=>>960M/2=480M
    RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
    RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
    RCC_OscInitStruct.PLL.PLLFRACN = 0;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
    Error_Handler();
    }
	
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;//===Fsys=480M
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;//===AHB1/2/3/4(rcc_hclk1/2/3/4)=Fsys/2>>480M/2=240M
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;//===APB1/2/3/4(rcc_pclk1/2/3/4)=Fsys/2>>480M/2=240M
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
    Error_Handler();
    }
  
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI4|RCC_PERIPHCLK_SPI1
                                |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_USB;
    //===SPI 180M                              
    PeriphClkInitStruct.PLL3.PLL3M = 5;//===25;
    PeriphClkInitStruct.PLL3.PLL3N = 72;//===192;//===Fvco=Fs*(plln/pllm)>>25M/25*192=192M
    //+++
    PeriphClkInitStruct.PLL3.PLL3P = 2;//===Fp=>>192M/2=96M
    PeriphClkInitStruct.PLL3.PLL3Q = 4;//===Fq=>>192M/4=48M
    PeriphClkInitStruct.PLL3.PLL3R = 2;//===Fr=>>192M/2=96M
    PeriphClkInitStruct.PLL3.PLL3RGE = RCC_PLL3VCIRANGE_0;
    PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
    PeriphClkInitStruct.PLL3.PLL3FRACN = 0;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL3;//===RCC_SPI123CLKSOURCE_PLL;
    PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;//===RCC Domain 2(??2????)
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL3;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
    Error_Handler();
    }

    //===HAL_PWREx_EnableUSBVoltageDetector();
}

static void MX_DCMI_Init(void)
{
    hdcmi.Instance = DCMI;
    hdcmi.Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
    hdcmi.Init.PCKPolarity = DCMI_PCKPOLARITY_FALLING;
    hdcmi.Init.VSPolarity = DCMI_VSPOLARITY_LOW;
    hdcmi.Init.HSPolarity = DCMI_HSPOLARITY_LOW;
    hdcmi.Init.CaptureRate = DCMI_CR_ALL_FRAME;
    hdcmi.Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
    hdcmi.Init.JPEGMode = DCMI_JPEG_DISABLE;
    hdcmi.Init.ByteSelectMode = DCMI_BSM_ALL;
    hdcmi.Init.ByteSelectStart = DCMI_OEBS_ODD;
    hdcmi.Init.LineSelectMode = DCMI_LSM_ALL;
    hdcmi.Init.LineSelectStart = DCMI_OELS_ODD;
    if (HAL_DCMI_Init(&hdcmi) != HAL_OK)
    {
        Error_Handler();
    }

    HAL_NVIC_SetPriority(DCMI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DCMI_IRQn);
}

static void MX_SPI2_Init(void)
{
    hspi2.Instance = SPI2;
    hspi2.Init.Mode = SPI_MODE_MASTER;
    hspi2.Init.Direction = SPI_DIRECTION_2LINES;
    hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi2.Init.NSS = SPI_NSS_SOFT;
    hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi2.Init.CRCPolynomial = 0x0;
    hspi2.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    if (HAL_SPI_Init(&hspi2) != HAL_OK)
    {
    Error_Handler();
    }
}

void MX_SPI1_Init(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 0x0;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }

    // HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
    // HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

void MX_SPI1_Init2(void)
{
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_16BIT;//========
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;//========180M/2
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 0x0;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
    hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
    hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
    hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
    hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
    hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
    hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
    hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK)
    {
        Error_Handler();
    }

    // HAL_NVIC_SetPriority(SPI1_IRQn, 5, 0);
    // HAL_NVIC_EnableIRQ(SPI1_IRQn);
}

static void MX_DMA_Init(void) 
{
    __HAL_RCC_DMA1_CLK_ENABLE();

    hdma_dcmi.Instance = DMA1_Stream0;
    hdma_dcmi.Init.Request = DMA_REQUEST_DCMI;
    hdma_dcmi.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_dcmi.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_dcmi.Init.MemInc = DMA_MINC_ENABLE;
    hdma_dcmi.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_dcmi.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_dcmi.Init.Mode = DMA_CIRCULAR;
    hdma_dcmi.Init.Priority = DMA_PRIORITY_MEDIUM;
    hdma_dcmi.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_dcmi) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(&hdcmi,DMA_Handle,hdma_dcmi);

    HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

#if SPI1_DMA_EN
    // __HAL_RCC_DMA2_CLK_ENABLE();
    
    hdma_spi1_rx.Instance=DMA1_Stream2;                      
	hdma_spi1_rx.Init.Request=DMA_REQUEST_SPI1_RX;		
    hdma_spi1_rx.Init.Direction=DMA_PERIPH_TO_MEMORY;           
    hdma_spi1_rx.Init.PeriphInc=DMA_PINC_DISABLE;               
    hdma_spi1_rx.Init.MemInc=DMA_MINC_ENABLE;                  
    hdma_spi1_rx.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    
    hdma_spi1_rx.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       
    hdma_spi1_rx.Init.Mode=DMA_NORMAL;//===DMA_NORMAL;                            
    hdma_spi1_rx.Init.Priority=DMA_PRIORITY_HIGH;             
    hdma_spi1_rx.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    hdma_spi1_rx.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    hdma_spi1_rx.Init.MemBurst=DMA_MBURST_SINGLE;                
    hdma_spi1_rx.Init.PeriphBurst=DMA_PBURST_SINGLE;              
    if (HAL_DMA_Init(&hdma_spi1_rx) != HAL_OK)
    {
      Error_Handler();
    }
    hdma_spi1_tx.Instance=DMA1_Stream3;                           
	hdma_spi1_tx.Init.Request=DMA_REQUEST_SPI1_TX;				
    hdma_spi1_tx.Init.Direction=DMA_MEMORY_TO_PERIPH;           
    hdma_spi1_tx.Init.PeriphInc=DMA_PINC_DISABLE;                 
    hdma_spi1_tx.Init.MemInc=DMA_MINC_ENABLE;                     
    // hdma_spi1_tx.Init.PeriphDataAlignment=DMA_PDATAALIGN_WORD;//===DMA_PDATAALIGN_BYTE;    
    // hdma_spi1_tx.Init.MemDataAlignment=DMA_MDATAALIGN_WORD;//===DMA_MDATAALIGN_BYTE; 
    hdma_spi1_tx.Init.PeriphDataAlignment=DMA_PDATAALIGN_HALFWORD;//DMA_PDATAALIGN_HALFWORD;//===DMA_PDATAALIGN_BYTE;    
    hdma_spi1_tx.Init.MemDataAlignment=DMA_MDATAALIGN_HALFWORD;//DMA_MDATAALIGN_HALFWORD;//===DMA_MDATAALIGN_BYTE;             
    hdma_spi1_tx.Init.Mode=DMA_NORMAL;//===DMA_NORMAL;                           
    hdma_spi1_tx.Init.Priority=DMA_PRIORITY_HIGH;             
    hdma_spi1_tx.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    hdma_spi1_tx.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    hdma_spi1_tx.Init.MemBurst=DMA_MBURST_SINGLE;                 
    hdma_spi1_tx.Init.PeriphBurst=DMA_PBURST_SINGLE;              
    if (HAL_DMA_Init(&hdma_spi1_tx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(&hspi1,hdmarx,hdma_spi1_rx); 
    __HAL_LINKDMA(&hspi1,hdmatx,hdma_spi1_tx);  

    hdma_spi1_tx.XferCpltCallback=SPI_DMAXferCplt;

    HAL_NVIC_SetPriority( DMA1_Stream2_IRQn, 5, 0 );  
	HAL_NVIC_EnableIRQ( DMA1_Stream2_IRQn );
    HAL_NVIC_SetPriority( DMA1_Stream3_IRQn, 5, 0 );  
	HAL_NVIC_EnableIRQ( DMA1_Stream3_IRQn );    
#endif
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();
    //===SHUTTER 
    HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SHUTTER_INTA_GPIO_Port, SHUTTER_INTA_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(SHUTTER_INTB_GPIO_Port, SHUTTER_INTB_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = DRV8837_SLEEP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DRV8837_SLEEP_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SHUTTER_INTA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SHUTTER_INTA_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SHUTTER_INTB_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(SHUTTER_INTB_GPIO_Port, &GPIO_InitStruct);
    //===SENSOR
    HAL_GPIO_WritePin(GPIOB, CAL_DATAVALID_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, TCAL_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOE, SENSOR_RESET_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = CAL_DATAVALID_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = TCAL_EN_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SENSOR_RESET_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF0_MCO;//===
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    //===POWER 
    HAL_GPIO_WritePin(GPIOE, POWE_EN_Pin|P_EN_3_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, P_EN_1_Pin|P_EN_2_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = POWE_EN_Pin|P_EN_3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = P_EN_1_Pin|P_EN_2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    //===LCD    
    HAL_GPIO_WritePin(GPIOB, ST7789_RST_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD, ST7789_DC_PIN|ST7789_CS_PIN|ST7789_LCDBL_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin = ST7789_RST_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = ST7789_DC_PIN|ST7789_CS_PIN|ST7789_LCDBL_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

#if 0
    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : ADC_RDY_Pin */
    GPIO_InitStruct.Pin = ADC_RDY_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(ADC_RDY_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : LED1_Pin LED2_Pin */
    GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI2_IRQn);
#endif
}
#if RTOS_EN
__weak void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */ 
}

__weak void StartIRCameraTask(void const * argument)
{
  /* USER CODE BEGIN StartIRCameraTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartIRCameraTask */
}

__weak void StartIRCameraTask_LoadParam(void const * argument)
{
  /* USER CODE BEGIN StartIRCameraTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartIRCameraTask */
}

__weak void StartLCDCameraTask(void const * argument)
{
  /* USER CODE BEGIN StartIRCameraTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartIRCameraTask */
}
#endif
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

#if 0 //MX_I2C2_Init
static void MX_I2C2_Init(void)
{
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x307075B1;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK)
    {
    Error_Handler();
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
    {
    Error_Handler();
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
    {
    Error_Handler();
    }
}
#endif
#if 0 //MX_SPI4_Init
static void MX_SPI4_Init(void)
{
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 0x0;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif












