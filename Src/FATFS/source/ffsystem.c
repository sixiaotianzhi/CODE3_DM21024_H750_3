/*------------------------------------------------------------------------*/
/* Sample Code of OS Dependent Functions for FatFs                        */
/* (C)ChaN, 2017                                                          */
/*------------------------------------------------------------------------*/
#include "ff.h"
#include "sys.h"
#include "malloc.h"
#include "rtc.h"//===
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32H7开发板
//FATFS底层(ffsystem) 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2018/8/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 



//获得时间
//User defined function to give a current time to fatfs module      */
//31-25: Year(0-127 org.1980), 24-21: Month(1-12), 20-16: Day(1-31) */                                                                                                                                                                                                                                          
//15-11: Hour(0-23), 10-5: Minute(0-59), 4-0: Second(0-29 *2) */                                                                                                                                                                                                                                                
DWORD get_fattime (void)
{				 
    DWORD Value;

    u8 Year;
    u8 Month;
    u8 Date;
    u8 WeekDay;
    u8 Hour;
    u8 Minute;
    u8 Second;
    u8 ampm;

    RTC_Get_Time(&Hour,&Minute,&Second,&ampm); 
    RTC_Get_Date(&Year,&Month,&Date,&WeekDay);

    Value = ((Year + 20) << 25) | (Month << 21) | (Date << 16) |
             (Hour << 11) | (Minute << 5) | (Second >> 1);

    return Value;                                
}

//动态分配内存
void *ff_memalloc (UINT size)			
{
	return (void*)mymalloc(SRAMIN,size);
}

//释放内存
void ff_memfree (void* mf)		 
{
	myfree(SRAMIN,mf);
}











