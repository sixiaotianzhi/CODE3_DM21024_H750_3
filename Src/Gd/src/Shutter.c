#include "Shutter.h"
#include "main.h"
#include "Varible.h"

void Shutter_Off(void)
{
	HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(SHUTTER_INTA_GPIO_Port, SHUTTER_INTA_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SHUTTER_INTB_GPIO_Port, SHUTTER_INTB_Pin, GPIO_PIN_RESET);
}

void Shutter_Stop(void)
{
	HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(SHUTTER_INTA_GPIO_Port, SHUTTER_INTA_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SHUTTER_INTB_GPIO_Port, SHUTTER_INTB_Pin, GPIO_PIN_RESET);
	
}

void Shutter_On(void)
{
	HAL_GPIO_WritePin(DRV8837_SLEEP_GPIO_Port, DRV8837_SLEEP_Pin, GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(SHUTTER_INTA_GPIO_Port, SHUTTER_INTA_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SHUTTER_INTB_GPIO_Port, SHUTTER_INTB_Pin, GPIO_PIN_SET);
}

void DealyMs(unsigned int ms)
{
	HAL_Delay(ms);	
}

short GetavgShutter(void)
{
	return Loop_Data120.us_avg_shutter;
}
