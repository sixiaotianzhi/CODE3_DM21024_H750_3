
#include "TempProcess.h"
#include "W25QXX.h"
#include <stdarg.h>
#include<string.h>//===memcpy
#include<main.h>


#define LOW_Degree_ADDR 360//340           
#define HIGH_Degree_ADDR 400//386  

uint32_t gReadAddr;
uint32_t gReadPosition;
uint16_t gNumByteToRead;
uint32_t gDegree;
unsigned char read_cnt = 0;
unsigned char buf_temp[4096]={0};

extern u8 UserReadParam(u8 *data,u32 ReadAddr,u16 NumByteToRead,u32 Degree);

int BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead,uint32_t Degree)
{
	//uint8_t res;
	int i;
	// uint16_t flash_page;
	uint32_t total_coped = 0;

	// if(Degree == 1)
	// {
	// 	flash_page = LOW_Degree_ADDR;
	// }
	// else if(Degree == 2)
	// {
	// 	flash_page = HIGH_Degree_ADDR;
	// }

	gReadAddr = ReadAddr / 0x1000;
	gReadPosition = ReadAddr % 0x1000;
	gNumByteToRead = NumByteToRead + gReadPosition;
	

	if(gNumByteToRead <= 0x1000)
	{
		// SPI_FLASH_BufferRead(buf_temp, (flash_page + gReadAddr)*0x1000, 0x1000);
        UserReadParam(buf_temp, (0 + gReadAddr)*0x1000, 0x1000, Degree);
		memcpy(pBuffer, buf_temp + gReadPosition, NumByteToRead);
	}
	else
	{
		read_cnt = gNumByteToRead / 0x1000;
		for(i = 0; i <= read_cnt; i++)
		{
			// SPI_FLASH_BufferRead(buf_temp, (flash_page + gReadAddr + i)*0x1000, 0x1000);
            UserReadParam(buf_temp, (0 + gReadAddr + i)*0x1000, 0x1000, Degree);
			if(!i)
			{
				memcpy(pBuffer, buf_temp + gReadPosition, 0x1000 - gReadPosition);
				total_coped = 0x1000 - gReadPosition;
			}
			else if(i < read_cnt)
			{
				memcpy(pBuffer + total_coped, buf_temp, 0x1000);
				total_coped += 0x1000;
			}
			else if(i == read_cnt)
			{
				memcpy(pBuffer + total_coped, buf_temp, NumByteToRead - total_coped);
			}			

		}
	}
	
	return 0;
}


// int BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead,uint32_t Degree)
// {
// 	//uint8_t res;
// 	int i;
// 	uint16_t flash_page;
// 	uint32_t total_coped = 0;

// 	if(Degree == 1)
// 	{
// 		flash_page = LOW_Degree_ADDR;
// 	}
// 	else if(Degree == 2)
// 	{
// 		flash_page = HIGH_Degree_ADDR;
// 	}

// 	gReadAddr = ReadAddr / 0x1000;
// 	gReadPosition = ReadAddr % 0x1000;
// 	gNumByteToRead = NumByteToRead + gReadPosition;
	

// 	if(gNumByteToRead <= 0x1000)
// 	{
// 		SPI_FLASH_BufferRead(buf_temp, (flash_page + gReadAddr)*0x1000, 0x1000);
//         // UserReadParam(buf_temp, (0 + gReadAddr)*0x1000, 0x1000, Degree);
// 		memcpy(pBuffer, buf_temp + gReadPosition, NumByteToRead);
// 	}
// 	else
// 	{
// 		read_cnt = gNumByteToRead / 0x1000;
// 		for(i = 0; i <= read_cnt; i++)
// 		{
// 			SPI_FLASH_BufferRead(buf_temp, (flash_page + gReadAddr + i)*0x1000, 0x1000);
//             // UserReadParam(buf_temp, (0 + gReadAddr + i)*0x1000, 0x1000, Degree);
// 			if(!i)
// 			{
// 				memcpy(pBuffer, buf_temp + gReadPosition, 0x1000 - gReadPosition);
// 				total_coped = 0x1000 - gReadPosition;
// 			}
// 			else if(i < read_cnt)
// 			{
// 				memcpy(pBuffer + total_coped, buf_temp, 0x1000);
// 				total_coped += 0x1000;
// 			}
// 			else if(i == read_cnt)
// 			{
// 				memcpy(pBuffer + total_coped, buf_temp, NumByteToRead - total_coped);
// 			}			

// 		}
// 	}
	
// 	return 0;
// }

//���Դ�ӡ����ʵ��
void DebugPrint(char*formt,...)
{

}

#if 0
int BufferRead(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t NumByteToRead,uint32_t Degree)
{
	uint8_t res;
	#if 1
	gReadAddr = ReadAddr;
	gNumByteToRead = NumByteToRead;
	gDegree = Degree;
	if(Degree==1)
	{
		SPI_FLASH_BufferRead(pBuffer, LOW_Degree_ADDR+ReadAddr, NumByteToRead);
		memcpy(buf_test, pBuffer, 4200);
		//res=f_open(&testFile,"0:low.raw",FA_READ);
		//BSP_W25Qx_Read_Sector(g_update_pageID, pBuffer, NumByteToRead);
	}
	else if(Degree==2)
	{
		SPI_FLASH_BufferRead(pBuffer,  HIGH_Degree_ADDR+ReadAddr,  NumByteToRead);
		//res=f_open(&testFile,"0:high.raw",FA_READ);
		//BSP_W25Qx_Read_Sector(g_update_pageID, pBuffer, NumByteToRead);
	}
	#endif
}
#endif

