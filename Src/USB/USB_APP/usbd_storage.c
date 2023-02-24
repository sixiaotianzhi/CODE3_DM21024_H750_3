/**
  ******************************************************************************
  * @file    USB_Device/MSC_Standalone/Src/usbd_storage.c
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    29-December-2017
  * @brief   Memory management layer
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include "usbd_storage.h"
#include "sdmmc_sdcard.h"
#include "w25qxx.h"
#include "main.h"
//===#include "nand.h"    
//===#include "ftl.h"  

/* Private typedef ----------------------------------------------------------- */
/* Private define ------------------------------------------------------------ */
#define STORAGE_LUN_NBR                  1
#define STORAGE_BLK_NBR                  0x10000
#define STORAGE_BLK_SIZ                  0x200

////////////////////////////自己定义的一个标记USB状态的寄存器///////////////////
//bit0:表示电脑正在向SD卡写入数据
//bit1:表示电脑正从SD卡读出数据
//bit2:SD卡写数据错误标志位
//bit3:SD卡读数据错误标志位
//bit4:1,表示电脑有轮询操作(表明连接还保持着)
vu8 USB_STATUS_REG=0;
////////////////////////////////////////////////////////////////////////////////
#if 1

#if USB_FLASH_EN
int8_t STORAGE_Inquirydata[] = {  /* 36 */
/* LUN 0 */
	0x00,		
	0x80,		
	0x02,		
	0x02,
	(STANDARD_INQUIRY_DATA_LEN - 5),
	0x00,
	0x00,	
	0x00,
    /* Vendor Identification */
    'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',//9字节
    /* Product Identification */
    'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',//15字节
    ' ','D', 'i', 's', 'k', ' ',
    /* Product Revision Level */	
    '1', '.', '0', ' ',							//4字节	
};

/* Private function prototypes ----------------------------------------------- */
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata,
};

int8_t STORAGE_Init(uint8_t lun)
{
	u8 res=0;
	switch(lun)
	{
		case 0://SPI FLASH
			W25QXX_Init();
			break;
        default:
			res=1;
            break; 
	} 
	return res; 
}

int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size)
{   
    switch(lun)
	{
		case 0://SPI FLASH			  
            *block_size=512;  
			*block_num=1024*1024*4/512;	//SPI FLASH的前面25M字节,文件系统用                     
			break;
        default:
			// res=1;
            break;
	}  	
	return 0; 
}

int8_t STORAGE_IsReady(uint8_t lun)
{

    int8_t ret = 0;   
    USB_STATUS_REG|=0X10;//标记轮询
    return ret;
}

int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
    return 0;
}

int8_t STORAGE_Read(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len)
{
    int8_t res=0;
	USB_STATUS_REG|=0X02;//标记正在读数据
	switch(lun)
	{
		case 0://SPI FLASH
			W25QXX_Read(buf,blk_addr*512,blk_len*512);
			break;
        default:
			res=1;
            break; 
	} 
	if(res)
	{
		USB_STATUS_REG|=0X08;//读错误! 
	} 
	return res;
}

int8_t STORAGE_Write(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len)
{
    int8_t res=0;
	USB_STATUS_REG|=0X01;//标记正在写数据
	switch(lun)
	{
		case 0://SPI FLASH
			W25QXX_Write(buf,blk_addr*512,blk_len*512);
			break;
        default:
			res=1;
            break;
	}  
	if(res)
	{
		USB_STATUS_REG|=0X04;//写错误!	 
	}
	return res;
}

int8_t STORAGE_GetMaxLun(void)
{    
    return STORAGE_LUN_NBR-1;
}
#else
int8_t STORAGE_Inquirydata[] = {  /* 36 */
/* LUN 0 */
    0x00,
	0x80,		
	0x02,		
	0x02,
	(STANDARD_INQUIRY_DATA_LEN - 5),
	0x00,
	0x00,	
	0x00,
	/* Vendor Identification */
	'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K',' ',	//9字节
	/* Product Identification */				
	'S', 'D', ' ', 'F', 'l', 'a', 's', 'h', ' ',//15字节
	'D', 'i', 's', 'k', ' ', ' ',  
    /* Product Revision Level */
	'1', '.', '0' ,' ',                      	//4字节	
};

/* Private function prototypes ----------------------------------------------- */
int8_t STORAGE_Init(uint8_t lun);
int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size);
int8_t STORAGE_IsReady(uint8_t lun);
int8_t STORAGE_IsWriteProtected(uint8_t lun);
int8_t STORAGE_Read(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len);
int8_t STORAGE_Write(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len);
int8_t STORAGE_GetMaxLun(void);

USBD_StorageTypeDef USBD_DISK_fops = {
  STORAGE_Init,
  STORAGE_GetCapacity,
  STORAGE_IsReady,
  STORAGE_IsWriteProtected,
  STORAGE_Read,
  STORAGE_Write,
  STORAGE_GetMaxLun,
  STORAGE_Inquirydata,
};

int8_t STORAGE_Init(uint8_t lun)
{
	u8 res=0;
	switch(lun)
	{
		case 0://SD卡
			res=SD_Init();
			break;
        default:
			res=1;
            break; 
	} 
	return res; 
}

int8_t STORAGE_GetCapacity(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size)
{
    switch(lun)
	{
		case 0://SD卡            
            *block_num = SDCardInfo.CardCapacity - 1;
            *block_size = SDCardInfo.CardBlockSize;
			break;
        default:
			// res=1;
            break; 
	}  	
	return 0; 
}

int8_t STORAGE_IsReady(uint8_t lun)
{

    int8_t ret = 0;   
    USB_STATUS_REG|=0X10;//标记轮询
    return ret;
}

int8_t STORAGE_IsWriteProtected(uint8_t lun)
{
    return 0;
}

int8_t STORAGE_Read(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len)
{
    int8_t res=0;
	USB_STATUS_REG|=0X02;//标记正在读数据
	switch(lun)
	{		
		case 0://SD卡
			res=SD_ReadDisk(buf,blk_addr,blk_len);
			break;
        default:
			res=1;
            break; 
	} 
	if(res)
	{
		USB_STATUS_REG|=0X08;//读错误! 
	} 
	return res;
}

int8_t STORAGE_Write(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len)
{
    int8_t res=0;
	USB_STATUS_REG|=0X01;//标记正在写数据
	switch(lun)
	{		
		case 0://SD卡
			res=SD_WriteDisk(buf,blk_addr,blk_len);
			break;
        default:
			res=1;
            break; 
	}  
	if(res)
	{
		USB_STATUS_REG|=0X04;//写错误!	 
	}
	return res;
}

int8_t STORAGE_GetMaxLun(void)
{
    return STORAGE_LUN_NBR-1;
}
#endif

#endif


//===LOAD PARAM MODE
#if 1
int8_t STORAGE_Inquirydata2[] = {  /* 36 */
/* LUN 0 */
	0x00,		
	0x80,		
	0x02,		
	0x02,
	(STANDARD_INQUIRY_DATA_LEN - 5),
	0x00,
	0x00,	
	0x00,
    /* Vendor Identification */
    'A', 'L', 'I', 'E', 'N', 'T', 'E', 'K', ' ',//9字节
    /* Product Identification */
    'S', 'P', 'I', ' ', 'F', 'l', 'a', 's', 'h',//15字节
    ' ','D', 'i', 's', 'k', ' ',
    /* Product Revision Level */	
    '1', '.', '0', ' ',							//4字节	
};

/* Private function prototypes ----------------------------------------------- */
int8_t STORAGE_Init2(uint8_t lun);
int8_t STORAGE_GetCapacity2(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size);
int8_t STORAGE_IsReady2(uint8_t lun);
int8_t STORAGE_IsWriteProtected2(uint8_t lun);
int8_t STORAGE_Read2(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len);
int8_t STORAGE_Write2(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len);
int8_t STORAGE_GetMaxLun2(void);

USBD_StorageTypeDef USBD_DISK_fops2 = {
  STORAGE_Init2,
  STORAGE_GetCapacity2,
  STORAGE_IsReady2,
  STORAGE_IsWriteProtected2,
  STORAGE_Read2,
  STORAGE_Write2,
  STORAGE_GetMaxLun2,
  STORAGE_Inquirydata2,
};

int8_t STORAGE_Init2(uint8_t lun)
{
	u8 res=0;
	switch(lun)
	{
		case 0://SPI FLASH
			W25QXX_Init();
			break;
        default:
			res=1;
            break; 
	} 
	return res; 
}

int8_t STORAGE_GetCapacity2(uint8_t lun, uint32_t * block_num,
                           uint16_t * block_size)
{   
    switch(lun)
	{
		case 0://SPI FLASH			  
            *block_size=512;  
			*block_num=1024*1024*4/512;	//SPI FLASH的前面25M字节,文件系统用                     
			break;
        default:
			// res=1;
            break;
	}  	
	return 0; 
}

int8_t STORAGE_IsReady2(uint8_t lun)
{

    int8_t ret = 0;   
    USB_STATUS_REG|=0X10;//标记轮询
    return ret;
}

int8_t STORAGE_IsWriteProtected2(uint8_t lun)
{
    return 0;
}

int8_t STORAGE_Read2(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                    uint16_t blk_len)
{
    int8_t res=0;
	USB_STATUS_REG|=0X02;//标记正在读数据
	switch(lun)
	{
		case 0://SPI FLASH
			W25QXX_Read(buf,blk_addr*512,blk_len*512);
			break;
        default:
			res=1;
            break; 
	} 
	if(res)
	{
		USB_STATUS_REG|=0X08;//读错误! 
	} 
	return res;
}

int8_t STORAGE_Write2(uint8_t lun, uint8_t * buf, uint32_t blk_addr,
                     uint16_t blk_len)
{
    int8_t res=0;
	USB_STATUS_REG|=0X01;//标记正在写数据
	switch(lun)
	{
		case 0://SPI FLASH
			W25QXX_Write(buf,blk_addr*512,blk_len*512);
			break;
        default:
			res=1;
            break;
	}  
	if(res)
	{
		USB_STATUS_REG|=0X04;//写错误!	 
	}
	return res;
}

int8_t STORAGE_GetMaxLun2(void)
{    
    return STORAGE_LUN_NBR-1;
}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
