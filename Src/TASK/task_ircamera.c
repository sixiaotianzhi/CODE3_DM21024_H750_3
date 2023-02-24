#include "stm32h7xx_hal.h"
#include "cmsis_os.h"
#include "spi_device.h"
#include "ir_def.h"
#include "W25QXX.h"
#include "Varible.h"
#include "MeasureInterface.h"
#include "Sensor.h"
#include "st7789.h"
#include "key.h"
#include "icon.h"
#include "rtc.h" 
#include "bmp.h"
#include "stmflash.h" 
#include "stdio.h" 
#include "sdmmc_sdcard.h" 
#include "fattester.h" 
#include "timer.h" 

#if ADC_EN
#include "adc.h"
#include "math.h"

extern uint16_t g_param_init_done;
extern uint16_t g_start_tmp_read_done;
//uint16_t ad_vaule;
//uint64_t ad;
uint32_t ad;

const float Rp=20000.0;//===分压
const float Rp2=10000.0;//===NTC 10K
const float T2=273.15+25;
const float Bx=3380;//3950;
const float Ka=273.15;
float vol=0;
float Rt=0;
float Rt0=0;//===Rt//Rp并联
float T1=0;

uint8_t chn = 0;
short temp_value;     
uint8_t average_shutter_start_cnt=0;
uint16_t g_ad_channel = 0;
uint16_t g_ad_readcnt = 0;

u8 adc_ch;
#endif

extern DCMI_HandleTypeDef hdcmi;
extern vu8 USB_STATUS_REG;			//USB状态
extern vu8 bDeviceState;			//USB连接 情况
extern uint16_t g_start_tmp_read_done;

static uint16_t frame_count = 0;
__ALIGN_BEGIN static uint8_t dcmi_buff[ ONE_PACKAGE_SIZE * MUTILPEL_BUFFER_NUM ] __ALIGN_END;
__ALIGN_BEGIN static uint8_t X_buff[ FRAME_SIZE ] __ALIGN_END;
__ALIGN_BEGIN static uint8_t y8_buff[ 120*90 ] __ALIGN_END;
__ALIGN_BEGIN static uint8_t y8_buff_filter[ 120*90 ]  __ALIGN_END;
__ALIGN_BEGIN static u16 dst_rgb[ LCD_W*LCD_H ]   __ALIGN_END;

#if 1
u8 key;
u8 palette_select_cnt=2;
u8 temp_select_cnt=0;
u8 temp_select_ok=0;
u8 center_flag=1;
u8 maxmin_flag=1;

u8 unit_select_cnt=0;

u8 param_select_cnt=0;
u8 param_select_ok=0;
u8 emiss_flag=0;
u8 degree_flag=0;
u8 emiss_select_cnt=99;
u8 degree_select_cnt=0;

u8 backlight_select_cnt=0;
u8 language_select_cnt=0;

u8 date_select_cnt=0;
u8 date_select_ok=0;
u8 data24h_flag=0;
u8 data_com_flag=0;
u8 year_flag=0;
u8 month_flag=0;
u8 day_flag=0;
u8 hour_flag=0;
u8 min_flag=0;
u8 pm_flag=0;//===0>>AM/24H 1>>PM
u8 data12h_select_cnt=0;//===0>>24H 1>>12H
u8 year_select_cnt=22;
u8 month_select_cnt=11;
u8 day_select_cnt=22;
u8 hour_select_cnt=12;
u8 min_select_cnt=0;
u8 sec_select_cnt=0;

u8 autooff_select_cnt=0;
u8 menu2_flag=0;

u8 keyok=0;
u8 keycnt=0;
u8 keyback=0;

u8 hour,min,sec,ampm;
u8 year,month,date,week;
u8 tbuf[40];
u8 t=0; 
u8 res;
u8 sd_flag;
u8 picsave_flag;
u8 picsaveok_flag;

u8 param_temp;
u8 offset_temp;                        
char param_str[4]={"\0"};
u8 pTempStr[10]={"\0"};
u8 pTempStr2[10]={"\0"};
char StrSN[20]={"\0"};
						
u8 *pname;
u8 pic_index_save[PIC_SAVE_MAX+1]={"\0"};//===

u8 pic_select_cnt=1;
u8 pic_open_cnt;
u8 pic_select_ok;
int pic_name;//===!!!
u8 pic_num;

u8 sleep_flag;
u8 sleep_en_flag;
volatile u16 sleep_timecnt;
volatile u16 sleep_time1scnt;
volatile u8 pwmcnt;
volatile u16 ad_timecnt;
volatile u16 sd_timecnt;
volatile u16 key_timecnt;
volatile u16 gettemp_timecnt;

u8 bat_level;
u8 usb_flag;

float ad_data=0.401200056;//===0.400809944;

#endif

// #if CAL_PARAM_EN
u8 distance_flag;
u8 kf_flag;
u8 b_flag;
u32 buf_param[20];
//要写入到STM32 FLASH的字符串数组
#define TEXT_LENTH sizeof(buf_param)	 		  	//数组长度	
#define SIZE TEXT_LENTH/4+((TEXT_LENTH%4)?1:0)
#define FLASH_SAVE_ADDR  0X080FF000 	//设置FLASH 保存地址(必须大于32KB地址范围,且为4的倍数. 
// #endif

//====================
u8 power_on_flag=0;
volatile u8 key3_temp1=1;
volatile u8 key3_temp2=1;
volatile u8 key3_cnt=0;
volatile u8 cal_mode1_flag=0;

volatile u8 key4_temp1=1;
volatile u8 key4_temp2=1;
volatile u8 key4_cnt=0;
volatile u8 cal_mode2_flag=0;
//++++++++++++++++++++

void UserGetDCMI(void);
void UserDisplay(void);
void StartDefaultTask2(void);
void UserInitFlash(void);
void UserInitSD(void);
void UserCheckSD(void);
void UserSaveData(void);
void UserInitData(void);
void UserCheckSleep(void);
void UserGetRTC(void);
void UserSetRTC(void);
void UserGetPicName(void);
void UserSavePicName(void);
void UserCheckBacklight(void);
void UserBacklightOff(void);
void UserCheckADC(void);
void UserCheckUsb(void);
void UserPowerOn(void);
void UserPowerOff(void);
void UserCheckPowerOnOff(void);
u8 UserReadParam(u8 *data,u32 ReadAddr,u16 NumByteToRead,u32 Degree);
void UserDisplayCal(void);

void UserParamWrite(u8 w_d_flag);
void UserParamRead(u8 w_d_flag);
void UserCheckDegree(void);
void UserCheckInCal(void);
void UserCheckInCal2(void);
void UserLoadParmModeLoop(void);

void usb_SDK_read_cb(uint8_t *pbuf, uint16_t len)
{
}

void bl_pwm(void)
{
    pwmcnt++;
    if(pwmcnt>20)
        pwmcnt=1;
   
#if PWM_EN
    switch(backlight_select_cnt)
    {
        case 0:           
            if(pwmcnt==1)
                ST7789_LCDBL_Set();
            else 
                ST7789_LCDBL_Clr();
            break;
        case 1:
            if(pwmcnt<11)
                ST7789_LCDBL_Set();
            else 
                ST7789_LCDBL_Clr();
            break;
        case 2:
            ST7789_LCDBL_Set();
            break;
        default:
            break;
    }
#endif

    ad_timecnt++;
    sd_timecnt++;

    sleep_timecnt++;
    if(sleep_timecnt>1000)
    {
        sleep_timecnt=0;
        if(sleep_en_flag)
        {
            if(sleep_time1scnt)
            {
                sleep_time1scnt--;
                if(sleep_time1scnt==0)
                    sleep_flag=1;
            }            
        }
    }

    if(key_timecnt)
        key_timecnt--;

    gettemp_timecnt++;

    UserCheckInCal();
    UserCheckInCal2();
}

u8 getWeek(u8 y, u8 m, u8 d)
{
	if(m==1||m==2) 
	{
		m+=12;
		y--;
	}
	return (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400)%7+1;
}

//=============================================================MAIN LOOP
//===LOAD FLASH MODE
void StartIRCameraTask_LoadParam(void const * argument)
{    
    ST7789_Init();    
    HAL_Delay(2000);//===开机2S
	HAL_GPIO_WritePin(GPIOE, POWE_EN_Pin, GPIO_PIN_SET);    
    #if !PWM_EN
    TIM4_PWM_Init(500-1,8000-1);//200M/200=1M的计数频率，自动重装载为500，那么PWM频率为1M/500=2kHZ 50HZ>>200*40=8000    
    TIM_SetTIM4Compare2(150);
    #endif    
    // ST7789_WriteString(0,30,"power on......",Font_11x18, YELLOW, RED);
    minibmp_decode("1:/ICON/DM.bmp",10,60,320,240,GREEN,0);
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOA, P_EN_1_Pin, GPIO_PIN_SET);
	HAL_Delay(150);
	HAL_GPIO_WritePin(GPIOA, P_EN_2_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOE, P_EN_3_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_5);
    HAL_Delay(250);
			
    RTC_Init();		 					  
    UserInitFlash();

    if(res!=FR_EXIST&&res!=FR_OK)//===判断初始化时创建文件夹	                			          
    {
        ST7789_WriteString(0,30,"ext flash NG",Font_11x18, YELLOW, RED);
        for(;;)
            ;
    }
    else
        UserInitUsb(1);

	for(;;)
	{             
        memset(StrSN,0,20);
        memset(X_buff,0,20);       
        // if(0==UserReadParam(X_buff, (0 + 0)*0x1000, 0x1000, 1))
        if(0==UserReadParam(X_buff, 0x92, 17, 1))
        {
            strncpy(StrSN,(const char *)X_buff,17);            
            ST7789_WriteString(0,30,(const TCHAR*)StrSN,Font_11x18, YELLOW, RED);
            ST7789_WriteString(0,60,"load paramL OK",Font_11x18, YELLOW, RED);
        }
        else
        { 
            ST7789_WriteString(0,30,"load paramL......",Font_11x18, YELLOW, RED);           
            ST7789_WriteString(0,60,"load paramL NG",Font_11x18, YELLOW, RED);
        }

        memset(StrSN,0,20);
        memset(X_buff,0,20); 
        // if(0==UserReadParam(X_buff, (0 + 0)*0x1000, 0x1000, 2))
        if(0==UserReadParam(X_buff, 0x92, 17, 2))
        {
            strncpy(StrSN,(const char *)X_buff,17);            
            ST7789_WriteString(0,90,(const TCHAR*)StrSN,Font_11x18, YELLOW, RED);
            ST7789_WriteString(0,120,"load paramH OK",Font_11x18, YELLOW, RED);
        }
        else
        { 
            ST7789_WriteString(0,90,"load paramH......",Font_11x18, YELLOW, RED);           
            ST7789_WriteString(0,120,"load paramH NG",Font_11x18, YELLOW, RED);
        } 

        HAL_Delay(2000);
        
        UserCheckPowerOnOff();            
    }
}

#if USB_FLASH_EN //===LOAD FLASH MODE
    #if RTOS_EN
void StartIRCameraTask(void const * argument)
    #else
void StartIRCameraTask2(void)
    #endif
{    
    ST7789_Init();    
    HAL_Delay(2000);//===开机2S
	HAL_GPIO_WritePin(GPIOE, POWE_EN_Pin, GPIO_PIN_SET);    
    #if !PWM_EN
    TIM4_PWM_Init(500-1,8000-1);//200M/200=1M的计数频率，自动重装载为500，那么PWM频率为1M/500=2kHZ 50HZ>>200*40=8000    
    TIM_SetTIM4Compare2(150);
    #endif    
    // ST7789_WriteString(0,30,"power on......",Font_11x18, YELLOW, RED);
    minibmp_decode("1:/ICON/DM.bmp",10,60,320,240,GREEN,0);
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOA, P_EN_1_Pin, GPIO_PIN_SET);
	HAL_Delay(150);
	HAL_GPIO_WritePin(GPIOA, P_EN_2_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOE, P_EN_3_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_5);
    HAL_Delay(250);
			
    // KEY_Init();
    RTC_Init();		 					  
    UserInitFlash();

    if(res!=FR_EXIST&&res!=FR_OK)//===判断初始化时创建文件夹	                			          
    {
        ST7789_WriteString(0,30,"ext flash NG",Font_11x18, YELLOW, RED);
        for(;;)
            ;
    }
    else
        UserInitUsb(0);

	for(;;)
	{             
        memset(StrSN,0,20);
        memset(X_buff,0,20);       
        // if(0==UserReadParam(X_buff, (0 + 0)*0x1000, 0x1000, 1))
        if(0==UserReadParam(X_buff, 0x92, 17, 1))
        {
            strncpy(StrSN,(const char *)X_buff,17);            
            ST7789_WriteString(0,30,(const TCHAR*)StrSN,Font_11x18, YELLOW, RED);
            ST7789_WriteString(0,60,"load paramL OK",Font_11x18, YELLOW, RED);
        }
        else
        { 
            ST7789_WriteString(0,30,"load paramL......",Font_11x18, YELLOW, RED);           
            ST7789_WriteString(0,60,"load paramL NG",Font_11x18, YELLOW, RED);
        }

        memset(StrSN,0,20);
        memset(X_buff,0,20); 
        // if(0==UserReadParam(X_buff, (0 + 0)*0x1000, 0x1000, 2))
        if(0==UserReadParam(X_buff, 0x92, 17, 2))
        {
            strncpy(StrSN,(const char *)X_buff,17);            
            ST7789_WriteString(0,90,(const TCHAR*)StrSN,Font_11x18, YELLOW, RED);
            ST7789_WriteString(0,120,"load paramH OK",Font_11x18, YELLOW, RED);
        }
        else
        { 
            ST7789_WriteString(0,90,"load paramH......",Font_11x18, YELLOW, RED);           
            ST7789_WriteString(0,120,"load paramH NG",Font_11x18, YELLOW, RED);
        } 

        HAL_Delay(2000);
        
        UserCheckPowerOnOff();            
    }
}
#else//===NORMAL MODE
    #if RTOS_EN
void StartIRCameraTask(void const * argument)
    #else
void StartIRCameraTask2(void)
    #endif
{    
    ST7789_Init();    
    HAL_Delay(2000);//===开机2S
	HAL_GPIO_WritePin(GPIOE, POWE_EN_Pin, GPIO_PIN_SET);

    TIM4_PWM_Init(500-1,8000-1);//200M/200=1M的计数频率，自动重装载为500，那么PWM频率为1M/500=2kHZ 50HZ>>200*40=8000    
    TIM_SetTIM4Compare2(150);	
    RTC_Init();		 					 
    UserInitFlash();
    UserInitSD();
    UserInitUsb(0);
    MY_ADC_Init();
    UserInitData();

    power_on_flag=1;      
    // ST7789_WriteString(0,30,"power on......",Font_11x18, YELLOW, RED);
    minibmp_decode("1:/ICON/DM.bmp",10,60,320,240,GREEN,0);
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOA, P_EN_1_Pin, GPIO_PIN_SET);
	HAL_Delay(150);
	HAL_GPIO_WritePin(GPIOA, P_EN_2_Pin, GPIO_PIN_SET);
	HAL_Delay(50);
	HAL_GPIO_WritePin(GPIOE, P_EN_3_Pin, GPIO_PIN_SET);
	HAL_Delay(500);
    HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSE, RCC_MCODIV_5);
    HAL_Delay(250);

    SetMeasureDegree(1);//===!!!
    Sensor_Reg_Initial(SPI_Reg_Data120);    
	HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)(X_buff), (FRAME_SIZE / 4));
    HAL_Delay(1000);

    // ST7789_Fill_Color2(RED);
    // ST7789_WriteString(0,30,"ready",Font_11x18, YELLOW, RED); 
    minibmp_decode("1:/ICON/WEL.bmp",160,120,320,240,GREEN,0); 
    HAL_Delay(1000); 
    power_on_flag=0;

	for(;;)
	{
        if(cal_mode1_flag==0)
        {
            UserGetDCMI();        	
            UserDisplay();
            UserSavePicName();        
            UserCheckSD();
            UserCheckBacklight();
            UserCheckPowerOnOff();
            UserCheckUsb();
            UserCheckDegree();

            #if ADC_EN
            if(shutter_id==IDLE)
                UserCheckADC();            
            #endif        
        }
        else
        {
            UserGetDCMI();        	
            UserDisplayCal();
            UserCheckPowerOnOff();

            #if ADC_EN
            if(shutter_id==IDLE)
                UserCheckADC();            
            #endif
        }               
    }
}
#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void DMA_Copy(uint32_t DstAddress, uint32_t SrcAddress, uint32_t DataLength)
{
	DMA_HandleTypeDef     DmaHandle;
  
    DmaHandle.Instance = DMA1_Stream2;//===DMA2_Stream0;
	DmaHandle.Init.Request = DMA_REQUEST_MEM2MEM;	
	DmaHandle.Init.Direction = DMA_MEMORY_TO_MEMORY;
	DmaHandle.Init.PeriphInc = DMA_PINC_ENABLE;
	DmaHandle.Init.MemInc = DMA_MINC_ENABLE;
	DmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DmaHandle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DmaHandle.Init.Mode = DMA_NORMAL;
	DmaHandle.Init.Priority = DMA_PRIORITY_HIGH;
	DmaHandle.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	DmaHandle.Init.MemBurst = DMA_MBURST_SINGLE;              /* Memory burst                     */
    DmaHandle.Init.PeriphBurst = DMA_PBURST_SINGLE;           /* Peripheral burst */

    if (HAL_DMA_Init(&DmaHandle)== HAL_OK)
    {    
        if(HAL_DMA_Start(&DmaHandle,SrcAddress,DstAddress,DataLength)== HAL_OK)
        {
            HAL_DMA_PollForTransfer(&DmaHandle, HAL_DMA_FULL_TRANSFER, 200);
        }
    }	 	
}

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
	uint8_t *pBuff_dcmi = (uint8_t *) ( &dcmi_buff[ ONE_PACKAGE_SIZE * 0] );
	uint8_t *pBuff_dst = pBuff_dcmi + USB_HEADER_SIZE;
	if(frame_count==0)
	{
		frame_count =1;
		DMA_Copy((uint32_t)pBuff_dst, (uint32_t)(X_buff), FRAME_SIZE);       
    }
}

void SPI_DMAXferCplt(DMA_HandleTypeDef *hdma_spi1_tx)
{
    ;
}

void UserGetDCMI(void)
{
    if((keyok==0)&&(picsave_flag==0))
    {                        
        if(frame_count)
        {							
            uint8_t *pBuff_dcmi = (uint8_t *) ( &dcmi_buff[ ONE_PACKAGE_SIZE * 0] );			
            uint8_t *pBuff_x16 = pBuff_dcmi + USB_HEADER_SIZE+120*2*2;
                            
            if(history_shutter_id==NUC_LOOP&&shutter_id==SHUTTER) 
            {
                HAL_DCMI_Stop(&hdcmi);
                HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)(X_buff), (FRAME_SIZE / 4));				
            }					
            history_shutter_id=shutter_id;					
            if(shutter_id!=IDLE)
            {								
                shutter_id=Shutter_State((uint16_t*)pBuff_x16,shutter_id,SPI_Reg_Data120,&Loop_Data120);
            }		
                
            pImage=NUCbyTwoPoint((unsigned short*)pBuff_x16);			
            // InfraredProcessInterface((unsigned short *)pImage);//pImage:Y16 data			
            // memcpy(pBuff_x16, pImage, 120*90*2);
                            
            DealShutterInterface(measureParam120.realTimeTfpa,measureParam120.realTimeTfpa, \
                            measureParam120.realTimeTlen,measureParam120.realTimeTs);                                
                                
            if( gettemp_timecnt>=500 && shutter_id==IDLE)
            {
                GetTempInfo( 0,0,120,90,( short* )pImage,Loop_Data120.us_avg_shutter,&IRInfo120 ,&measureParam120 );                
                gettemp_timecnt=0;
            }
                                                                                            
            frame_count = 0;
        }
    }
}

void UserDisplay(void)
{
    if(shutter_id==IDLE)
    {   
        if((keyok==0)&&(picsave_flag==0))	
        {                    
            LinearDrtInterface(y8_buff, (uint16_t *)pImage);	
			// Y16toY8(y8_buff,pImage);	
            Y8MedianFilter(y8_buff_filter,y8_buff);  
            // Y8AverageFilter(y8_buff,y8_buff_filter);		        
            Y8toRGB((uint16_t *)pImage,y8_buff_filter,palette_select_cnt);                        	
            image120x90to320x240((u8 *)pImage,(u8 *)dst_rgb);                                               
             		                                           
            sprintf((char*)pTempStr,"E:0.%d",emiss_select_cnt);
            ST7789_WriteString3(XY_EMISS,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);

            UserGetRTC();
            if(data12h_select_cnt)
            {
                if(pm_flag)
                    sprintf((char*)pTempStr,"%02d:%02d:%02dPM",hour_select_cnt,min_select_cnt,sec_select_cnt);
                else
                    sprintf((char*)pTempStr,"%02d:%02d:%02dAM",hour_select_cnt,min_select_cnt,sec_select_cnt);
            }
            else
                sprintf((char*)pTempStr,"%02d:%02d:%02d",hour_select_cnt,min_select_cnt,sec_select_cnt); 			
            ST7789_WriteString3(XY_RTC,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);                        

            if(sd_flag)
                ST7789_WriteString3(XY_SD,"SD",Font_11x18, YELLOW, RED, dst_rgb, 0);
            else 
                ST7789_WriteString3(XY_SD,"SD",Font_11x18, LGRAY, RED, dst_rgb, 0);            

            if(unit_select_cnt==0) 			
                ST7789_WriteString3(XY_UNIT,"C",Font_11x18, YELLOW, RED, dst_rgb, 0);
            else
                ST7789_WriteString3(XY_UNIT,"F",Font_11x18, YELLOW, RED, dst_rgb, 0);

            // IRInfo120.MaxValue+=degree_select_cnt/10;
            // IRInfo120.MinValue+=degree_select_cnt/10;
            // IRInfo120.CursorValue+=degree_select_cnt/10;
            
            if(unit_select_cnt==0)
            {			
                sprintf((char*)pTempStr,"H:%.1f",IRInfo120.MaxValue);
                ST7789_WriteString3(XY_MAX,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0); 
                sprintf((char*)pTempStr,"L:%.1f",IRInfo120.MinValue);
                ST7789_WriteString3(XY_MIN,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);
            } 
            else
            {
                sprintf((char*)pTempStr,"H:%.1f",IRInfo120.MaxValue*9/5+32);
                ST7789_WriteString3(XY_MAX,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0); 
                sprintf((char*)pTempStr,"L:%.1f",IRInfo120.MinValue*9/5+32);
                ST7789_WriteString3(XY_MIN,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);
            }

            ST7789_Fill2(XY0_PALETTE, XY1_PALETTE, palette_select_cnt, dst_rgb);

            if(center_flag)
            {
                if(unit_select_cnt==0) 			
                    sprintf((char*)pTempStr,"C:%.1f",IRInfo120.CursorValue);
                else
                    sprintf((char*)pTempStr,"C:%.1f",IRInfo120.CursorValue*9/5+32);                                

                ST7789_WriteString3(XY_CURSOR,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);
                ST7789_DrawCircle2(160,120,5,YELLOW, dst_rgb);
                ST7789_DrawCircle2(160,120,4,YELLOW, dst_rgb);
                ST7789_DrawCircle2(160,120,3,YELLOW, dst_rgb);
                ST7789_DrawLine2(160-5-5,120,160-5,120,YELLOW, dst_rgb);
                ST7789_DrawLine2(160+5,120,160+5+5,120,YELLOW, dst_rgb);
                ST7789_DrawLine2(160,120-5-5,160,120-5,YELLOW, dst_rgb);
                ST7789_DrawLine2(160,120+5,160,120+5+5,YELLOW, dst_rgb);	
            }
            if(maxmin_flag)
            {                                                    
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
                ST7789_WriteString3(tempX,tempY,"L",Font_11x18, YELLOW, RED, dst_rgb, 0);
                tempX=IRInfo120.PostXMax*320/120;
                tempY=IRInfo120.PostYMax*320/120;
                tempX=(tempX<tempMIN ? tempMIN : tempX);
                tempX=(tempX>tempMAX ? tempMAX : tempX);
                tempY=(tempY<tempMIN ? tempMIN :tempY);
                tempY=(tempY>tempMAX ? tempMAX : tempY);
                ST7789_WriteString3(tempX,tempY,"H",Font_11x18, YELLOW, RED, dst_rgb, 0); 
            } 
            
            if(bDeviceState)
                ST7789_WriteString3(XY_USB,"U",Font_11x18, YELLOW, RED, dst_rgb, 0);

            if(usb_flag)
                ST7789_WriteString3(XY_BAT,"BAT5",Font_11x18, YELLOW, RED, dst_rgb, 0);       
            else
            {
                switch(bat_level)
                {                    
                    case 4:
                        ST7789_WriteString3(XY_BAT,"BAT4",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 3:
                        ST7789_WriteString3(XY_BAT,"BAT3",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 2:
                        ST7789_WriteString3(XY_BAT,"BAT2",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 1:
                        ST7789_WriteString3(XY_BAT,"BAT1",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 0:
                        ST7789_WriteString3(XY_BAT,"BAT0",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    default:
                        break;
                }
            } 

            //===BAT
            minibmp_decode2("1:/ICON/BAT.bmp",XY_BAT2,320,240, RED, dst_rgb, 0);  

            ST7789_DrawImage2(0, 0, LCD_W, LCD_H, (uint16_t *)dst_rgb);                                                                                                                                          
        }

        #if KEY_EN //按键	
        key=KEY_Scan(0); 		
        if(key)
        {						   
            switch(key)
            {				 
                case KEY1_OK:
                    if(picsave_flag)
                        break;
                    keyok++;
                    if(keyok>2)
                        keyok=2;                    
                    if(keyok>0)
                    {
                        HAL_DCMI_Suspend(&hdcmi);                                
                    }
                    if(keyok==2)
                    {
                        switch(keycnt)
                        {
                            case MENU0_0:
                                if(bDeviceState)
                                {
                                    keyok=1;//===
                                    ST7789_WriteString2(XY_NOTICE_USB,"USB MODE",Font_11x18, YELLOW, RED, 1);
                                    HAL_Delay(500);
                                    break;
                                }

                                if(pic_select_ok==0)//===首次 不执行
                                {
                                    pic_select_ok=1;
                                    break;
                                }
                                if(sd_flag)
                                {
                                    if(pic_num)
                                    {
                                        pic_open_cnt++;
                                        if(pic_open_cnt>2)
                                            pic_open_cnt=2;
                                    }
                                } 
                                 
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
                                // #if CAL_PARAM_EN
                                // else
                                if(cal_mode2_flag)
                                {
                                    if(param_select_cnt==2)
                                    {
                                        distance_flag=(~distance_flag)&0x01;
                                        if(distance_flag==0)
                                            UserParamWrite(0);//===UserParamWrite(1);
                                    }
                                    else if(param_select_cnt==3)
                                    {
                                        kf_flag=(~kf_flag)&0x01;
                                        if(kf_flag==0)
                                            UserParamWrite(0);//===UserParamWrite(1);
                                    }
                                    else if(param_select_cnt==4)
                                    {
                                        b_flag=(~b_flag)&0x01;
                                        if(b_flag==0)
                                            UserParamWrite(0);//===UserParamWrite(1);
                                    }
                                } 
                                // #endif
                                break;
                            case MENU0_5:
                                
                                break;
                            case MENU0_6:

                                break;
                            case MENU0_7:
                                if(date_select_ok==0)//===首次 不执行
                                {
                                    date_select_ok=1;                                                                    
                                    UserGetRTC();
                                    break;
                                }                                
                                
                                data_com_flag=(~data_com_flag)&0x01;
                                if(data_com_flag==0)                                                                                                                                             
                                    UserSetRTC();
                                                                
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
                    if((keyok==0)&&(picsave_flag==0))
                        shutter_id = NUC_LOOP;
                    else if(keyok==1)
                    {
                        if(keycnt>0)
                            keycnt--;
                        else 
                            keycnt=MENU0_NUM-1;
                    }
                    else if(keyok==2)
                    {                        
                        switch(keycnt)
                        {
                            case MENU0_0:
                                if(pic_open_cnt==1)
                                    break;
                                else if(pic_open_cnt==2)
                                {
                                    pic_open_cnt=0;
                                    menu2_flag=0;
                                    pname=pic_memalloc(30);//为带路径的文件名分配30个字节的内存
                                    sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",pic_index_save[pic_select_cnt]);
                                    if(FR_OK==f_unlink((const TCHAR*)pname))
                                    {                                        
                                        for(u8 cnt1=pic_select_cnt;cnt1<pic_num;cnt1++)
                                            pic_index_save[cnt1]=pic_index_save[cnt1+1];
                                        pic_num--;

                                        if(pic_select_cnt>1)
                                            pic_select_cnt--;                                                                                                                                
                                    }
                                    pic_memfree(pname);
                                    
                                    break;
                                }
                                
                                if(pic_num)
                                {
                                    if(pic_num>7)
                                    {
                                        if( (pic_select_cnt-1)%7==0 )//===翻页 清屏
                                            menu2_flag=0;
                                    }
                                    if(pic_select_cnt>1)
                                        pic_select_cnt--;
                                    else    
                                        pic_select_cnt=pic_num;
                                }                                
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
                                    measureParam120.emiss=emiss_select_cnt;
                                }
                                else if(degree_flag) 
                                {
                                    if(degree_select_cnt>0)
                                        degree_select_cnt--;
                                    else
                                        degree_select_cnt=2;
                                }
                                
                                // #if CAL_PARAM_EN
                                // else
                                if(cal_mode2_flag) 
                                {
                                    if(distance_flag) 
                                    {
                                        if(measureParam120.distance==5)
                                            measureParam120.distance=12;
                                        else
                                            measureParam120.distance=5;
                                    }
                                    else if(kf_flag) 
                                    {
                                        if(degree_select_cnt==0)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[0].kf-=100;
                                            else
                                                measureParam120.fliter[0].kf_2-=100;
                                        }
                                        else if(degree_select_cnt==1)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[1].kf-=100;
                                            else
                                                measureParam120.fliter[1].kf_2-=100;
                                        }                                        
                                    }
                                    else if(b_flag) 
                                    {
                                        if(degree_select_cnt==0)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[0].b-=10;
                                            else
                                                measureParam120.fliter[0].b2-=10;
                                        }
                                        else if(degree_select_cnt==1)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[1].b-=100;
                                            else
                                                measureParam120.fliter[1].b2-=100;
                                        }                                        
                                    }
                                }
                                // #endif
                                // else
                                if((emiss_flag==0)&&(degree_flag==0)&&(distance_flag==0)&&(kf_flag==0)&&(b_flag==0))
                                {
                                    if(param_select_cnt>0)
                                        param_select_cnt--;
                                    else
                                    {
                                        // #if CAL_PARAM_EN
                                        if(cal_mode2_flag)
                                            param_select_cnt=4;
                                        // #else
                                        else
                                            param_select_cnt=1;
                                        // #endif
                                    }                                               
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
                                                data12h_select_cnt=1;
                                            else
                                                data12h_select_cnt=0;
                                            
                                            if(data12h_select_cnt)//===24>>12转换
                                            {
                                                if(hour_select_cnt>=12)
                                                    pm_flag=1;//===
                                                else
                                                    pm_flag=0;//===

                                                if(hour_select_cnt>12)
                                                    hour_select_cnt-=12;
                                                if(hour_select_cnt==0)
                                                    hour_select_cnt=12;
                                            }
                                            else//===12>>24转换
                                            {
                                                if(hour_select_cnt==12) 
                                                {
                                                    if(pm_flag==0)
                                                        hour_select_cnt=0;
                                                } 
                                                else
                                                {    
                                                    if(pm_flag)
                                                        hour_select_cnt+=12;
                                                }          
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
                                            if(hour_select_cnt==0)
                                                if(data12h_select_cnt==0)
                                                    hour_select_cnt=24;
                                            if(hour_select_cnt>0)
                                                hour_select_cnt--;
                                            if(hour_select_cnt==0)                                      
                                                if(data12h_select_cnt)
                                                    hour_select_cnt=12;
                                            if(data12h_select_cnt==0)
                                            {
                                                if(hour_select_cnt>=12)
                                                        pm_flag=1;//===
                                                    else
                                                        pm_flag=0;//===
                                            }                                                                                                                                         
                                            break;
                                        case 5:
                                            if(min_select_cnt>0)
                                                min_select_cnt--;
                                            else
                                                min_select_cnt=59;
                                            break;
                                        case 6:                                            
                                            if(pm_flag==0)
                                                pm_flag=1;
                                            else
                                                pm_flag=0;                                                
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
                                    {
                                        if(data12h_select_cnt)
                                            date_select_cnt=6;
                                        else    
                                            date_select_cnt=5;
                                    }                                         
                                }
                                break;
                            case MENU0_8:
                                if(autooff_select_cnt>0)
                                    autooff_select_cnt--;
                                else 
                                    autooff_select_cnt=3;
                                UserCheckSleep();
                                break;
                            case MENU0_9:
                                if(sd_flag)
                                {   
                                    #if RTOS_EN
                                    vTaskSuspendAll();
                                    #endif
                                    ST7789_DrawImage3(ICON6_X0, ICON6_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                                    ST7789_WriteString2(XY_NOTICE_SD,"FORMATTING",Font_11x18, YELLOW, RED, 1);
                                    if(mf_fmkfs((u8 *)"0:",FM_FAT32,(FF_MAX_SS*128))!=FR_OK)
                                    {
                                        ST7789_DrawImage3(ICON6_X0, ICON6_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                                        ST7789_WriteString2(XY_NOTICE_SD,"FORMAT NG",Font_11x18, YELLOW, RED, 1);
                                    }
                                    else
                                    {
                                        ST7789_DrawImage3(ICON6_X0, ICON6_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                                        ST7789_WriteString2(XY_NOTICE_SD,"FORMAT OK",Font_11x18, YELLOW, RED, 1);
                                        pic_num=0;
                                        pic_name=0;
                                        UserInitSD();
                                    }
                                    #if RTOS_EN
                                    xTaskResumeAll();
                                    #endif
                                } 
                                break;
                            case MENU0_10:
                                UserInitData();
                                if(keyok>0)
                                    keyok--; 
                                break;
                            case MENU0_11:

                                break;
                            default:
                                break;                            
                        }    
                    }	 				
                    if(sd_flag)
                    {
                        if(picsave_flag)                            
                            picsaveok_flag=1;
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
                                if(pic_open_cnt==1)
                                    break;
                                else if(pic_open_cnt==2)
                                {
                                    pic_open_cnt=1;
                                    break;
                                }

                                if(pic_num)
                                {
                                    if(pic_select_cnt%7==0)//===翻页 清屏
                                        menu2_flag=0;
                                    pic_select_cnt++;
                                    if(pic_select_cnt>pic_num)
                                        pic_select_cnt=1; 
                                }                               
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
                                    measureParam120.emiss=emiss_select_cnt;
                                }
                                else if(degree_flag)
                                {
                                    degree_select_cnt++;
                                    if(degree_select_cnt>2)
                                        degree_select_cnt=0;
                                }
                                // #if CAL_PARAM_EN
                                // else 
                                if(cal_mode2_flag)
                                {
                                    if(distance_flag) 
                                    {
                                        if(measureParam120.distance==5)
                                            measureParam120.distance=12;
                                        else
                                            measureParam120.distance=5;
                                    }
                                    else if(kf_flag) 
                                    {
                                        if(degree_select_cnt==0)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[0].kf+=100;
                                            else
                                                measureParam120.fliter[0].kf_2+=100;
                                        }
                                        else if(degree_select_cnt==1)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[1].kf+=100;
                                            else
                                                measureParam120.fliter[1].kf_2+=100;
                                        }                                        
                                    }
                                    else if(b_flag) 
                                    {
                                        if(degree_select_cnt==0)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[0].b+=10;
                                            else
                                                measureParam120.fliter[0].b2+=10;
                                        }
                                        else if(degree_select_cnt==1)
                                        {
                                            if(measureParam120.distance==5)
                                                measureParam120.fliter[1].b+=100;
                                            else
                                                measureParam120.fliter[1].b2+=100;
                                        }                                        
                                    }
                                }
                                // #endif
                                // else
                                if((emiss_flag==0)&&(degree_flag==0)&&(distance_flag==0)&&(kf_flag==0)&&(b_flag==0))                           
                                {
                                    param_select_cnt++;
                                    
                                    // #if CAL_PARAM_EN
                                    // if(param_select_cnt>4)
                                    // #else 
                                    // if(param_select_cnt>1)
                                    // #endif
                                    //     param_select_cnt=0;
                                    
                                    if(cal_mode2_flag)
                                    {
                                        if(param_select_cnt>4)
                                            param_select_cnt=0;
                                    }
                                    else
                                    {
                                        if(param_select_cnt>1)
                                            param_select_cnt=0;
                                    }
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
                                            
                                            if(data12h_select_cnt)//===24>>12转换
                                            {
                                                if(hour_select_cnt>=12)
                                                    pm_flag=1;//===
                                                else
                                                    pm_flag=0;//===

                                                if(hour_select_cnt>12)
                                                    hour_select_cnt-=12;
                                                if(hour_select_cnt==0)
                                                    hour_select_cnt=12;
                                            }
                                            else//===12>>24转换
                                            {
                                                if(hour_select_cnt==12) 
                                                {
                                                    if(pm_flag==0)
                                                        hour_select_cnt=0;
                                                } 
                                                else
                                                {    
                                                    if(pm_flag)
                                                        hour_select_cnt+=12;
                                                }          
                                            }                                               
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
                                            if(data12h_select_cnt)
                                            {
                                                if(hour_select_cnt>12)
                                                    hour_select_cnt=1;            
                                            }
                                            else
                                            {
                                                if(hour_select_cnt>23)
                                                    hour_select_cnt=0;
                                            }
                                            if(data12h_select_cnt==0)
                                            {
                                                if(hour_select_cnt>=12)
                                                        pm_flag=1;//===
                                                    else
                                                        pm_flag=0;//===
                                            }                        
                                            break;
                                        case 5:
                                            min_select_cnt++;
                                            if(min_select_cnt>59)
                                                min_select_cnt=0; 
                                            break; 
                                        case 6:                                            
                                            if(pm_flag==0)
                                                pm_flag=1;
                                            else
                                                pm_flag=0;                                                
                                            break;                                        
                                        default:
                                            break;
                                    }
                                }
                                else
                                {
                                    date_select_cnt++;
                                    if(data12h_select_cnt)
                                    {
                                        if(date_select_cnt>6)
                                            date_select_cnt=0;       
                                    }                                 
                                    else 
                                    {
                                        if(date_select_cnt>5)
                                            date_select_cnt=0;
                                    }                                                                               
                                }
                                break;
                            case MENU0_8:
                                autooff_select_cnt++;
                                if(autooff_select_cnt>3)
                                    autooff_select_cnt=0;
                                UserCheckSleep();
                                break;
                            case MENU0_9:
                                if(sd_flag)
                                {
                                    if(keyok>0)
                                        keyok--;
                                }
                                break;
                            case MENU0_10:
                                if(keyok>0)
                                    keyok--; 
                                break;
                            case MENU0_11:

                                break;
                            default:
                                break;                            
                        }
                    }
                    if(sd_flag)
                    {
                        if(picsave_flag)                            
                            picsave_flag=0;
                    }                                                          
                    break;
                case KEY4_BACK:
                    if(keyok==0&&bDeviceState==0)//===拍照 放前面 不包括从设置菜单返回!!!
                    {
                        picsave_flag=!picsave_flag;
                        if(picsave_flag)
                        {                 
                            HAL_DCMI_Suspend(&hdcmi);
                            if(sd_flag==0)                          
                                ST7789_WriteString2(XY_NOTICE_SD,"No SD card",Font_11x18, YELLOW, RED, 1);                                                                           
                        }
                    }
                    else if(keyok==0&&bDeviceState==1)
                    {
                        ST7789_WriteString2(XY_NOTICE_USB,"USB MODE",Font_11x18, YELLOW, RED, 1);
                        HAL_Delay(500);
                    }

                    switch(keycnt)
                    {
                        case MENU0_0:
                            if(pic_open_cnt)
                            {
                                pic_open_cnt--;
                                if(pic_open_cnt==0)
                                    menu2_flag=0;
                                break;
                            }
                            if(pic_open_cnt==0)
                            {
                                if(keyok>0)
                                    keyok--;                                
                                
                                pic_select_ok=0;
                                pic_select_cnt=1;
                            }
                            break;
                        case MENU0_4:
                            // #if CAL_PARAM_EN
                            // if((emiss_flag==0)&&(degree_flag==0)&&(distance_flag==0)&&(kf_flag==0)&&(b_flag==0))
                            // #else
                            // if((emiss_flag==0)&&(degree_flag==0)) 
                            // #endif
                            // {
                            //     if(keyok>0)
                            //         keyok--;                                
                                
                            //     param_select_ok=0;
                            //     param_select_cnt=0;
                            // }
                            if(cal_mode2_flag)
                            {
                                if((emiss_flag==0)&&(degree_flag==0)&&(distance_flag==0)&&(kf_flag==0)&&(b_flag==0))                                
                                {
                                    if(keyok>0)
                                        keyok--;                                
                                    
                                    param_select_ok=0;
                                    param_select_cnt=0;
                                }
                            }
                            else
                            {
                                if((emiss_flag==0)&&(degree_flag==0)) 
                                {
                                    if(keyok>0)
                                        keyok--;                                
                                    
                                    param_select_ok=0;
                                    param_select_cnt=0;
                                }
                            }                            
                            break;
                        case MENU0_7:
                            if(data_com_flag==0)
                            {
                                if(keyok>0)
                                    keyok--;                                
                                
                                date_select_ok=0;
                                date_select_cnt=0;                                
                            }
                            break;
                        default:
                            if(keyok>0)
                                keyok--;                                                            
                            
                            temp_select_ok=0;
                            temp_select_cnt=0;
                            
                            break;
                    }                              

                    if(keyok==0)
                    {
                        keycnt=0; 	                          	
                        HAL_DCMI_Resume(&hdcmi);
                        HAL_DCMI_Stop(&hdcmi);
                        HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_CONTINUOUS, (uint32_t)(X_buff), (FRAME_SIZE / 4));                     
                    }
                   
                    break;
                // case KEY5_POWER:                    	                    
                //     break;                
                default:
                    break;
            }

        #if KEY_MENU_EN //菜单
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
            #if 1
            else if(keyok==2)
            {
                if(menu2_flag==0)
                {
                    ST7789_Fill_Color2(BLACK);
                    menu2_flag=1;
                }
                
                switch(keycnt)
                {	
                    case MENU0_0:                        
                        if(sd_flag==0)
                        {
                            ST7789_WriteString2(XY_NOTICE_SD,"No SD card",Font_11x18, YELLOW, RED, 1);
                            break;
                        }

                        pname=pic_memalloc(30);//为带路径的文件名分配30个字节的内存
                        if(pic_open_cnt==0)
                        {
                            ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_2_1);

                            sprintf((char*)pname,"%d/%d",pic_select_cnt,pic_num);
                            ST7789_WriteString2(XY_PICNUM,(const TCHAR*)pname,Font_11x18, YELLOW, RED, 1);
                            
                            if(pic_num==0)
                            {
                                ST7789_WriteString2(XY_PIC_NOSAVE,"NO SAVE",Font_11x18, YELLOW, RED, 1);
                            }
                            else
                            {
                                u8 pic_page;
                                u8 cnt1max;
                                u8 cnt1;
                                pic_page=(pic_select_cnt-1)/7;//===page0 start                                
                                
                                switch(pic_page)
                                {  
                                    case 0:
                                        if(pic_num<=7)
                                            cnt1max=pic_num;
                                        else
                                            cnt1max=7;
                                        for(cnt1=7*0+1;cnt1<=cnt1max;cnt1++)
                                        {                                        
                                            sprintf((char*)pname,"PIC%05d.bmp",pic_index_save[cnt1]);
                                            if(pic_select_cnt==cnt1)
                                                ST7789_WriteString(ICON2_X0*(cnt1),ICON2_Y0*(cnt1),(const TCHAR*)pname,Font_11x18, YELLOW, LGRAY);
                                            else    
                                                ST7789_WriteString(ICON2_X0*(cnt1),ICON2_Y0*(cnt1),(const TCHAR*)pname,Font_11x18, YELLOW, BLACK);                                                                               
                                        }
                                        break;
                                    case 1:
                                        if(pic_num<=7*2)
                                            cnt1max=pic_num;
                                        else
                                            cnt1max=7*2;
                                        for(cnt1=7*1+1;cnt1<=cnt1max;cnt1++)
                                        {                                            
                                            sprintf((char*)pname,"PIC%05d.bmp",pic_index_save[cnt1]);
                                            if(pic_select_cnt==cnt1)
                                                ST7789_WriteString(ICON2_X0*(cnt1-7*1),ICON2_Y0*(cnt1-7*1),(const TCHAR*)pname,Font_11x18, YELLOW, LGRAY);
                                            else    
                                                ST7789_WriteString(ICON2_X0*(cnt1-7*1),ICON2_Y0*(cnt1-7*1),(const TCHAR*)pname,Font_11x18, YELLOW, BLACK);                                                                                   
                                        }
                                        break;
                                    case 2:
                                        if(pic_num<=7*3)
                                            cnt1max=pic_num;
                                        else
                                            cnt1max=7*3;
                                        for(cnt1=7*2+1;cnt1<=cnt1max;cnt1++)
                                        {                                            
                                            sprintf((char*)pname,"PIC%05d.bmp",pic_index_save[cnt1]);
                                            if(pic_select_cnt==cnt1)
                                                ST7789_WriteString(ICON2_X0*(cnt1-7*2),ICON2_Y0*(cnt1-7*2),(const TCHAR*)pname,Font_11x18, YELLOW, LGRAY);
                                            else    
                                                ST7789_WriteString(ICON2_X0*(cnt1-7*2),ICON2_Y0*(cnt1-7*2),(const TCHAR*)pname,Font_11x18, YELLOW, BLACK);                                                                                 
                                        }
                                        break;
                                    default:
                                        break;
                                }                                                                             
                            }                                 
                        }
                        else if(pic_open_cnt==1)
                        {                            
                            sprintf((char*)pname,"0:PHOTO/PIC%05d.bmp",pic_index_save[pic_select_cnt]);                                                                    
                            res=stdbmp_decode2(pname,dst_rgb);
                            ST7789_DrawImage2(0, 0, LCD_W, LCD_H, (uint16_t *)dst_rgb);                                     
                        }
                        else if(pic_open_cnt==2)
                        {
                            ST7789_WriteString2(XY_DELETE,"delete",Font_11x18, YELLOW, RED, 1);
                            ST7789_WriteString2(XY_CANCEL,"cancel",Font_11x18, YELLOW, RED, 1);
                        }
                        pic_memfree(pname);                  
                        
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
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_6_1);
                        ST7789_DrawImage3(ICON2_X0, ICON2_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                        ST7789_DrawImage3(ICON3_X0, ICON3_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);

                        sprintf((char*)pTempStr,"0.%d",emiss_select_cnt);
                        if(param_select_cnt==0)
                        {
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"emissivity",Font_11x18, YELLOW, LGRAY);
                            if(emiss_flag)
                                ST7789_WriteString(180,ICON2_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(180,ICON2_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"emissivity",Font_11x18, YELLOW, BLACK);                            
                            ST7789_WriteString(180,ICON2_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                        }
                                                
                        if(degree_select_cnt==0)
                            sprintf((char*)pTempStr,"L");
                        else if(degree_select_cnt==1)
                            sprintf((char*)pTempStr,"H");
                        else 
                            sprintf((char*)pTempStr,"AUTO");

                        if(param_select_cnt==1)
                        {                            
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"degree",Font_11x18, YELLOW, LGRAY);                            
                            if(degree_flag)
                                ST7789_WriteString(180,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(180,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"degree",Font_11x18, YELLOW, BLACK);                            
                            ST7789_WriteString(180,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                        }
                        // #if CAL_PARAM_EN
                        if(cal_mode2_flag)
                        {
                            sprintf((char*)pTempStr,"%02d",measureParam120.distance);
                            if(param_select_cnt==2)
                            {                                                        
                                ST7789_WriteString(ICON4_X0,ICON4_Y0,"D",Font_11x18, YELLOW, LGRAY);                            
                                if(distance_flag)
                                    ST7789_WriteString(180,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                                else
                                    ST7789_WriteString(180,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                            }
                            else
                            {                            
                                ST7789_WriteString(ICON4_X0,ICON4_Y0,"D",Font_11x18, YELLOW, BLACK);                            
                                ST7789_WriteString(180,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                            }

                            if(degree_select_cnt==0)
                            {
                                if(measureParam120.distance==5)
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[0].kf);
                                    sprintf((char*)pTempStr2,"L_K1");
                                }
                                else
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[0].kf_2);
                                    sprintf((char*)pTempStr2,"L_K2");
                                }                                
                            }
                            else
                            {
                                if(measureParam120.distance==5)
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[1].kf);
                                    sprintf((char*)pTempStr2,"H_K1");
                                }
                                else
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[1].kf_2);
                                    sprintf((char*)pTempStr2,"H_K2");
                                }
                            }
                            ST7789_DrawImage3(ICON5_X0, ICON5_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);                             
                            if(param_select_cnt==3)
                            {                                                                                        
                                ST7789_WriteString(ICON5_X0,ICON5_Y0,(char*)pTempStr2,Font_11x18, YELLOW, LGRAY);                            
                                if(kf_flag)
                                    ST7789_WriteString(180,ICON5_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                                else
                                    ST7789_WriteString(180,ICON5_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                            }
                            else
                            {                                                            
                                ST7789_WriteString(ICON5_X0,ICON5_Y0,(char*)pTempStr2,Font_11x18, YELLOW, BLACK);                            
                                ST7789_WriteString(180,ICON5_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                            }
                            
                            if(degree_select_cnt==0)
                            {
                                if(measureParam120.distance==5)
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[0].b);
                                    sprintf((char*)pTempStr2,"L_B1");
                                }
                                else
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[0].b2);
                                    sprintf((char*)pTempStr2,"L_B2");
                                }                                
                            }
                            else
                            {
                                if(measureParam120.distance==5)
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[1].b);
                                    sprintf((char*)pTempStr2,"H_B1");
                                }
                                else
                                {
                                    sprintf((char*)pTempStr,"%d",measureParam120.fliter[1].b2);
                                    sprintf((char*)pTempStr2,"H_B2");
                                }
                            }
                            ST7789_DrawImage3(ICON6_X0, ICON6_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_14_1);
                            if(param_select_cnt==4)
                            {                                                                                        
                                ST7789_WriteString(ICON6_X0,ICON6_Y0,(char*)pTempStr2,Font_11x18, YELLOW, LGRAY);                            
                                if(b_flag)
                                    ST7789_WriteString(180,ICON6_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                                else
                                    ST7789_WriteString(180,ICON6_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                            }
                            else
                            {                                                            
                                ST7789_WriteString(ICON6_X0,ICON6_Y0,(char*)pTempStr2,Font_11x18, YELLOW, BLACK);                            
                                ST7789_WriteString(180,ICON6_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                            }
                            // sprintf((char*)pTempStr,"Ts=%d",measureParam120.realTimeTs);
                            // ST7789_WriteString(ICON7_X0,ICON7_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                            // sprintf((char*)pTempStr,"Tlen=%d",measureParam120.realTimeTlen);
                            // ST7789_WriteString(ICON8_X0,ICON8_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                            // sprintf((char*)pTempStr,"Tfpa=%d",measureParam120.realTimeTfpa);
                            // ST7789_WriteString(180,ICON7_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                        }
                        // #endif 

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
                        {
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"Chinese SELECT",Font_11x18, YELLOW, LGRAY);                            
                        }
                        else
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,"Chinese",Font_11x18, YELLOW, BLACK);
                        if(language_select_cnt==1)
                        {
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"English SELECT",Font_11x18, YELLOW, LGRAY);                            
                        }
                        else
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,"English",Font_11x18, YELLOW, BLACK);
                        break;						
                    case MENU0_7:                                                        
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
                        
                        sprintf((char*)pTempStr,"20%02d",year_select_cnt);
                        if(date_select_cnt==1)
                        {                                                                                    
                            if(data_com_flag)
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON3_X0,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);                            
                        }
                        sprintf((char*)pTempStr,"%02d",month_select_cnt);
                        if(date_select_cnt==2)
                        {
                            ST7789_WriteString(ICON3_X0+11*4,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK);
                            if(data_com_flag)
                                ST7789_WriteString(ICON3_X0+11*5,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON3_X0+11*5,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0+11*4,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK);
                            ST7789_WriteString(ICON3_X0+11*5,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);                                                        
                        }
                        sprintf((char*)pTempStr,"%02d",day_select_cnt);
                        if(date_select_cnt==3)
                        {
                            ST7789_WriteString(ICON3_X0+11*7,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK);                            
                            if(data_com_flag)
                                ST7789_WriteString(ICON3_X0+11*8,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON3_X0+11*8,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON3_X0+11*7,ICON3_Y0,"-",Font_11x18, YELLOW, BLACK); 
                            ST7789_WriteString(ICON3_X0+11*8,ICON3_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                        }
                        
                        sprintf((char*)pTempStr,"%02d",hour_select_cnt);
                        if(date_select_cnt==4)
                        {                            
                            if(data_com_flag)
                                ST7789_WriteString(ICON4_X0,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON4_X0,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON4_X0,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                        }
                        sprintf((char*)pTempStr,"%02d",min_select_cnt); 
                        if(date_select_cnt==5)
                        { 
                            ST7789_WriteString(ICON4_X0+11*2,ICON4_Y0,":",Font_11x18, YELLOW, BLACK);                           
                            if(data_com_flag)
                                ST7789_WriteString(ICON4_X0+11*3,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLUE);
                            else
                                ST7789_WriteString(ICON4_X0+11*3,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, LGRAY);
                        }
                        else
                        {
                            ST7789_WriteString(ICON4_X0+11*2,ICON4_Y0,":",Font_11x18, YELLOW, BLACK);
                            ST7789_WriteString(ICON4_X0+11*3,ICON4_Y0,(char*)pTempStr,Font_11x18, YELLOW, BLACK);
                        }

                        if(data12h_select_cnt)
                        {
                            if(date_select_cnt==6)
                            {
                                if(data_com_flag)
                                {
                                    if(pm_flag==0)
                                        ST7789_WriteString(ICON5_X0,ICON5_Y0,"AM",Font_11x18, YELLOW, BLUE);
                                    else  
                                        ST7789_WriteString(ICON5_X0,ICON5_Y0,"PM",Font_11x18, YELLOW, BLUE);
                                }
                                else
                                {
                                    if(pm_flag==0)
                                        ST7789_WriteString(ICON5_X0,ICON5_Y0,"AM",Font_11x18, YELLOW, LGRAY);
                                    else  
                                        ST7789_WriteString(ICON5_X0,ICON5_Y0,"PM",Font_11x18, YELLOW, LGRAY);
                                } 
                                
                            }
                            else
                            {
                                if(pm_flag==0)
                                    ST7789_WriteString(ICON5_X0,ICON5_Y0,"AM",Font_11x18, YELLOW, BLACK);
                                else  
                                    ST7789_WriteString(ICON5_X0,ICON5_Y0,"PM",Font_11x18, YELLOW, BLACK);
                            }                               
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
                        if(sd_flag==0)
                        {
                            ST7789_WriteString2(XY_NOTICE_SD,"No SD card",Font_11x18, YELLOW, RED, 1);
                        }
                        else
                        {                            
                            u32 total,free;
                            exf_getfree("0",&total,&free);
                            sprintf((char*)tbuf,"total:%dM",total>>10);
                            ST7789_WriteString(ICON2_X0,ICON2_Y0,(char*)tbuf,Font_11x18, YELLOW, BLACK); 
                            sprintf((char*)tbuf,"free:%dM",free>>10);
                            ST7789_WriteString(ICON3_X0,ICON3_Y0,(char*)tbuf,Font_11x18, YELLOW, BLACK); 

                            ST7789_WriteString2(XY_DELETE,"format",Font_11x18, YELLOW, RED, 1);
                            ST7789_WriteString2(XY_CANCEL,"cancel",Font_11x18, YELLOW, RED, 1);                           
                        }                        
                        break; 
                    case MENU0_10:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_12_1);
                        ST7789_WriteString2(XY_SURE,"sure",Font_11x18, YELLOW, RED, 1);
                        ST7789_WriteString2(XY_QUIT2,"quit",Font_11x18, YELLOW, RED, 1);
                        break;
                    case MENU0_11:	
                        ST7789_DrawImage3(ICON1_X0, ICON1_Y0, ICON1_X1, ICON1_Y1, (u8 *)gImage_13_1);
                        ST7789_WriteString(ICON2_X0,ICON2_Y0,"vendor:",Font_11x18, YELLOW, BLACK);
                        ST7789_WriteString(ICON3_X0,ICON3_Y0,"model:",Font_11x18, YELLOW, BLACK);
                        ST7789_WriteString(ICON4_X0,ICON4_Y0,"version:",Font_11x18, YELLOW, BLACK);
                        ST7789_WriteString(ICON5_X0,ICON5_Y0,"sn:",Font_11x18, YELLOW, BLACK);
                        ST7789_WriteString(ICON6_X0,ICON6_Y0,"capacity:",Font_11x18, YELLOW, BLACK);
                        if(sd_flag==0)
                        {
                            ST7789_WriteString2(XY_NOTICE_SD,"No SD card",Font_11x18, YELLOW, RED, 1);
                        }
                        else
                        {                            
                            u32 total,free;
                            exf_getfree("0",&total,&free);
                            sprintf((char*)tbuf,"total:%dM",total>>10);
                            ST7789_WriteString(ICON7_X0,ICON7_Y0,(char*)tbuf,Font_11x18, YELLOW, BLACK);
                            sprintf((char*)tbuf,"free:%dM",free>>10);
                            ST7789_WriteString(ICON8_X0,ICON8_Y0,(char*)tbuf,Font_11x18, YELLOW, BLACK);
                        }
                        break; 
            
                    default:
                        break;
                }                   
            }            
            #endif
        #endif         
        }
        #endif 

        if(cal_mode2_flag)
        {
            if((keyok==2)&&(keycnt==4))
            {                
                sprintf((char*)pTempStr,"Ta1=%d",measureParam120.realTimeTs);
                ST7789_WriteString(ICON8_X0,ICON8_Y0,(char*)pTempStr,Font_11x18, BLUE, BLACK);                
                sprintf((char*)pTempStr,"Ta2=%d",measureParam120.realTimeTfpa);
                ST7789_WriteString(ICON8_X0+110,ICON8_Y0,(char*)pTempStr,Font_11x18, BLUE, BLACK);
                sprintf((char*)pTempStr,"Ta3=%d",measureParam120.realTimeTlen);
                ST7789_WriteString(ICON8_X0+220,ICON8_Y0,(char*)pTempStr,Font_11x18, BLUE, BLACK);

                // sprintf((char*)pTempStr,"To1=%.1f",IRInfo120.MinValue);
                // ST7789_WriteString(ICON7_X0,ICON7_Y0,(char*)pTempStr,Font_11x18, BLUE, BLACK);                
                // sprintf((char*)pTempStr,"To2=%.1f",IRInfo120.CursorValue);
                // ST7789_WriteString(ICON7_X0+110,ICON7_Y0,(char*)pTempStr,Font_11x18, BLUE, BLACK);
                // sprintf((char*)pTempStr,"To3=%.1f",IRInfo120.MaxValue);
                // ST7789_WriteString(ICON7_X0+220,ICON7_Y0,(char*)pTempStr,Font_11x18, BLUE, BLACK);
            }            
        }	                                 
    } 
}

void UserInitFlash(void)
{
    W25QXX_Init(); 
    my_mem_init(SRAMIN);       		
    exfuns_init();
    f_mount(fs[1],"1:",1);
    res=f_mkdir("1:/PARAM");
}

#if SD_EN
void UserInitSD(void)
{       
    if(SD_Init()==SD_OK)
	{  
        my_mem_init(SRAMIN);     
        if(exfuns_init()==0)				 
        {                                 
            f_mount(fs[0],"0:",1);
            sd_flag=1;
            res=f_mkdir("0:/PHOTO");
            if(res!=FR_EXIST&&res!=FR_OK)	                			  
                sd_flag=0;
            else
                UserGetPicName();	 
        }                         
	}    
}
#endif

void UserCheckSD(void)
{
    if(sd_timecnt>1000)
    {
        sd_timecnt=0;

        if(sd_flag==0)
            UserInitSD();
        else
        {
            if(SD_GetState()==SD_CARD_ERROR)            
                sd_flag=0;
        }
    }  
}

void UserCheckUsb(void)
{
    u8 offline_cnt=0;
	u8 tct=0;
	u8 USB_STA;
	u8 Divece_STA;

	{					  
		if(USB_STA!=USB_STATUS_REG)//状态改变了 
		{	 						   
			// // LCD_Fill(30,210,240,210+16,WHITE);//清除显示			  	   
			// if(USB_STATUS_REG&0x01)//正在写		  
			// {
			// 	// LED1(0);
			// 	// LCD_ShowString(30,210,200,16,16,"USB Writing...");//提示USB正在写入数据	 
			// }
			// if(USB_STATUS_REG&0x02)//正在读
			// {
			// 	// LED1(0);
			// 	// LCD_ShowString(30,210,200,16,16,"USB Reading...");//提示USB正在读出数据  		 
			// }	 										  
			// if(USB_STATUS_REG&0x04)//写入错误
            //     LCD_ShowString(30,230,200,16,16,"USB Write Err ");//提示写入错误
			// else 
            //     LCD_Fill(30,230,240,230+16,WHITE);//清除显示	  
			// if(USB_STATUS_REG&0x08)//读出错误
            //     LCD_ShowString(30,250,200,16,16,"USB Read  Err ");//提示读出错误
			// else 
            //     LCD_Fill(30,250,240,250+16,WHITE);//清除显示

			USB_STA=USB_STATUS_REG;//记录最后的状态
		}
		if(Divece_STA!=bDeviceState) 
		{
			// if(bDeviceState==1)
            //     LCD_ShowString(30,190,200,16,16,"USB Connected    ");//提示USB连接已经建立
			// else 
            //     LCD_ShowString(30,190,200,16,16,"USB DisConnected ");//提示USB被拔出了

			Divece_STA=bDeviceState;
		}
		tct++;
		if(tct==200)
		{
			tct=0;			
			if(USB_STATUS_REG&0x10)
			{
				offline_cnt=0;//USB连接了,则清除offline计数器
				bDeviceState=1;//===不可拍照 不可浏览图片
			}else//没有得到轮询 
			{
				offline_cnt++;  
				if(offline_cnt>10)
                    bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
			}
			USB_STATUS_REG=0;
		}
	}   
}

void UserInitData(void)
{
    palette_select_cnt=2;
    center_flag=1;
    maxmin_flag=1;
    unit_select_cnt=0;
    emiss_select_cnt=99;
    degree_select_cnt=0;
    backlight_select_cnt=1;
    language_select_cnt=0;
    
    // data12h_select_cnt=0;//===0>>24H 1>>12H
    // year_select_cnt=23;
    // month_select_cnt=1;
    // day_select_cnt=5;
    // hour_select_cnt=8;
    // min_select_cnt=0;
    // pm_flag=0;
    // UserSetRTC();

    autooff_select_cnt=0;
    sleep_en_flag=AUTO_SLEEP_ON;
    sleep_time1scnt=AUTO_SLEEP_5MIN;

    measureParam120.emiss=emiss_select_cnt;
    measureParam120.realTimeTfpa=2780;
    measureParam120.realTimeTlen=3200;
    measureParam120.realTimeTs=3300;

    // bat_level=4;
    UserParamRead(0);
  
    Para120.AutoShutterInterval=300;

    // cal_mode1_flag=1;
    // cal_mode2_flag=1;        
}

void UserCheckSleep(void)
{
    switch(autooff_select_cnt)
    {
        case 0:
            sleep_en_flag=AUTO_SLEEP_ON;
            sleep_time1scnt=AUTO_SLEEP_5MIN;            
            break;
        case 1:
            sleep_en_flag=AUTO_SLEEP_ON;
            sleep_time1scnt=AUTO_SLEEP_15MIN;            
            break;
        case 2:
            sleep_en_flag=AUTO_SLEEP_ON;
            sleep_time1scnt=AUTO_SLEEP_30MIN;
            break;
        case 3:
            sleep_en_flag=AUTO_SLEEP_OFF;
            break;
        default:
            break;
    }
}

void UserGetRTC(void)
{
    RTC_Get_Time(&hour,&min,&sec,&ampm);                                    	
    RTC_Get_Date(&year,&month,&date,&week);
              
    year_select_cnt=year;
    month_select_cnt=month;
    day_select_cnt=date;
    hour_select_cnt=hour;
    min_select_cnt=min;
    sec_select_cnt=sec;
    pm_flag=ampm;

    if(data12h_select_cnt==0)
    {
        if(hour_select_cnt==12) 
        {
            if(pm_flag==0)
                hour_select_cnt=0;
        } 
        else
        {    
            if(pm_flag)
                hour_select_cnt+=12;
        }
    }   
}

void UserSetRTC(void)
{
    year=year_select_cnt;
    month=month_select_cnt;
    date=day_select_cnt;
    hour=hour_select_cnt;
    min=min_select_cnt;
    ampm=pm_flag;
    sec=0;

    if(data12h_select_cnt==0)
    {
        if(hour>12)
            hour-=12;
        if(hour==0)
            hour=12;

        if(pm_flag==0)
            RTC_Set_Time(hour,min,sec,RTC_HOURFORMAT12_AM);
        else
            RTC_Set_Time(hour,min,sec,RTC_HOURFORMAT12_PM);
    }
    else
    {
        if(pm_flag==0)
            RTC_Set_Time(hour,min,sec,RTC_HOURFORMAT12_AM);
        else
            RTC_Set_Time(hour,min,sec,RTC_HOURFORMAT12_PM);
    }                                   	
    RTC_Set_Date(year,month,date,getWeek(year,month,date));
}

void UserGetPicName(void)
{
    DIR picdir;
    FILINFO *picfileinfo;
    u8 *ptemp_str;

    #if RTOS_EN
    vTaskSuspendAll();
    #endif

    picfileinfo=(FILINFO*)mymalloc(SRAMIN,sizeof(FILINFO));	//申请内存
    ptemp_str=(u8 *)mymalloc(SRAMIN,20);

    res=f_opendir(&picdir,"0:/PHOTO"); //打开目录
	if(res==FR_OK)
    {
        pic_num=0;
		while(1)//全部查询一遍
		{
	        res=f_readdir(&picdir,picfileinfo);//读取目录下的一个文件
	        if(res!=FR_OK||picfileinfo->fname[0]==0)
                break;	//错误了/到末尾了,退出	 	 
			res=f_typetell((u8*)picfileinfo->fname);	
			if((res&0XF0)==0X50)//取高四位,看看是不是图片文件	
			{
                sscanf((const char *)picfileinfo->fname, "%8s",ptemp_str);
                sscanf((const char *)(ptemp_str+3),"%d",&pic_name);//===%d %u必须int!!!
                pic_num++;
				pic_index_save[pic_num]=pic_name;			
                if(pic_num>PIC_SAVE_MAX)
                    pic_num=PIC_SAVE_MAX;
			}	    
		} 
        f_closedir(&picdir);         
    }  
    myfree(SRAMIN,picfileinfo);
    myfree(SRAMIN,ptemp_str);

    #if RTOS_EN
    xTaskResumeAll();
    #endif  
}

#if KEY_PIC_EN //拍照
void UserSavePicName(void)
{
    if(sd_flag)
    {
        if(picsave_flag)
        {                                                          
            ST7789_WriteString2(XY_SAVE,"save",Font_11x18, YELLOW, RED, 1);
            ST7789_WriteString2(XY_QUIT,"quit",Font_11x18, YELLOW, RED, 1);                          
        }
        if(picsaveok_flag)
        {
            picsaveok_flag=0;
            picsave_flag=0;
            #if RTOS_EN
            vTaskSuspendAll();
            #endif
            pname=pic_memalloc(30);	 
            pic_name=create_pathname(pname);                    
                                                
            res=bmp_encode2(pname,0,0,320,240,1,dst_rgb);
            if(res==FR_OK)
            {                         
                pic_num++;
                pic_index_save[pic_num]=pic_name;
                if(pic_num>PIC_SAVE_MAX)
                    pic_num=PIC_SAVE_MAX;
                // //===save
                // pic_index_save[0]=pic_num;//===总数
                // STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)pic_index_save,SIZE);
                // STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)pic_index_save,SIZE);
                // //+++                      
            }                                      
            pic_memfree(pname);
            #if RTOS_EN
            xTaskResumeAll();
            #endif                                     
        }                
    }            
}
#endif

void UserCheckBacklight(void)
{
    static u8 temp=0;
    if(temp!=backlight_select_cnt)//===变化执行一次!!!
    {
        switch(backlight_select_cnt)
        {
            case 0:           
                TIM_SetTIM4Compare2(50);
                break;
            case 1:
                TIM_SetTIM4Compare2(150);
                break;
            case 2:
                TIM_SetTIM4Compare2(350);
                break;
            default:
                break;            
        }
        temp=backlight_select_cnt;
    }
}

void UserPowerOff(void)
{
    TIM_SetTIM4Compare2(0);//===关背光
    keyok=99;//===关图像
    shutter_id=99;//===关按键
    HAL_GPIO_WritePin(GPIOE, POWE_EN_Pin, GPIO_PIN_RESET);  
}

void UserPowerOn(void)
{
    TIM_SetTIM4Compare2(150);//===开背光
    keyok=0;//===开图像
    shutter_id=0;//===开按键
    UserInitData();
    HAL_GPIO_WritePin(GPIOE, POWE_EN_Pin, GPIO_PIN_SET); 
}

void UserCheckPowerOnOff(void)//===中断中检测 必须按键初始化之后!!!
{
    static u8 key5_up=0;
    static u8 temp=0;		
    if((key5_up==1)&&(KEY5==1))
    {
        if(key_timecnt==0)
        {            
            temp=~temp;
            if(temp)
                UserPowerOff();
            else
                UserPowerOn();
            key5_up=0;                    	                    
        }
    }
    else if(KEY5==0)
    {
        key5_up=1;
        key_timecnt=2000;
    }
}

void UserParamWrite(u8 w_d_flag)
{
    //===L
    if(measureParam120.fliter[0].kf>=0)
        buf_param[0]=0;
    else
        buf_param[0]=1;
    if(measureParam120.fliter[0].kf_2>=0)
        buf_param[1]=0;
    else
        buf_param[1]=1;
    if(measureParam120.fliter[0].b>=0)
        buf_param[2]=0;
    else
        buf_param[2]=1;
    if(measureParam120.fliter[0].b2>=0)
        buf_param[3]=0;
    else
        buf_param[3]=1;

    buf_param[4]=ABS(measureParam120.fliter[0].kf);
    buf_param[5]=ABS(measureParam120.fliter[0].kf_2);
    buf_param[6]=ABS(measureParam120.fliter[0].b);
    buf_param[7]=ABS(measureParam120.fliter[0].b2);

    //===H
    if(measureParam120.fliter[1].kf>=0)
        buf_param[8]=0;
    else
        buf_param[8]=1;
    if(measureParam120.fliter[1].kf_2>=0)
        buf_param[9]=0;
    else
        buf_param[9]=1;
    if(measureParam120.fliter[1].b>=0)
        buf_param[10]=0;
    else
        buf_param[10]=1;
    if(measureParam120.fliter[1].b2>=0)
        buf_param[11]=0;
    else
        buf_param[11]=1;

    buf_param[12]=ABS(measureParam120.fliter[1].kf);
    buf_param[13]=ABS(measureParam120.fliter[1].kf_2);
    buf_param[14]=ABS(measureParam120.fliter[1].b);
    buf_param[15]=ABS(measureParam120.fliter[1].b2);

    if(w_d_flag)
        buf_param[16]=measureParam120.distance;

    STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)buf_param,SIZE);    
}

void UserParamRead(u8 r_d_flag)
{    
    STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)buf_param,SIZE);
    #if 0
    if(1)
    {
        measureParam120.fliter[0].kf=11200;
        measureParam120.fliter[0].kf_2=11200;
        measureParam120.fliter[0].b=0;       
        measureParam120.fliter[0].b2=0;
        measureParam120.fliter[1].kf=12900;
        measureParam120.fliter[1].kf_2=12900;
        measureParam120.fliter[1].b=0;       
        measureParam120.fliter[1].b2=0;       
        measureParam120.distance=5;
    }
    #else
    if(buf_param[0]==0xffffffff)
    {        
        measureParam120.fliter[0].kf=10000;
        measureParam120.fliter[0].kf_2=10000;
        measureParam120.fliter[0].b=0;       
        measureParam120.fliter[0].b2=0;
        measureParam120.fliter[1].kf=10000;
        measureParam120.fliter[1].kf_2=10000;
        measureParam120.fliter[1].b=0;       
        measureParam120.fliter[1].b2=0;       
        measureParam120.distance=5;
        UserParamWrite(0);//===UserParamWrite(1);
    }
    #endif
    else
    {
        //===L
        if(buf_param[0]==0)
            measureParam120.fliter[0].kf=buf_param[4];
        else 
            measureParam120.fliter[0].kf=-buf_param[4];
        if(buf_param[1]==0)
            measureParam120.fliter[0].kf_2=buf_param[5];
        else 
            measureParam120.fliter[0].kf_2=-buf_param[5];
        if(buf_param[2]==0)
            measureParam120.fliter[0].b=buf_param[6];
        else 
            measureParam120.fliter[0].b=-buf_param[6];
        if(buf_param[3]==0)
            measureParam120.fliter[0].b2=buf_param[7];
        else 
            measureParam120.fliter[0].b2=-buf_param[7];   

        //===H
        if(buf_param[8]==0)
            measureParam120.fliter[1].kf=buf_param[12];
        else 
            measureParam120.fliter[1].kf=-buf_param[12];
        if(buf_param[9]==0)
            measureParam120.fliter[1].kf_2=buf_param[13];
        else 
            measureParam120.fliter[1].kf_2=-buf_param[13];
        if(buf_param[10]==0)
            measureParam120.fliter[1].b=buf_param[14];
        else 
            measureParam120.fliter[1].b=-buf_param[14];
        if(buf_param[11]==0)
            measureParam120.fliter[1].b2=buf_param[15];
        else 
            measureParam120.fliter[1].b2=-buf_param[15];

        if(r_d_flag)
            measureParam120.distance=buf_param[16];
    }    
}

#if ADC_EN
void Get_VTEMP_Average(short temp_value,u8 times,short *to_avg)
{
	static short temp_val=0;
    static short temp_avg=0;
    static int temp_sum=0;
    static short temp_max=0;
    static short temp_min=0;
	static u8 t=0;

    temp_val=temp_value;
    temp_sum+=temp_val;

    if(t==0)
    {
        temp_max=temp_val;
        temp_min=temp_val;
    }
    if(temp_val>temp_max)
    {
        temp_max=temp_val;                            
    }
    if(temp_val<temp_min)
    {
        temp_min=temp_val;                            
    }

    t++;

    if(t>=times)
    {
        temp_avg=(temp_sum-temp_max-temp_min)/(times-2);
        *to_avg=temp_avg+COMP_VTEMP;

        temp_val=0;
        temp_avg=0;
        temp_sum=0;
        temp_max=0;
        temp_min=0;
        t=0;
    }
}

void UserCheckADC(void)
{
    if(ad_timecnt>200)
    {        
        switch (adc_ch)
        {
            case NTC_LENS:
                ad=Get_Adc3_Average(10,10);            
                vol=ad*3.3/65536;
                Rt0=12000*vol/(3.6f-vol);
                Rt=Rp*Rt0/(Rp-Rt0);
                T1=1/(1/T2+log(Rt/Rp2)/Bx)-Ka+0.5;
                temp_value=(short)(T1*100);
                if(temp_value >= -2000 && temp_value <= 7000)
                {
                    measureParam120.realTimeTlen = temp_value;
                }                
                break;            
            case NTC_SHUTTER:
                ad=Get_Adc_Average(16,10);                
                vol=ad*3.3/65536;
                Rt0=12000*vol/(3.6f-vol);
                Rt=Rp*Rt0/(Rp-Rt0);
                T1=1/(1/T2+log(Rt/Rp2)/Bx)-Ka+0.5;
                temp_value=(short)(T1*100);                
                if(temp_value >= -2000 && temp_value <= 7000)
                {
                    measureParam120.realTimeTs = temp_value;
                }				                                
                break;            
            case NTC_VTEMP:
                ad=Get_Adc_Average(4,10);              
                temp_value = 37129-2.01f*(ad*ad_data);
                if(temp_value >= -2000 && temp_value <= 7000)
                {
                    // measureParam120.realTimeTfpa = temp_value;
                    Get_VTEMP_Average(temp_value,10,&measureParam120.realTimeTfpa);//===1s*10
                }                                	
                break; 
            case NTC_BAT:
                ad=Get_Adc_Average(15,10);                         
                    
                break;
            case AD_BAT:
                ad=Get_Adc3_Average(0,10);              
                //===4/2 3.6/2 3.2/2 3.0/2            
                if(ad>39781)
                    bat_level=4;
                else if(ad>35747)
                    bat_level=3;
                else if(ad>31775)
                    bat_level=2;
                else if(ad>29789)
                    bat_level=1;
                else
                    bat_level=0;                	
                break;
            default:
                break;       
        }

        adc_ch++;
        adc_ch=adc_ch%NTC_NONE;

        ad_timecnt=0;                   
    }  
}
#endif

void UserCheckDegree(void)
{
    static u8 degree_cnt=0;

    switch(degree_select_cnt)
    {
        case 0:
            if(measureParam120.MeasureDegree==2)
            {                
                degree_cnt=0;
                measureParam120.MeasureDegree=1;
                measureParam120.MeasureRealGear=1;
                SetMeasureDegree(1);
            }
            break;
        case 1:
            if(measureParam120.MeasureDegree==1)
            {                
                degree_cnt=0;
                measureParam120.MeasureDegree=2;
                measureParam120.MeasureRealGear=2;
                SetMeasureDegree(2);
            }
            break;
        case 2:
            if((IRInfo120.MaxValue>150)&&(measureParam120.MeasureDegree==1))
            {
                degree_cnt++;
                if(degree_cnt>10)
                {
                    degree_cnt=0;
                    measureParam120.MeasureDegree=2;
                    measureParam120.MeasureRealGear=2;
                    SetMeasureDegree(2);
                }
            }
            else if((IRInfo120.MaxValue<=130)&&(measureParam120.MeasureDegree==2))
            {
                degree_cnt++;
                if(degree_cnt>10)
                {
                    degree_cnt=0;
                    measureParam120.MeasureDegree=1;
                    measureParam120.MeasureRealGear=1;
                    SetMeasureDegree(1);
                }
            }
            break;
        default:
            break;        
    }
}

u8 UserReadParam(u8 *data,u32 ReadAddr,u16 NumByteToRead,u32 Degree)
{
    FIL* f_bmp;
    u16 br;		   
	u8 res;
    u8 *ptemp;    

    ptemp=data;
		
    if(Degree == 1)
    {
        f_bmp=(FIL *)pic_memalloc(sizeof(FIL));
		res=f_open(f_bmp,"1:/PARAM/Low.raw",FA_READ);            
        if(res==0x04)
            res=f_open(f_bmp,"1:/PARAM/Low.raw",FA_WRITE|FA_CREATE_NEW); 
        if(res==0)
        { 
            f_lseek(f_bmp,ReadAddr);        
            res=f_read(f_bmp,ptemp,NumByteToRead,(UINT*)&br);                   
            f_close(f_bmp);
        }  	
        pic_memfree(f_bmp);
        if(strncmp((const char *)ptemp,"G",1)==0)
            return 0;           
	}
    else if(Degree == 2)
    {
        f_bmp=(FIL *)pic_memalloc(sizeof(FIL));
		res=f_open(f_bmp,"1:/PARAM/High.raw",FA_READ);
        if(res==0x04)
            res=f_open(f_bmp,"1:/PARAM/High.raw",FA_WRITE|FA_CREATE_NEW);
        if(res==0)
        { 
            f_lseek(f_bmp,ReadAddr);        
            res=f_read(f_bmp,ptemp,NumByteToRead,(UINT*)&br);       
            f_close(f_bmp);
        }  	
        pic_memfree(f_bmp);
        if(strncmp((const char *)ptemp,"G",1)==0)
            return 0;
    }
  						  	
	return 1;
}

#if 0 //ADC_EN
void StartDefaultTask2(void)
{
	// uint8_t chn = 0;
	// short temp_value;     
	// uint8_t average_shutter_start_cnt=0;
	// uint16_t g_ad_channel = 0;
    // uint16_t g_ad_readcnt = 0;
	
	// MY_ADC_Init();
    // g_ad_channel = NTC_SHUTTER;
    // Get_Adc_Average(16,10);
	
	// for(;;)
	{	
		if ( g_ad_channel == NTC_NONE )
		{
			chn = (chn + 1) % 3;
			g_ad_channel = chn;
			g_ad_readcnt = 0;						
		}		
        switch ( g_ad_channel )
        {
            case NTC_LENS:
                // ad=Get_Adc3_Average(10,10);//===NTC_LENS 
                ad=Get_Adc_Average(10,10);//===NTC_LENS               
                vol=ad*3.3/65536;
                Rt0=12000*vol/(3.6f-vol);
                Rt=Rp*Rt0/(Rp-Rt0);
                T1=1/(1/T2+log(Rt/Rp2)/Bx)-Ka+0.5;
                temp_value=(short)T1*100;
                if(temp_value >= -2000 && temp_value <= 7000)
                {
                    measureParam120.realTimeTlen = temp_value;
                }                
                break;            
            case NTC_SHUTTER:
                ad=Get_Adc_Average(16,10);//===NTC_SHUTTER
                ad_vaule=ad;                
                vol=ad*3.3/65536;
                Rt0=12000*vol/(3.6f-vol);
                Rt=Rp*Rt0/(Rp-Rt0);
                T1=1/(1/T2+log(Rt/Rp2)/Bx)-Ka+0.5;
                temp_value=(short)T1*100;                
                if(temp_value >= -2000 && temp_value <= 7000)
                {
                    measureParam120.realTimeTs = temp_value;
                }				                                
                break;            
            case NTC_VTEMP:
                ad=Get_Adc_Average(4,10);//===NTC_VTEMP
                ad_vaule=ad;                
                temp_value = 37129-2.01*(ad_vaule*0.40283);
                if(temp_value >= -2000 && temp_value <= 7000)
                {
                    measureParam120.realTimeTfpa = temp_value;
                }                	
                break;
            default:
                break;
        }
        g_ad_channel = NTC_NONE;        
	}
}
#endif

#if 0 //test over!!!
// //===test over!!!
// UBaseType_t StackSize2;
// void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
// {
//     while(1)
//     {
//         u8 i;
//         i++;
//     }
// }
// UBaseType_t StackSize2;
// extern osThreadId irCameraTaskHandle;  
// StackSize2=uxTaskGetStackHighWaterMark(irCameraTaskHandle);
// char pWriteBuffer[500];
// u16 testcnt;
//+++
// //===test 
// testcnt++;
// if(testcnt>100)
// {
//     testcnt=0;
//     extern osThreadId irCameraTaskHandle;  
//     StackSize2=uxTaskGetStackHighWaterMark(irCameraTaskHandle);
//     vTaskList((char *)&pWriteBuffer);
//     printf("task_name   task_state  priority   stack  tasK_num\n");
//     printf("%s\n", pWriteBuffer);
// }
// //+++
#endif

// void UserSaveData(void)
// {
//     STMFLASH_Write(FLASH_SAVE_ADDR,(u32*)TEXT_Buffer,SIZE);
//     STMFLASH_Read(FLASH_SAVE_ADDR,(u32*)datatemp,SIZE);
// }

//=======================CAL
u8 key_cnt_cal;
u8 key_ok_cal;
u8 key_ok_cal2;
u8 auto_cnt_cal;
u8 auto_cnt_cal2;
short h_kf_temp;
short h_kf2_temp;
short l_kf_temp;
short l_kf2_temp;
//+++++++++++++++++++++++

//==============================CAL
void UserDisplayCal2(void);
void UserCheckKey(void);
void UserAutoCal(void);
//++++++++++++++++++++++++++++++

#define AUTO_DELAY_1    (10/2)
#define AUTO_DELAY_2    (3*2)

#define AUTO_CAL_1      100
#define AUTO_CAL_2      200
#define AUTO_CAL_3      100
#define AUTO_CAL_4      200

#define AUTO_CAL_RANGE  0.5
#define AUTO_CAL_RANGE2 9000

void UserDisplayCal(void)
{
    if(shutter_id==IDLE)
    {   
        if((keyok==0)&&(picsave_flag==0))	
        {                    
            LinearDrtInterface(y8_buff, (uint16_t *)pImage);	
			// Y16toY8(y8_buff,pImage);	
            Y8MedianFilter(y8_buff_filter,y8_buff);  
            // Y8AverageFilter(y8_buff,y8_buff_filter);		        
            Y8toRGB((uint16_t *)pImage,y8_buff_filter,palette_select_cnt);                        	
            image120x90to320x240((u8 *)pImage,(u8 *)dst_rgb);                                               
             		                                           
            sprintf((char*)pTempStr,"E:0.%d",emiss_select_cnt);
            ST7789_WriteString3(XY_EMISS,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);

            UserGetRTC();
            if(data12h_select_cnt)
            {
                if(pm_flag)
                    sprintf((char*)pTempStr,"%02d:%02d:%02dPM",hour_select_cnt,min_select_cnt,sec_select_cnt);
                else
                    sprintf((char*)pTempStr,"%02d:%02d:%02dAM",hour_select_cnt,min_select_cnt,sec_select_cnt);
            }
            else
                sprintf((char*)pTempStr,"%02d:%02d:%02d",hour_select_cnt,min_select_cnt,sec_select_cnt); 			
            ST7789_WriteString3(XY_RTC,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);                        

            if(sd_flag)
                ST7789_WriteString3(XY_SD,"SD",Font_11x18, YELLOW, RED, dst_rgb, 0);
            else 
                ST7789_WriteString3(XY_SD,"SD",Font_11x18, LGRAY, RED, dst_rgb, 0);            

            if(unit_select_cnt==0) 			
                ST7789_WriteString3(XY_UNIT,"C",Font_11x18, YELLOW, RED, dst_rgb, 0);
            else
                ST7789_WriteString3(XY_UNIT,"F",Font_11x18, YELLOW, RED, dst_rgb, 0);
            
            if(unit_select_cnt==0)
            {			
                sprintf((char*)pTempStr,"H:%.1f",IRInfo120.MaxValue);
                ST7789_WriteString3(XY_MAX,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0); 
                sprintf((char*)pTempStr,"L:%.1f",IRInfo120.MinValue);
                ST7789_WriteString3(XY_MIN,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);
            } 
            else
            {
                sprintf((char*)pTempStr,"H:%.1f",IRInfo120.MaxValue*9/5+32);
                ST7789_WriteString3(XY_MAX,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0); 
                sprintf((char*)pTempStr,"L:%.1f",IRInfo120.MinValue*9/5+32);
                ST7789_WriteString3(XY_MIN,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);
            }

            ST7789_Fill2(XY0_PALETTE, XY1_PALETTE, palette_select_cnt, dst_rgb);

            if(center_flag)
            {
                if(unit_select_cnt==0) 			
                    sprintf((char*)pTempStr,"C:%.1f",IRInfo120.CursorValue);
                else
                    sprintf((char*)pTempStr,"C:%.1f",IRInfo120.CursorValue*9/5+32);                                

                ST7789_WriteString3(XY_CURSOR,(const TCHAR*)pTempStr,Font_11x18, YELLOW, RED, dst_rgb, 0);
                ST7789_DrawCircle2(160,120,5,YELLOW, dst_rgb);
                ST7789_DrawCircle2(160,120,4,YELLOW, dst_rgb);
                ST7789_DrawCircle2(160,120,3,YELLOW, dst_rgb);
                ST7789_DrawLine2(160-5-5,120,160-5,120,YELLOW, dst_rgb);
                ST7789_DrawLine2(160+5,120,160+5+5,120,YELLOW, dst_rgb);
                ST7789_DrawLine2(160,120-5-5,160,120-5,YELLOW, dst_rgb);
                ST7789_DrawLine2(160,120+5,160,120+5+5,YELLOW, dst_rgb);	
            }
            if(maxmin_flag)
            {                                                    
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
                ST7789_WriteString3(tempX,tempY,"L",Font_11x18, YELLOW, RED, dst_rgb, 0);
                tempX=IRInfo120.PostXMax*320/120;
                tempY=IRInfo120.PostYMax*320/120;
                tempX=(tempX<tempMIN ? tempMIN : tempX);
                tempX=(tempX>tempMAX ? tempMAX : tempX);
                tempY=(tempY<tempMIN ? tempMIN :tempY);
                tempY=(tempY>tempMAX ? tempMAX : tempY);
                ST7789_WriteString3(tempX,tempY,"H",Font_11x18, YELLOW, RED, dst_rgb, 0); 
            } 
            
            if(bDeviceState)
                ST7789_WriteString3(XY_USB,"U",Font_11x18, YELLOW, RED, dst_rgb, 0);

            if(usb_flag)
                ST7789_WriteString3(XY_BAT,"BAT5",Font_11x18, YELLOW, RED, dst_rgb, 0);       
            else
            {
                switch(bat_level)
                {                    
                    case 4:
                        ST7789_WriteString3(XY_BAT,"BAT4",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 3:
                        ST7789_WriteString3(XY_BAT,"BAT3",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 2:
                        ST7789_WriteString3(XY_BAT,"BAT2",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 1:
                        ST7789_WriteString3(XY_BAT,"BAT1",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    case 0:
                        ST7789_WriteString3(XY_BAT,"BAT0",Font_11x18, YELLOW, RED, dst_rgb, 0);
                        break;
                    default:
                        break;
                }
            } 

            UserDisplayCal2();
            //===BAT
            minibmp_decode2("1:/ICON/BAT.bmp",XY_BAT2,320,240, RED, dst_rgb, 0);
            ST7789_DrawImage2(0, 0, LCD_W, LCD_H, (uint16_t *)dst_rgb);                                                                                                                                                        
        }     	                                 

        // HAL_Delay(20);//===!!!
        // UserDisplayCal2();        
        UserCheckKey();
        UserAutoCal();
    } 
}

void UserDisplayCal2(void)
{
    ST7789_WriteString3(XY_CAL,"CAL MODE SEL:",Font_11x18, LIGHTBLUE, LGRAY, dst_rgb, 0);
    //===0.5M
    if(key_cnt_cal==0)
        ST7789_WriteString3(XY_100,"T1 100 SEL",Font_11x18, RED, LGRAY, dst_rgb, 0);
    else
        ST7789_WriteString3(XY_100,"T1 100",Font_11x18, LIGHTBLUE, RED, dst_rgb, 0);
    if(key_cnt_cal==1)    
        ST7789_WriteString3(XY_200,"T2 200 SEL",Font_11x18, RED, LGRAY, dst_rgb, 0);
    else
        ST7789_WriteString3(XY_200,"T2 200",Font_11x18, LIGHTBLUE, RED, dst_rgb, 0);
    //===1.2M
    if(key_cnt_cal==2)
        ST7789_WriteString3(XY2_100,"T3 100 SEL",Font_11x18, RED, LGRAY, dst_rgb, 0);
    else
        ST7789_WriteString3(XY2_100,"T3 100",Font_11x18, LIGHTBLUE, RED, dst_rgb, 0);
    // if(key_cnt_cal==3)    
    //     ST7789_WriteString3(XY2_200,"T4 200 SEL",Font_11x18, RED, LGRAY, dst_rgb, 0);
    // else
    //     ST7789_WriteString3(XY2_200,"T4 200",Font_11x18, LIGHTBLUE, RED, dst_rgb, 0);    
    
    switch(key_cnt_cal)
    {
        case 0:
            if(key_ok_cal)
            {
                sprintf((char*)pTempStr,"K1=%d 0.5M CAL...",measureParam120.fliter[0].kf);
                ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, RED, LGRAY, dst_rgb, 0);
            }
            else
            {
                if(key_ok_cal2==1)
                    sprintf((char*)pTempStr,"K1=%d 0.5M CAL OK",measureParam120.fliter[0].kf);
                else
                    sprintf((char*)pTempStr,"K1=%d 0.5M CAL",measureParam120.fliter[0].kf);
                ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, LIGHTBLUE, LGRAY, dst_rgb, 0); 
            }                        
            break;
        case 1:
            if(key_ok_cal)
            {
                sprintf((char*)pTempStr,"K2=%d 0.5M CAL...",measureParam120.fliter[1].kf);
                ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, RED, LGRAY, dst_rgb, 0);
            }
            else
            {
                if(key_ok_cal2==1)
                    sprintf((char*)pTempStr,"K2=%d 0.5M CAL OK",measureParam120.fliter[1].kf);
                else
                    sprintf((char*)pTempStr,"K2=%d 0.5M CAL",measureParam120.fliter[1].kf);
                ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, LIGHTBLUE, LGRAY, dst_rgb, 0); 
            }                           
            break;
        case 2:
            if(key_ok_cal)
            {
                sprintf((char*)pTempStr,"K3=%d 1.2M CAL...",measureParam120.fliter[0].kf_2);
                ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, RED, LGRAY, dst_rgb, 0);
            }
            else
            {
                if(key_ok_cal2==1)
                    sprintf((char*)pTempStr,"K3=%d 1.2M CAL OK",measureParam120.fliter[0].kf_2);
                else 
                    sprintf((char*)pTempStr,"K3=%d 1.2M CAL",measureParam120.fliter[0].kf_2);    
                ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, LIGHTBLUE, LGRAY, dst_rgb, 0);
            }                          
            break;
        // case 3:
        //     if(key_ok_cal)
        //     {                
        //         sprintf((char*)pTempStr,"1.2M K4=%d CAL...",measureParam120.fliter[1].kf_2);            
        //         ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, RED, LGRAY, dst_rgb, 0);
        //     }
        //     else
        //     {
        //         if(key_ok_cal2==1)
        //             sprintf((char*)pTempStr,"1.2M K4=%d CAL OK",measureParam120.fliter[1].kf_2);
        //         else 
        //             sprintf((char*)pTempStr,"1.2M K4=%d CAL NG",measureParam120.fliter[1].kf_2);
        //         ST7789_WriteString3(XY_KF,(const TCHAR*)pTempStr,Font_11x18, LIGHTBLUE, LGRAY, dst_rgb, 0);
        //     }                        
        //     break;
        default:
            break;        
    }    
}

void UserChangeMode(void)
{
    if(key_cnt_cal<2)//===距离
    {
        if(measureParam120.distance==12)
            measureParam120.distance=5;
    }                        
    else    
        if(measureParam120.distance==5)
            measureParam120.distance=12;
    if((key_cnt_cal==0)||(key_cnt_cal==2))//===档位
    {                        
        if(measureParam120.MeasureDegree==2)
        {                                
            measureParam120.MeasureDegree=1;
            measureParam120.MeasureRealGear=1;
            SetMeasureDegree(1);
        } 
    }
    else
    {
        if(measureParam120.MeasureDegree==1)
        {                                
            measureParam120.MeasureDegree=2;
            measureParam120.MeasureRealGear=2;
            SetMeasureDegree(2);
        }                                                      
    }
}

void UserCheckKey(void)
{ 
    key=KEY_Scan(0); 		
    if(key)
    {						   
        switch(key)
        {				 
            case KEY1_OK:
                key_ok_cal2=0;
                if(key_ok_cal)
                    key_ok_cal=0;
                else 
                    key_ok_cal=1;

                if(key_ok_cal==0)
                {
                    measureParam120.fliter[1].kf=h_kf_temp;//===自动调整不满足时 手动返回
                    measureParam120.fliter[1].kf_2=h_kf2_temp;
                    measureParam120.fliter[0].kf=l_kf_temp;
                    measureParam120.fliter[0].kf_2=l_kf2_temp;
                    UserParamWrite(0);                           
                }
                else
                {
                    h_kf_temp=measureParam120.fliter[1].kf;
                    h_kf2_temp=measureParam120.fliter[1].kf_2;
                    l_kf_temp=measureParam120.fliter[0].kf;
                    l_kf2_temp=measureParam120.fliter[0].kf_2;                                                             
                }                
                
                break;						
            case KEY2_DOWN:	
                if(key_ok_cal==0)
                {
                    key_ok_cal2=0;                                        
                    if(key_cnt_cal)
                        key_cnt_cal--;
                    else
                        key_cnt_cal=2;//===key_cnt_cal=3;
                    UserChangeMode();             
                }                            
                break;					
            case KEY3_UP:
                if(key_ok_cal==0)
                {
                    key_ok_cal2=0;
                    key_cnt_cal++;
                    if(key_cnt_cal>2)//===if(key_cnt_cal>3) 
                        key_cnt_cal=0;
                    UserChangeMode();             
                }                                                               
                break;
            case KEY4_BACK:
                shutter_id = NUC_LOOP;                
                break;
            // case KEY5_POWER:                    	                    
            //     break;                
            default:
                break;
        }                 
    }    
}        

void UserAutoCal(void)
{
    auto_cnt_cal++;
    if(auto_cnt_cal<AUTO_DELAY_1)
        return;
    auto_cnt_cal=0;

    if(key_ok_cal==1)
    {
        switch(key_cnt_cal)
        {
            //===0.5M
            case 0:
                if((IRInfo120.MaxValue>AUTO_CAL_1-50)&&(IRInfo120.MaxValue<AUTO_CAL_1+50))
                {
                    if(IRInfo120.MaxValue<(AUTO_CAL_1-AUTO_CAL_RANGE))
                    {
                        if(measureParam120.fliter[0].kf<10000+AUTO_CAL_RANGE2)
                            measureParam120.fliter[0].kf+=100;
                        auto_cnt_cal2=0;       
                    }
                    else if(IRInfo120.MaxValue>(AUTO_CAL_1+AUTO_CAL_RANGE))
                    {
                        if(measureParam120.fliter[0].kf>10000-AUTO_CAL_RANGE2)
                            measureParam120.fliter[0].kf-=100;
                        auto_cnt_cal2=0;
                    }
                    else
                    {
                        auto_cnt_cal2++;
                        if(auto_cnt_cal2>AUTO_DELAY_2)
                        {
                            auto_cnt_cal2=0;
                            key_ok_cal=0;
                            key_ok_cal2=1;
                            UserParamWrite(0);
                        }                        
                    }
                }
                break;
            case 1:
                if((IRInfo120.MaxValue>AUTO_CAL_2-50)&&(IRInfo120.MaxValue<AUTO_CAL_2+50))
                {
                    if(IRInfo120.MaxValue<(AUTO_CAL_2-AUTO_CAL_RANGE))
                    {
                        if(measureParam120.fliter[1].kf<10000+AUTO_CAL_RANGE2)
                            measureParam120.fliter[1].kf+=100;
                        auto_cnt_cal2=0;       
                    }
                    else if(IRInfo120.MaxValue>(AUTO_CAL_2+AUTO_CAL_RANGE))
                    {
                        if(measureParam120.fliter[1].kf>10000-AUTO_CAL_RANGE2)
                            measureParam120.fliter[1].kf-=100;
                        auto_cnt_cal2=0;
                    }
                    else
                    {
                        auto_cnt_cal2++;
                        if(auto_cnt_cal2>AUTO_DELAY_2)
                        {
                            auto_cnt_cal2=0;
                            key_ok_cal=0;
                            key_ok_cal2=1;
                            UserParamWrite(0);
                        }                                                
                    }
                }
                break;
            //===1.2M
            case 2:
                if((IRInfo120.MaxValue>AUTO_CAL_3-50)&&(IRInfo120.MaxValue<AUTO_CAL_3+50))
                {
                    if(IRInfo120.MaxValue<(AUTO_CAL_3-AUTO_CAL_RANGE))
                    {
                        if(measureParam120.fliter[0].kf_2<10000+AUTO_CAL_RANGE2)
                            measureParam120.fliter[0].kf_2+=100;
                        auto_cnt_cal2=0;       
                    }
                    else if(IRInfo120.MaxValue>(AUTO_CAL_3+AUTO_CAL_RANGE))
                    {
                        if(measureParam120.fliter[0].kf_2>10000-AUTO_CAL_RANGE2)
                            measureParam120.fliter[0].kf_2-=100;
                        auto_cnt_cal2=0;
                    }
                    else
                    {
                        auto_cnt_cal2++;
                        if(auto_cnt_cal2>AUTO_DELAY_2)
                        {
                            auto_cnt_cal2=0;
                            key_ok_cal=0;
                            key_ok_cal2=1;
                            UserParamWrite(0);
                        }                        
                    }
                }
                break;
            // case 3:
            //     if((IRInfo120.MaxValue>AUTO_CAL_4-50)&&(IRInfo120.MaxValue<AUTO_CAL_4+50))
            //     {
            //         if(IRInfo120.MaxValue<(AUTO_CAL_4-AUTO_CAL_RANGE))
            //         {
            //             if(measureParam120.fliter[1].kf_2<10000+AUTO_CAL_RANGE2)
            //                 measureParam120.fliter[1].kf_2+=100;
            //             auto_cnt_cal2=0;       
            //         }
            //         else if(IRInfo120.MaxValue>(AUTO_CAL_4+AUTO_CAL_RANGE))
            //         {
            //             if(measureParam120.fliter[1].kf_2>10000-AUTO_CAL_RANGE2)
            //                 measureParam120.fliter[1].kf_2-=100;
            //             auto_cnt_cal2=0;
            //         }
            //         else
            //         {
            //             auto_cnt_cal2++;
            //             if(auto_cnt_cal2>AUTO_DELAY_2)
            //             {
            //                 auto_cnt_cal2=0;
            //                 key_ok_cal=0;
            //                 key_ok_cal2=1;
            //                 UserParamWrite(0);
            //             }                                                
            //         }
            //     }
            //     break;            
            default:
                break;
        }
    }
}

void UserCheckInCal(void)
{    
    if(power_on_flag)
    {   
        key3_temp1=KEY3;
        if(key3_temp2!=key3_temp1)
        {
            key3_temp2=key3_temp1;
            if(key3_temp1==1)
            {
                key3_cnt++;
            }
        }
    }
    else
    {
        if(key3_cnt==3)
        {
            cal_mode1_flag=1;
        }
    }
}

void UserCheckInCal2(void)
{    
    if(power_on_flag)
    {   
        key4_temp1=KEY4;
        if(key4_temp2!=key4_temp1)
        {
            key4_temp2=key4_temp1;
            if(key4_temp1==1)
            {
                key4_cnt++;
            }
        }
    }
    else
    {
        if(key4_cnt==4)
        {
            cal_mode2_flag=1;
        }
    }
}


































