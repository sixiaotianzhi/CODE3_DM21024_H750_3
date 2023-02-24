#include "SPI.h"
#include "main.h"

extern SPI_HandleTypeDef hspi2;
uint8_t SPI_Read_Byte(uint8_t pData)
{
	uint8_t temp;

	HAL_SPI_Receive(&hspi2, &temp, 1,100);	
	return temp;
}

void SPI_SendByte(uint8_t pData)
{
	HAL_SPI_Transmit(&hspi2,&pData,1,100);
}
//===
void SPI_FLASH_CS_LOW(void)
{
	//hardware nss
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10,GPIO_PIN_RESET);//TODO
}	
void SPI_FLASH_CS_HIGH(void)
{
	//hardware nss
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10,GPIO_PIN_SET);//TODO
}	
