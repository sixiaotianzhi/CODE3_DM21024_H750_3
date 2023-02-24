#include "stm32h7xx_hal.h"
#include "cmsis_os.h"

#include "ir_def.h"
#include "main.h"
#include "Varible.h"

#if 0 //RTOS_EN

#include "adc.h"
#include "math.h"

extern uint16_t g_param_init_done;
extern uint16_t g_start_tmp_read_done;
uint16_t ad_vaule;
uint64_t ad;


const float Rp=20000.0;//===分压
const float Rp2=10000.0;//===NTC 10K
const float T2=273.15+25;
const float Bx=3380;//3950;
const float Ka=273.15;
float vol=0;
float Rt=0;
float Rt0=0;//===Rt//Rp并联
float T1=0;


void StartDefaultTask(void const * argument)
{
	uint8_t chn = 0;
	short temp_value;     
	uint8_t average_shutter_start_cnt=0;
	uint16_t g_ad_channel = 0;
    uint16_t g_ad_readcnt = 0;
	
    // while ( g_param_init_done == 0 )
	// {
	// 	osDelay(100);
	// }

	MY_ADC_Init();
    g_ad_channel = NTC_SHUTTER;
    Get_Adc_Average(16,10);
	
	// osDelay(100);
	
	for(;;)
	{	
		if ( g_ad_channel == NTC_NONE )
		{
			chn = (chn + 1) % 3;
			g_ad_channel = chn;
			g_ad_readcnt = 0;						
		}
		else
		{
			g_ad_readcnt++;
			if ( ( g_ad_readcnt == 10 ) && ( g_start_tmp_read_done == 0 ) )
			{
				g_start_tmp_read_done = 1;
				pParamStatus->temper_shutter_start = 0;
				pParamStatus1->temper_shutter_start = 0;
				pParamStatus->temper_len_realtime = 0;
				pParamStatus1->temper_len_realtime = 0;
				pParamStatus->temper_shutter_realtime = 0;
				pParamStatus1->temper_shutter_realtime = 0;
				pParamStatus->temper_dectect = 0;
				pParamStatus1->temper_dectect = 0;
			}
		}
        switch ( g_ad_channel )
        {
            case NTC_LENS:
                ad=Get_Adc3_Average(10,10);//===NTC_LENS                
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
                if ( g_start_tmp_read_done == 0 )
                {
                    average_shutter_start_cnt++;
                    if ( average_shutter_start_cnt == 3 )
                    {
                        g_start_tmp_read_done = 1;
                        pParamStatus->temper_shutter_start = ad_vaule;
                        pParamStatus1->temper_shutter_start = ad_vaule;
                    }
                }                
                break;
            
            case NTC_VTEMP:
                ad=Get_Adc_Average(4,50);//===NTC_VTEMP
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

        osDelay(300);         
	}
}
#endif

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// if ( GPIO_Pin == ADC_RDY_Pin )
	// {

    // }
    ;
}

#if 0
extern int ads1115_init(void);
extern int ads1115_convert(uint8_t chn);
extern int ads1115_read(uint16_t *val);

extern uint16_t g_param_init_done;
extern uint16_t g_start_tmp_read_done;

static uint16_t g_ad_channel = 0;
static uint16_t g_ad_readcnt = 0;

void StartDefaultTask(void const * argument)
{
	uint8_t chn = 0;
	
	while ( g_param_init_done == 0 )
	{
		osDelay(100);
	}
	
	/* init code for ADS1115 */
	ads1115_init();
	g_ad_channel = NTC_SHUTTER;
	ads1115_convert(g_ad_channel);
	
	osDelay(500);
	
	g_ad_readcnt = 0;

	for(;;)
  {
		if ( g_ad_channel == NTC_NONE )
		{
			chn = (chn + 1) % 3;
			g_ad_channel = chn;
			ads1115_convert(g_ad_channel);
			g_ad_readcnt = 0;
		}
		else
		{
			g_ad_readcnt++;
			if ( ( g_ad_readcnt == 10 ) && ( g_start_tmp_read_done == 0 ) )
			{
				g_start_tmp_read_done = 1;
				pParamStatus->temper_shutter_start = 0;
				pParamStatus1->temper_shutter_start = 0;
				pParamStatus->temper_len_realtime = 0;
				pParamStatus1->temper_len_realtime = 0;
				pParamStatus->temper_shutter_realtime = 0;
				pParamStatus1->temper_shutter_realtime = 0;
				pParamStatus->temper_dectect = 0;
				pParamStatus1->temper_dectect = 0;
			}
		}
		
		osDelay(333);
		
    osDelay(1);
  }
}
#endif
#if 0
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint16_t ad_vaule;
	uint64_t ad;
	short temp_value;
	
	if ( GPIO_Pin == ADC_RDY_Pin )
	{
		ads1115_read(&ad_vaule);
		
		ad = ad_vaule;
		
		switch ( g_ad_channel )
		{
			case NTC_LENS:
				temp_value = (12736130490197.3+121840.2729*ad*ad-4.0235*ad*ad*ad-1821807621.6914*ad)/1000000000;
				if(temp_value >= -2000 && temp_value <= 7000)
				{
					measureParam120.realTimeTlen = temp_value;
				}
				//measureParam120.realTimeTlen = (12736130490197.3+121840.2729*ad*ad-4.0235*ad*ad*ad-1821807621.6914*ad)/1000000000;
				break;
			
			case NTC_SHUTTER:
				temp_value = (12736130490197.3+121840.2729*ad*ad-4.0235*ad*ad*ad-1821807621.6914*ad)/1000000000; 
				if(temp_value >= -2000 && temp_value <= 7000)
				{
					measureParam120.realTimeTs = temp_value;
				}
			  //measureParam120.realTimeTs =(12736130490197.3+121840.2729*ad*ad-4.0235*ad*ad*ad-1821807621.6914*ad)/1000000000; 
				if ( g_start_tmp_read_done == 0 )
				{
					average_shutter_start_cnt++;
					if ( average_shutter_start_cnt == 3 )
					{
						g_start_tmp_read_done = 1;
						pParamStatus->temper_shutter_start = ad_vaule;
						pParamStatus1->temper_shutter_start = ad_vaule;
					}
				}
				break;
			
			case NTC_VTEMP: 
				temp_value = 37129-2.01*ad_vaule;
				if(temp_value >= -2000 && temp_value <= 7000)
				{
					measureParam120.realTimeTfpa = temp_value;
				}
			  //measureParam120.realTimeTfpa= 37129-2.01*ad_vaule; //*100  20180428	;
				break;
			default:
				break;
		}
		
		g_ad_channel = NTC_NONE;
	}
}
#endif

// short temp_data(void)
// {
//    float Rt=0;
//    float Rp=10000;
//    float T2=273.15+25;
//    float Bx=3950;
//    float Ka=273.15;
//    float vol=0;
//    short temp

//    vol=(float)((Get_Adc_Average(16,10))*(3.3/65536));
//    Rt=vol*10000/(3.6-vol);
//    temp=1/(1/T2+log(Rt/Rp)/Bx)-Ka+0.5;

//    return temp;
// }




