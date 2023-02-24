#include "stm32h7xx_hal.h"
#include "cmsis_os.h"

#include "ir_def.h"
#include "main.h"
#include "Varible.h"

#include "key.h"
// #include "led.h"
// #include "lcd.h"

#include "st7789.h"
#include "icon.h"
#include "rtc.h"

#include "sdmmc_sdcard.h"  
#include "ff.h"  
#include "exfuns.h" 

// u8 palette_select_cnt=2;
// u8 temp_select_cnt=0;
// u8 temp_select_ok=0;
// u8 center_flag=1;
// u8 maxmin_flag=1;

// u8 unit_select_cnt=0;

// u8 param_select_cnt=0;
// u8 param_select_ok=0;
// u8 emiss_flag=0;
// u8 degree_flag=0;
// u8 emiss_select_cnt=95;
// u8 degree_select_cnt=0;

// u8 backlight_select_cnt=0;
// u8 language_select_cnt=0;

// u8 date_select_cnt=0;
// u8 date_select_ok=0;
// u8 data24h_flag=0;
// u8 data_com_flag=0;
// u8 year_flag=0;
// u8 month_flag=0;
// u8 day_flag=0;
// u8 hour_flag=0;
// u8 min_flag=0;
// u8 pm_flag=0;//===0>>AM/24H 1>>PM
// u8 data12h_select_cnt=0;//===0>>24H 1>>12H
// u8 year_select_cnt=22;
// u8 month_select_cnt=11;
// u8 day_select_cnt=22;
// u8 hour_select_cnt=12;
// u8 min_select_cnt=0;

// u8 autooff_select_cnt=0;

// u8 menu2_flag=0;

// extern u16 dst_rgb[ LCD_W*LCD_H ];
// extern uint8_t shutter_id;
// extern IRInfo IRInfo120;
// extern QueueHandle_t MutexSemaphore;
// extern uint8_t g_image_flag;
// extern osThreadId defaultTaskHandle;
// extern osThreadId irCameraTaskHandle;

// volatile u8 pwmcnt;
// void bl_pwm(void)
// {
//     // vPortRaiseBASEPRI(); 
//     pwmcnt++;
//     if(pwmcnt>20)
//         pwmcnt=1;

//     switch(backlight_select_cnt)
//     {
//         case 0:           
//             if(pwmcnt==1)
//                 ST7789_LCDBL_Set();
//             else 
//                 ST7789_LCDBL_Clr();
//             break;
//         case 1:
//             if(pwmcnt<11)
//                 ST7789_LCDBL_Set();
//             else 
//                 ST7789_LCDBL_Clr();
//             break;
//         case 2:
//             ST7789_LCDBL_Set();
//             break;
//     }
//     // vPortClearBASEPRIFromISR();
// }

// u8 getWeek(u8 y, u8 m, u8 d)
// {
// 	if(m==1||m==2) 
// 	{
// 		m+=12;
// 		y--;
// 	}
// 	return (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7+1;
// }
// u8 keyok=0;
// u8 keycnt=0;
// u8 keyback=0;

// u8 hour,min,sec,ampm;
// u8 year,month,date,week;
// u8 tbuf[40];
// u8 t=0;  
#if RTOS_EN
void StartLcdCameraTask(void const * argument)
{
	ST7789_Init();
    KEY_Init();

    // RTC_Init();		 				//初始化RTC
	// RTC_Set_WakeUp(4,0);			//配置WAKE UP中断,1秒钟中断一次

    ST7789_WriteString(0,30,"power on......",Font_11x18, YELLOW, RED);
    osDelay(1000);
    ST7789_Fill_Color(RED);
    ST7789_WriteString(0,30,"ready",Font_11x18, YELLOW, RED);

    // while(SD_Init())//检测不到SD卡
	// {  
	// 	ST7789_WriteString(ICON6_X0,ICON6_Y0,"SD Card Error!",Font_11x18, YELLOW, BLACK);
	// }
    
    // if(exfuns_init())				//为fatfs相关变量申请内存				 
	// {
    //     ST7789_WriteString(ICON6_X0,ICON7_Y0,"mem alloc Error!",Font_11x18, YELLOW, BLACK);
    // }
    // f_mount(fs[0],"0:",1);		//挂载SD卡
    // u32 total,free;
    // exf_getfree("0",&total,&free);
    // sprintf((char*)tbuf,"total:%d free:%d ",total,free);
    // ST7789_WriteString(ICON6_X0,ICON6_Y0,tbuf,Font_11x18, YELLOW, BLACK);
    // f_unmount("0:");

    for(;;)
    {
        osDelay(100);
    }
}   
#endif
#if 0
	for(;;)
	{
        if(keyok==0)
        {
#if 0 //时钟
            t++;
            if((t%100)==0)	//每100ms更新一次显示数据
            {
                t=0;

                ST7789_Fill_Color(RED);

                RTC_Get_Time(&hour,&min,&sec,&ampm);
                sprintf((char*)tbuf,"Time:%02d:%02d:%02d",hour,min,sec); 			
                ST7789_WriteString(ICON3_X0,ICON3_Y0,tbuf,Font_11x18, YELLOW, BLACK);	
                RTC_Get_Date(&year,&month,&date,&week);
                sprintf((char*)tbuf,"Date:20%02d-%02d-%02d",year,month,date); 
                ST7789_WriteString(ICON4_X0,ICON4_Y0,tbuf,Font_11x18, YELLOW, BLACK);	
                sprintf((char*)tbuf,"Week:%d",week); 
                ST7789_WriteString(ICON5_X0,ICON5_Y0,tbuf,Font_11x18, YELLOW, BLACK);

                if(data12h_select_cnt==1)
                {
                    if(pm_flag==0)
                        ST7789_WriteString(ICON6_X0,ICON6_Y0,"AM",Font_11x18, YELLOW, BLACK);
                    else  
                        ST7789_WriteString(ICON6_X0,ICON6_Y0,"PM",Font_11x18, YELLOW, BLACK);
                }                 
            } 
#endif

            if(g_image_flag)
            {                  
                ST7789_DrawImage2(0, 0, LCD_W, LCD_H, (uint16_t *)dst_rgb);                
                
				u16 temp1;
				u16 temp2;
				u16 temp3;
				u16 temp4;
				char temp_str[4]={"\0"};

                if(center_flag)
                {
                    temp1=(u16)(IRInfo120.CursorValue*10);
                    temp2=(u16)(temp1/100);
                    myitoa(temp2,&temp_str[0],10);
                    temp3=temp1%100/10;
                    myitoa(temp3,&temp_str[1],10);
                    temp_str[2]='.';
                    temp4=temp1%10;
                    myitoa(temp4,&temp_str[3],10);						
                    ST7789_WriteString(10,10,temp_str,Font_11x18, YELLOW, RED);

                    ST7789_DrawCircle(160,120,5,BLACK);
                    ST7789_DrawLine(160-5-5,120,160-5,120,BLACK);
                    ST7789_DrawLine(160+5,120,160+5+5,120,BLACK);
                    ST7789_DrawLine(160,120-5-5,160,120-5,BLACK);
                    ST7789_DrawLine(160,120+5,160,120+5+5,BLACK);	
                }
				if(maxmin_flag)
                {
                    temp1=(u16)(IRInfo120.MaxValue*10);
                    temp2=(u16)(temp1/100);
                    myitoa(temp2,&temp_str[0],10);
                    temp3=temp1%100/10;
                    myitoa(temp3,&temp_str[1],10);
                    temp_str[2]='.';
                    temp4=temp1%10;
                    myitoa(temp4,&temp_str[3],10);						
                    ST7789_WriteString(320-50,10,temp_str,Font_11x18, YELLOW, RED);

                    temp1=(u16)(IRInfo120.MinValue*10);
                    temp2=(u16)(temp1/100);
                    myitoa(temp2,&temp_str[0],10);
                    temp3=temp1%100/10;
                    myitoa(temp3,&temp_str[1],10);
                    temp_str[2]='.';
                    temp4=temp1%10;
                    myitoa(temp4,&temp_str[3],10);						
                    ST7789_WriteString(320-50,30,temp_str,Font_11x18, YELLOW, RED);

                    u16 tempX;
                    u16 tempY;
                    u16 tempMIN=20;
                    u16 tempMAX=320-20;
                    tempX=IRInfo120.PostXMin*320/120;
                    tempY=IRInfo120.PostYMin*320/120;
                    tempX=(tempX<tempMIN ? tempMIN : tempX);
                    tempX=(tempX>tempMAX ? tempMAX : tempX);
                    tempY=(tempY<tempMIN ? tempMIN : tempY);
                    tempY=(tempY>tempMAX ? tempMAX : tempY);
                    ST7789_WriteString(tempX,tempY,"L",Font_11x18, YELLOW, RED);
                    tempX=IRInfo120.PostXMax*320/120;
                    tempY=IRInfo120.PostYMax*320/120;
                    tempX=(tempX<tempMIN ? tempMIN : tempX);
                    tempX=(tempX>tempMAX ? tempMAX : tempX);
                    tempY=(tempY<tempMIN ? tempMIN :tempY);
                    tempY=(tempY>tempMAX ? tempMAX : tempY);
                    ST7789_WriteString(tempX,tempY,"H",Font_11x18, YELLOW, RED); 
                }                                                                              					
                g_image_flag=0;                           
            }                                   
        }        

#if 1 //按键	
        u8 key=KEY_Scan(0); 		
	  	if(key)
		{						   
			switch(key)
			{				 
				case KEY1_OK:
                    keyok++;
                    if(keyok>2)
                        keyok=2;
                    // if(!keyok)
                    // {
                    //     // xTaskResume(defaultTaskHandle);
                    //     xTaskResumeAll();                               
                    // }
                    // else
                    // {
                    //     osThreadSuspend(defaultTaskHandle);
                    //     osThreadSuspend(irCameraTaskHandle);     
                    // }
                    if(keyok==2)
                    {
                        switch(keycnt)
                        {
                            case MENU0_0:

                                break;
                            case MENU0_1:

                                break;
                            case MENU0_2:
                                if(temp_select_ok==0)//===首次 不执行
                                {
                                    temp_select_ok=1;
                                    break;
                                }                                
                                if(temp_select_cnt==0)
                                {
                                    center_flag=(~center_flag)&0x01;
                                }
                                else if(temp_select_cnt==1)
                                {
                                    maxmin_flag=(~maxmin_flag)&0x01;
                                }
                                break;
                            case MENU0_3:
                                
                                break;
                            case MENU0_4:
                                if(param_select_ok==0)//===首次 不执行
                                {
                                    param_select_ok=1;
                                    break;
                                }                                
                                if(param_select_cnt==0)
                                {
                                    emiss_flag=(~emiss_flag)&0x01;
                                }
                                else if(param_select_cnt==1)
                                {
                                    degree_flag=(~degree_flag)&0x01;
                                }
                                break;
                            case MENU0_5:
                                
                                break;
                            case MENU0_6:

                                break;
                            case MENU0_7:
                                if(date_select_ok==0)//===首次 不执行
                                {
                                    date_select_ok=1;                                
                                    
                                    // RTC_Get_Time(&hour,&min,&sec,&ampm);                                    	
                                    // RTC_Get_Date(&year,&month,&date,&week);
                                    // year_select_cnt=year;
                                    // month_select_cnt=month;
                                    // day_select_cnt=date;
                                    // hour_select_cnt=hour;
                                    // min_select_cnt=min; 
                                    // pm_flag=ampm; 

                                    break;
                                }                                
                                
                                data_com_flag=(~data_com_flag)&0x01;
                                                                
                                break;
                            case MENU0_8:

                                break;
                            case MENU0_9:

                                break;
                            case MENU0_10:

                                break;
                            default:
                                break;                            
                        }
                    }
					break;						
				case KEY2_DOWN:	
                    if(keyok==1)
                    {
                        if(keycnt>0)
					        keycnt--;
                        else 
                            keycnt=MENU0_NUM-1;
                    }
                    if(keyok==2)
                    {                        
                        switch(keycnt)
                        {
                            case MENU0_0:

                                break;
                            case MENU0_1:
                                if(palette_select_cnt>0)
					                palette_select_cnt--;
                                else    
                                    palette_select_cnt=3;
                                break;
                            case MENU0_2:
                                if(temp_select_cnt>0)
					                temp_select_cnt--;
                                else 
                                    temp_select_cnt=1;
                                break;
                            case MENU0_3:
                                if(unit_select_cnt>0)
					                unit_select_cnt--;
                                else
                                    unit_select_cnt=1;
                                break;
                            case MENU0_4:
                                if(emiss_flag)
                                {
                                    if(emiss_select_cnt>10)
					                    emiss_select_cnt--;
                                    else 
                                        emiss_select_cnt=99;
                                }
                                else if(degree_flag) 
                                {
                                    if(degree_select_cnt>0)
					                    degree_select_cnt--;
                                    else
                                        degree_select_cnt=2;
                                }
                                else
                                {
                                    if(param_select_cnt>0)
					                    param_select_cnt--;
                                    else
                                        param_select_cnt=1;       
                                }   
                                
                                break;
                            case MENU0_5:
                                if(backlight_select_cnt>0)
					                backlight_select_cnt--;
                                else 
                                    backlight_select_cnt=2;
                                break;
                            case MENU0_6:
                                if(language_select_cnt>0)
					                language_select_cnt--;
                                else 
                                    language_select_cnt=1;
                                break;
                            case MENU0_7:
                                if(data_com_flag)
                                {
                                    switch (date_select_cnt)
                                    {
                                        case 0:                                            
                                            if(data12h_select_cnt==0)
                                            {
                                                data12h_select_cnt=1;
                                            }
                                            else
                                            {
                                                data12h_select_cnt=0;                                                
                                            }
                                            break;
                                        case 1:
                                            if(year_select_cnt>0)
                                                year_select_cnt--;
                                            else
                                                year_select_cnt=99;
                                            break;
                                        case 2:
                                            if(month_select_cnt>1)
                                                month_select_cnt--;
                                            else
                                                month_select_cnt=12;
                                            break;
                                        case 3:
                                            if(day_select_cnt>1)
                                                day_select_cnt--;
                                            else
                                                day_select_cnt=30;
                                            break;
                                        case 4:
                                            if(hour_select_cnt>0)
                                                hour_select_cnt--;
                                            else
                                                hour_select_cnt=23;
                                            break;
                                        case 5:
                                            if(min_select_cnt>0)
                                                min_select_cnt--;
                                            else
                                                min_select_cnt=59;
                                            break;                                        
                                        default:
                                            break;
                                    }
                                }
                                else
                                {
                                    if(date_select_cnt>0)
                                        date_select_cnt--;
                                    else 
                                        date_select_cnt=5;
                                }
                                break;
                            case MENU0_8:
                                if(autooff_select_cnt>0)
					                autooff_select_cnt--;
                                else 
                                    autooff_select_cnt=3;
                                break;
                            case MENU0_9:

                                break;
                            case MENU0_10:

                                break;
                            default:
                                break;                            
                        }    
                    }	 				
					break;					
				case KEY3_UP:
                    if(keyok==1)
                    {
                        keycnt++;
                        if(keycnt>MENU0_NUM-1)
                            keycnt=0;
                    }
                    if (keyok==2)
                    {                        
                        switch(keycnt)
                        {
                            case MENU0_0:

                                break;
                            case MENU0_1:
                                palette_select_cnt++;
                                if(palette_select_cnt>3)
                                    palette_select_cnt=0;
                                break;
                            case MENU0_2:
                                temp_select_cnt++;
                                if(temp_select_cnt>1)
                                    temp_select_cnt=0;
                                break;
                            case MENU0_3:
                                unit_select_cnt++;
                                if(unit_select_cnt>1)
                                    unit_select_cnt=0;    
                                break;
                            case MENU0_4:
                                if(emiss_flag)
                                {
                                    emiss_select_cnt++;
                                    if(emiss_select_cnt>99)
                                        emiss_select_cnt=10;
                                }
                                else if(degree_flag)
                                {
                                    degree_select_cnt++;
                                    if(degree_select_cnt>2)
                                        degree_select_cnt=0;
                                }
                                else
                                {
                                    param_select_cnt++;
                                    if(param_select_cnt>1)
                                        param_select_cnt=0;
                                }
                                
                                break;
                            case MENU0_5:
                                backlight_select_cnt++;
                                if(backlight_select_cnt>2)
                                    backlight_select_cnt=0;
                                break;
                            case MENU0_6:
                                language_select_cnt++;
                                if(language_select_cnt>1)
                                    language_select_cnt=0;   
                                break;
                            case MENU0_7:
                                if(data_com_flag)
                                {
                                    switch (date_select_cnt)
                                    {
                                        case 0:
                                            data12h_select_cnt++;
                                            if(data12h_select_cnt>1)
                                                data12h_select_cnt=0;                                            
                                            break;
                                        case 1:
                                            year_select_cnt++;
                                            if(year_select_cnt>99)
                                                year_select_cnt=0; 
                                            break;
                                        case 2:
                                            month_select_cnt++;
                                            if(month_select_cnt>12)
                                                month_select_cnt=1; 
                                            break;
                                        case 3:
                                            day_select_cnt++;
                                            if(day_select_cnt>30)
                                                day_select_cnt=1; 
                                            break;
                                        case 4:
                                            hour_select_cnt++;
                                            if(hour_select_cnt>23)
                                                hour_select_cnt=0; 
                                            break;
                                        case 5:
                                            min_select_cnt++;
                                            if(min_select_cnt>59)
                                                min_select_cnt=0; 
                                            break;                                        
                                        default:
                                            break;
                                    }
                                }
                                else
                                {
                                    date_select_cnt++;
                                    if(date_select_cnt>5)
                                        date_select_cnt=0;
                                }
                                break;
                            case MENU0_8:
                                autooff_select_cnt++;
                                if(autooff_select_cnt>3)
                                    autooff_select_cnt=0;
                                break;
                            case MENU0_9:

                                break;
                            case MENU0_10:

                                break;
                            default:
                                break;                            
                        }
                    }
                    	 		
                    break;
				case KEY4_BACK:
                    switch(keycnt)
                    {
                        case MENU0_4:
                            if((emiss_flag==0)&&(degree_flag==0))
                            {
                                if(keyok>0)
                                    keyok--;
                                if(keyok==0)
                                    keycnt=0;
                                
                                param_select_ok=0;
                                param_select_cnt=0;
                            }
                            break;
                        case MENU0_7:
                            if(data_com_flag==0)
                            {
                                if(keyok>0)
                                    keyok--;
                                if(keyok==0)
                                    keycnt=0;
                                
                                date_select_ok=0;
                                date_select_cnt=0;

                                // year=year_select_cnt;
                                // month=month_select_cnt;
                                // date=day_select_cnt;
                                // hour=hour_select_cnt;
                                // min=min_select_cnt;
                                // sec=0;
                                // if(ampm==0)
                                //     RTC_Set_Time(hour,min,sec,RTC_HOURFORMAT12_AM);
                                // else
                                //     RTC_Set_Time(hour,min,sec,RTC_HOURFORMAT12_PM);	
		                        // RTC_Set_Date(year,month,date,getWeek(year,month,date));
                            }
                            break;
                        default:
                            if(keyok>0)
                                    keyok--;
                            if(keyok==0)
                                keycnt=0;

                            temp_select_ok=0;
                            temp_select_cnt=0;
                            break;
                    }	                    					                                        
					break;
				case KEY5_POWER:	
					shutter_id = NUC_LOOP;
					break;
                default:
                    break;
			}

        #if 1 //菜单
            if(keyok==1)
            {
                menu2_flag=0;
                ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_1);
                if(keycnt<7)
                {
                    if(keycnt==0)
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_2_2);
                    else 
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_2_1);
                    if(keycnt==1)
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_3_2);
                    else 
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_3_1);
                    if(keycnt==2)
                        ST7789_DrawImage3(ICON4_X0, ICON4_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_4_2);
                    else 
                        ST7789_DrawImage3(ICON4_X0, ICON4_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_4_1);
                    if(keycnt==3)
                        ST7789_DrawImage3(ICON5_X0, ICON5_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_5_2);
                    else 
                        ST7789_DrawImage3(ICON5_X0, ICON5_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_5_1);
                    if(keycnt==4)
                        ST7789_DrawImage3(ICON6_X0, ICON6_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_6_2);
                    else 
                        ST7789_DrawImage3(ICON6_X0, ICON6_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_6_1);
                    if(keycnt==5)
                        ST7789_DrawImage3(ICON7_X0, ICON7_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_7_2);
                    else 
                        ST7789_DrawImage3(ICON7_X0, ICON7_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_7_1);
                    if(keycnt==6)
                        ST7789_DrawImage3(ICON8_X0, ICON8_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_8_2);
                    else 
                        ST7789_DrawImage3(ICON8_X0, ICON8_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_8_1);                    
                }
                else
                {
                    if(keycnt==7)
                        ST7789_DrawImage3(ICON9_X0, ICON9_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_9_2);
                    else 
                        ST7789_DrawImage3(ICON9_X0, ICON9_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_9_1);
                    if(keycnt==8)
                        ST7789_DrawImage3(ICON10_X0, ICON10_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_10_2);
                    else 
                        ST7789_DrawImage3(ICON10_X0, ICON10_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_10_1);            
                    if(keycnt==9)
                        ST7789_DrawImage3(ICON11_X0, ICON11_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_11_2);
                    else 
                        ST7789_DrawImage3(ICON11_X0, ICON11_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_11_1);
                    if(keycnt==10)
                        ST7789_DrawImage3(ICON12_X0, ICON12_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_12_2);
                    else 
                        ST7789_DrawImage3(ICON12_X0, ICON12_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_12_1);
                    if(keycnt==11)
                        ST7789_DrawImage3(ICON13_X0, ICON13_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_13_2);
                    else 
                        ST7789_DrawImage3(ICON13_X0, ICON13_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_13_1);
                    
                    ST7789_DrawImage3(ICON7_X0, ICON7_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                    ST7789_DrawImage3(ICON8_X0, ICON8_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);                             
                }                                                            
            }
            else if(keyok==2)
            {
                if(menu2_flag==0)
                {
                    ST7789_Fill_Color(BLACK);
                    menu2_flag=1;
                }
                
                switch(keycnt)
                {	
                    case MENU0_0:
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_2_1);                        
                        ST7789_WriteString(ICON2_X0,ICON2_Y0,"picture browsing",Font_11x18, YELLOW, LGRAY);
                        
                        break;						
                    case MENU0_1:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_3_1);
                        if(palette_select_cnt==0)
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"CLUT1        SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                        {  
                            ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1); 
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"CLUT1",Font_11x18, YELLOW, BLACK);
                        }
                        if(palette_select_cnt==1)
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"JET          SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                        {  
                            ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1); 
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"JET",Font_11x18, YELLOW, BLACK);
                        }                            
                        if(palette_select_cnt==2)
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,"Grayscale    SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                        {  
                            ST7789_DrawImage3(ICON4_X0, ICON4_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1); 
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,"Grayscale",Font_11x18, YELLOW, BLACK);
                        }                            
                        if(palette_select_cnt==3)
                            ST7789_WriteString(ICON5_X0,ICON5_Y0,"HOT          SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                        {  
                            ST7789_DrawImage3(ICON5_X0, ICON5_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1); 
                            ST7789_WriteString(ICON5_X0,ICON5_Y0,"HOT",Font_11x18, YELLOW, BLACK);
                        }                             

                        break;
                    case MENU0_2:
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_4_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        if(temp_select_cnt==0)
                        {
                            if(center_flag==0)
                                ST7789_WriteString(ICON2_X0,ICON2_Y0,"center temperature",Font_11x18, YELLOW, LGRAY);
                            else
                                ST7789_WriteString(ICON2_X0,ICON2_Y0,"center temperature SELECT",Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            if(center_flag==0)
                                ST7789_WriteString(ICON2_X0,ICON2_Y0,"center temperature",Font_11x18, YELLOW, BLACK);
                            else
                                ST7789_WriteString(ICON2_X0,ICON2_Y0,"center temperature SELECT",Font_11x18, YELLOW, BLACK);
                        }
                        if(temp_select_cnt==1)
                        {
                            if(maxmin_flag==0)
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,"max/min temperature",Font_11x18, YELLOW, LGRAY);
                            else
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,"max/min temperature SELECT",Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            if(maxmin_flag==0)
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,"max/min temperature",Font_11x18, YELLOW, BLACK);
                            else
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,"max/min temperature SELECT",Font_11x18, YELLOW, BLACK);
                        }      
                        
                        break;						
                    case MENU0_3:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_5_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        if(unit_select_cnt==0)
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"centigrade degree SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"centigrade degree",Font_11x18, YELLOW, BLACK);
                        if(unit_select_cnt==1)
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"Fahrenheit degree SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"Fahrenheit degree",Font_11x18, YELLOW, BLACK);

                        break;
                    case MENU0_4:
                        u8 param_temp;
                        u8 offset_temp;                        
                        char param_str[4]={"\0"};

                        param_str[0]='0';
                        param_str[1]='.';
                        param_temp=emiss_select_cnt/10;
                        myitoa(param_temp,&param_str[2],10);
                        param_temp=emiss_select_cnt%10;
                        myitoa(param_temp,&param_str[3],10);                        						                                                     

                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_6_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        if(param_select_cnt==0)
                        {
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"emissivity",Font_11x18, YELLOW, LGRAY);
                            if(emiss_flag)
                                ST7789_WriteString(180,ICON2_Y0,param_str,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(180,ICON2_Y0,param_str,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"emissivity",Font_11x18, YELLOW, BLACK);                            
                            ST7789_WriteString(180,ICON2_Y0,param_str,Font_11x18, YELLOW, BLACK);
                        }

                        if(degree_select_cnt<0)
                            param_str[0]='-';
                        else
                            param_str[0]='+';
                        offset_temp=abs(degree_select_cnt); 
                        param_temp=offset_temp/10;
                        myitoa(param_temp,&param_str[1],10);
                        param_str[2]='.';
                        param_temp=offset_temp%10;
                        myitoa(param_temp,&param_str[3],10); 

                        if(param_select_cnt==1)
                        {                            
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"offset",Font_11x18, YELLOW, LGRAY);                            
                            if(degree_flag)
                                ST7789_WriteString(180,ICON3_Y0,param_str,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(180,ICON3_Y0,param_str,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"offset",Font_11x18, YELLOW, BLACK);                            
                            ST7789_WriteString(180,ICON3_Y0,param_str,Font_11x18, YELLOW, BLACK);
                        }  

                        break;						
                    case MENU0_5:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_7_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON4_X0, ICON4_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        if(backlight_select_cnt==0)
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"LOW SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"LOW",Font_11x18, YELLOW, BLACK);
                        if(backlight_select_cnt==1)
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"MID SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"MID",Font_11x18, YELLOW, BLACK);
                        if(backlight_select_cnt==2)
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,"HIGH SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,"HIGH",Font_11x18, YELLOW, BLACK);
                        break;
                    case MENU0_6:
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_8_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        if(language_select_cnt==0)
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"Chinese SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"Chinese",Font_11x18, YELLOW, BLACK);
                        if(language_select_cnt==1)
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"English SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"English",Font_11x18, YELLOW, BLACK);
                        break;						
                    case MENU0_7:
                        u8 date_temp;                                           
                        char date_str1[4]={"\0"};
                        char date_str2[3]={"\0"};
                        char date_str3[3]={"\0"};

                        date_str1[0]='2';
                        date_str1[1]='0';
                        date_temp=year_select_cnt/10;
                        myitoa(date_temp,&date_str1[2],10);
                        date_temp=year_select_cnt%10;
                        myitoa(date_temp,&date_str1[3],10);
                        date_temp=month_select_cnt/10;
                        myitoa(date_temp,&date_str2[0],10);
                        date_temp=month_select_cnt%10;
                        myitoa(date_temp,&date_str2[1],10);
                        date_temp=day_select_cnt/10;
                        myitoa(date_temp,&date_str3[0],10);
                        date_temp=day_select_cnt%10;
                        myitoa(date_temp,&date_str3[1],10);

                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_9_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON4_X0, ICON4_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON5_X0, ICON5_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        if(date_select_cnt==0)
                        {
                            if(data12h_select_cnt==0) 
                            {                          
                                if(data_com_flag)
                                    ST7789_WriteString(ICON2_X0,ICON2_Y0,"24H",Font_11x18, YELLOW, BLUE);
                                else
                                    ST7789_WriteString(ICON2_X0,ICON2_Y0,"24H",Font_11x18, YELLOW, LGRAY);
                            }
                            else 
                            {                          
                                if(data_com_flag)
                                    ST7789_WriteString(ICON2_X0,ICON2_Y0,"12H",Font_11x18, YELLOW, BLUE);
                                else
                                    ST7789_WriteString(ICON2_X0,ICON2_Y0,"12H",Font_11x18, YELLOW, LGRAY);
                            }  
                        }
                        else
                        {                            
                            if(data12h_select_cnt==0) 
                            {                          
                                ST7789_WriteString(ICON2_X0,ICON2_Y0,"24H",Font_11x18, YELLOW, BLACK); 
                            }
                            else 
                            {                          
                                ST7789_WriteString(ICON2_X0,ICON2_Y0,"12H",Font_11x18, YELLOW, BLACK); 
                            }
                        }
                        if(date_select_cnt==1)
                        {                                                                                    
                            if(data_com_flag)
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,date_str1,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,date_str1,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,date_str1,Font_11x18, YELLOW, BLACK);                            
                        }
                        if(date_select_cnt==2)
                        {
                            ST7789_WriteString(ICON3_X0+11*4,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK);
                            if(data_com_flag)
                                ST7789_WriteString(ICON3_X0+11*5,ICON3_Y0,date_str2,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON3_X0+11*5,ICON3_Y0,date_str2,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0+11*4,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK);
                            ST7789_WriteString(ICON3_X0+11*5,ICON3_Y0,date_str2,Font_11x18, YELLOW, BLACK);                                                        
                        }
                        if(date_select_cnt==3)
                        {
                            ST7789_WriteString(ICON3_X0+11*7,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK);                            
                            if(data_com_flag)
                                ST7789_WriteString(ICON3_X0+11*8,ICON3_Y0,date_str3,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON3_X0+11*8,ICON3_Y0,date_str3,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0+11*7,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK); 
                            ST7789_WriteString(ICON3_X0+11*8,ICON3_Y0,date_str3,Font_11x18, YELLOW, BLACK);
                        }

                        memset(date_str1,0,sizeof(date_str1));
                        memset(date_str2,0,sizeof(date_str2));

                        date_temp=hour_select_cnt/10;
                        myitoa(date_temp,&date_str1[0],10);
                        date_temp=hour_select_cnt%10;
                        myitoa(date_temp,&date_str1[1],10);
                        date_temp=min_select_cnt/10;
                        myitoa(date_temp,&date_str2[0],10);
                        date_temp=min_select_cnt%10;
                        myitoa(date_temp,&date_str2[1],10);
                        
                        if(date_select_cnt==4)
                        {                            
                            if(data_com_flag)
                                ST7789_WriteString(ICON4_X0,ICON4_Y0,date_str1,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON4_X0,ICON4_Y0,date_str1,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,date_str1,Font_11x18, YELLOW, BLACK);
                        } 
                        if(date_select_cnt==5)
                        { 
                            ST7789_WriteString(ICON4_X0+11*2,ICON4_Y0,":",Font_11x18, YELLOW, BLACK);                           
                            if(data_com_flag)
                                ST7789_WriteString(ICON4_X0+11*3,ICON4_Y0,date_str2,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON4_X0+11*3,ICON4_Y0,date_str2,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON4_X0+11*2,ICON4_Y0,":",Font_11x18, YELLOW, BLACK);
                            ST7789_WriteString(ICON4_X0+11*3,ICON4_Y0,date_str2,Font_11x18, YELLOW, BLACK);
                        }

                        if(data12h_select_cnt)
                        {
                            if(pm_flag==0)
                                ST7789_WriteString(ICON5_X0,ICON5_Y0,"AM",Font_11x18, YELLOW, BLACK);
                            else  
                                ST7789_WriteString(ICON5_X0,ICON5_Y0,"PM",Font_11x18, YELLOW, BLACK);                               
                        }
                        else
                            ST7789_DrawImage3(ICON5_X0, ICON5_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        break;
                    case MENU0_8:                        	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_10_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON4_X0, ICON4_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON5_X0, ICON5_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        
                        if(autooff_select_cnt==0)
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"5MIN SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"5MIN",Font_11x18, YELLOW, BLACK);
                        if(autooff_select_cnt==1)
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"15MIN SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"15MIN",Font_11x18, YELLOW, BLACK);
                        if(autooff_select_cnt==2)
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,"30MIN SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,"30MIN",Font_11x18, YELLOW, BLACK);
                        if(autooff_select_cnt==3)
                            ST7789_WriteString(ICON5_X0,ICON5_Y0,"OFF SELECT",Font_11x18, YELLOW, LGRAY);
                        else
                            ST7789_WriteString(ICON5_X0,ICON5_Y0,"OFF",Font_11x18, YELLOW, BLACK);
                        break;  
                    case MENU0_9:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_11_1);
                        break; 
                    case MENU0_10:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_12_1);
                        break;
                    case MENU0_11:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_13_1);
                        break; 
                    default:
                        break;
                }                   
            }
            else 
            {
                // ST7789_Fill_Color(RED);
                // ST7789_WriteString(0,30,"ready",Font_11x18, YELLOW, RED);
            }
        #endif            				
		}
#endif                
		osDelay(1);
	}
}

#endif

#if 0
void StartLcdCameraTask(void const * argument)
{
	u8 key=0;
	u8 led1sta=0;

	HAL_GPIO_WritePin(GPIOE, POWE_EN_Pin, GPIO_PIN_SET);
	osDelay(50);
	HAL_GPIO_WritePin(GPIOA, P_EN_1_Pin, GPIO_PIN_SET);
	osDelay(150);
	HAL_GPIO_WritePin(GPIOA, P_EN_2_Pin, GPIO_PIN_SET);
	osDelay(50);
	HAL_GPIO_WritePin(GPIOE, P_EN_3_Pin, GPIO_PIN_SET);

	KEY_Init();
	//LED_Init();
#if 1
	for(;;)
	{
		key=KEY_Scan(0); 		
	  	if(key)
		{						   
			switch(key)
			{				 
				case KEY1_PRES:	
					led1sta=!led1sta;
					break;
				case KEY2_PRES:	
	 				led1sta=!led1sta;
					break;
				case KEY3_PRES:	
					led1sta=!led1sta;
					break;
				case KEY4_PRES:	
					led1sta=!led1sta;
					break;
				case KEY5_PRES:	
					led1sta=!led1sta;
					break;
			}		
			LED1(led1sta);		
		}
		else 
			osDelay(10); 

		osDelay(1);
	}
#else
	//===TEST LED
	for(;;)
	{
		osDelay(500);
		LED1_Toggle;
		osDelay(500);
		LED1_Toggle;
	}
#endif	
}
#endif

#if 0//===W25Q TEST OK
    W25QXX_Erase_Sector(0x3fff10/4096);
    SPI_FLASH_BufferRead(buf,0x3fff10,10);
    buf[0]=0x11;
    buf[1]=0x22;
    W25QXX_Write_Page(buf,0x3fff10,10);
    buf[0]=0;
    buf[1]=0;
    SPI_FLASH_BufferRead(buf,0x3fff10,10);
    //+++
    //===TEST OK
    // W25QXX_Erase_Sector(0x3fff10/4096);
    BSP_W25Qx_Read_Sector(0x3fff10/4096,buf,10);
    buf[0]=0x11;
    buf[1]=0x22;
    BSP_W25Qx_Program_Sector(0x3fff10/4096,buf,2);
    buf[0]=0;
    buf[1]=0;
    BSP_W25Qx_Read_Sector(0x3fff10/4096,buf,10);
    //+++
#endif

#if 0 //===TEST UART OK
    u8 buf[10]={0};
    buf[0]=0x30;
    buf[1]=0x31;
    buf[2]=0x32;
    buf[3]=0x33;
	uart_init(115200);
	// osDelay(100);
	// vTaskSuspendAll();
	printf("START===\r\n\r\n");//���뻻��
	HAL_UART_Transmit(&UART2_Handler,buf,4,5000);
	while(__HAL_UART_GET_FLAG(&UART2_Handler,UART_FLAG_TC)!=SET);		//�ȴ����ͽ���
	printf("END===\r\n\r\n");//���뻻��
	// xTaskResumeAll();
#endif

#if 0//===TEST LCD SPEED 
static u8 test=0;
        test=~test;

        if(test==0)
            ST7789_Fill_Color(0x07E0);
        else 
            ST7789_Fill_Color(0xFFE0);
#endif

// //===TEST
// Shutter_Off();
// osDelay(15);
// Shutter_Stop();
// osDelay(250);
// Shutter_On();
// osDelay(15);
// Shutter_Stop();
// osDelay(250);
// //+++

// MutexSemaphore=xSemaphoreCreateMutex();
// xSemaphoreTake(MutexSemaphore,portMAX_DELAY);
// xSemaphoreGive(MutexSemaphore); 

// vPortEnterCritical();
// vPortExitCritical();

// vTaskSuspendAll();
// xTaskResumeAll();


