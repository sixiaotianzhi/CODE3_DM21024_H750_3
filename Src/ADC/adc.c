#include "adc.h"
//#include "delay.h"
#include "cmsis_os.h"

ADC_HandleTypeDef ADC1_Handler;//ADC句柄
ADC_HandleTypeDef ADC3_Handler;//ADC句柄

//初始化ADC
//ch: ADC_channels 
//通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void MY_ADC_Init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4分频，ADCCLK=PER_CK/4=64/4=16MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16位模式
    ADC1_Handler.Init.ScanConvMode=DISABLE;                    	//非扫描模式
    ADC1_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//关闭EOC中断
	ADC1_Handler.Init.LowPowerAutoWait=DISABLE;					//自动低功耗关闭				
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;               //关闭连续转换
    ADC1_Handler.Init.NbrOfConversion=1;                        //1个转换在规则序列中 也就是只转换规则序列1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;            //禁止不连续采样模式
    ADC1_Handler.Init.NbrOfDiscConversion=0;                    //不连续采样通道数为0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //软件触发
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
	ADC1_Handler.Init.BoostMode=ENABLE;							//BOOT模式关闭
	ADC1_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//有新的数据的死后直接覆盖掉旧数据
	ADC1_Handler.Init.OversamplingMode=DISABLE;					//过采样关闭
	ADC1_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //规则通道的数据仅仅保存在DR寄存器里面
    HAL_ADC_Init(&ADC1_Handler);                                 //初始化 
	
	HAL_ADCEx_Calibration_Start(&ADC1_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADC校准
//===
    ADC3_Handler.Instance=ADC3;
    ADC3_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4分频，ADCCLK=PER_CK/4=64/4=16MHZ
    ADC3_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16位模式
    ADC3_Handler.Init.ScanConvMode=DISABLE;                    	//非扫描模式
    ADC3_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//关闭EOC中断
	ADC3_Handler.Init.LowPowerAutoWait=DISABLE;					//自动低功耗关闭				
    ADC3_Handler.Init.ContinuousConvMode=DISABLE;               //关闭连续转换
    ADC3_Handler.Init.NbrOfConversion=1;                        //1个转换在规则序列中 也就是只转换规则序列1 
    ADC3_Handler.Init.DiscontinuousConvMode=DISABLE;            //禁止不连续采样模式
    ADC3_Handler.Init.NbrOfDiscConversion=0;                    //不连续采样通道数为0
    ADC3_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //软件触发
    ADC3_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
	ADC3_Handler.Init.BoostMode=ENABLE;							//BOOT模式关闭
	ADC3_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//有新的数据的死后直接覆盖掉旧数据
	ADC3_Handler.Init.OversamplingMode=DISABLE;					//过采样关闭
	ADC3_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //规则通道的数据仅仅保存在DR寄存器里面
    HAL_ADC_Init(&ADC3_Handler);                                 //初始化 
	
	HAL_ADCEx_Calibration_Start(&ADC3_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADC校准
}

//ADC底层驱动，引脚配置，时钟使能
//此函数会被HAL_ADC_Init()调用
//hadc:ADC句柄
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //使能ADC1/2时钟
    __HAL_RCC_ADC3_CLK_ENABLE();           //使能ADC3时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();			//开启GPIOA时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟
	__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC外设时钟选择
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_3;            //PA0 PA3
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_2;            //PC0 PC4 PC2
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

    // GPIO_Initure.Pin=GPIO_PIN_4;            //PC4
    // GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //模拟
    // GPIO_Initure.Pull=GPIO_NOPULL;          //不带上下拉
    // HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

//获得ADC值
//ch: 通道值 0~16，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
//返回值:转换结果
u32 Get_Adc(u8 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //通道
    ADC1_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1个序列
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//采样时间       
	ADC1_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//单边采集          		
	ADC1_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	ADC1_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //通道配置

    HAL_ADC_Start(&ADC1_Handler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //轮询转换
	// return (u16)HAL_ADC_GetValue(&ADC1_Handler);	            //返回最近一次ADC1规则组的转换结果
    return HAL_ADC_GetValue(&ADC1_Handler);
}

u32 Get_Adc3(u8 ch)   
{
    ADC_ChannelConfTypeDef ADC3_ChanConf;
    
    ADC3_ChanConf.Channel=ch;                                   //通道
    ADC3_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1个序列
    ADC3_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//采样时间       
	ADC3_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//单边采集          		
	ADC3_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	ADC3_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC3_Handler,&ADC3_ChanConf);        //通道配置

    HAL_ADC_Start(&ADC3_Handler);                               //开启ADC
	
    HAL_ADC_PollForConversion(&ADC3_Handler,10);                //轮询转换
	// return (u16)HAL_ADC_GetValue(&ADC3_Handler);	            //返回最近一次ADC1规则组的转换结果、
    return HAL_ADC_GetValue(&ADC3_Handler);
}

//获取指定通道的转换值，取times次,然后平均 
//times:获取次数
//返回值:通道ch的times次转换结果平均值
u32 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
    u32 temp_avg=0;
    u64 temp_sum=0;
    u32 temp_max=0;
    u32 temp_min=0;
	u8 t;

    Get_Adc(ch);
	for(t=0;t<times;t++)
	{
        temp_val=Get_Adc(ch);
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

        // HAL_Delay(1);
	}

    temp_avg=(temp_sum-temp_max-temp_min)/(times-2);
	return temp_avg;
} 

u32 Get_Adc3_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
    u32 temp_avg=0;
    u64 temp_sum=0;
    u32 temp_max=0;
    u32 temp_min=0;
	u8 t;

    Get_Adc3(ch);
	for(t=0;t<times;t++)
	{
        temp_val=Get_Adc3(ch);
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

        // HAL_Delay(1);
	}

    temp_avg=(temp_sum-temp_max-temp_min)/(times-2);
	return temp_avg;
} 










