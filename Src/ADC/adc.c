#include "adc.h"
//#include "delay.h"
#include "cmsis_os.h"

ADC_HandleTypeDef ADC1_Handler;//ADC���
ADC_HandleTypeDef ADC3_Handler;//ADC���

//��ʼ��ADC
//ch: ADC_channels 
//ͨ��ֵ 0~16ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
void MY_ADC_Init(void)
{ 
    ADC1_Handler.Instance=ADC1;
    ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADC1_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADC1_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADC1_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	ADC1_Handler.Init.LowPowerAutoWait=DISABLE;					//�Զ��͹��Ĺر�				
    ADC1_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADC1_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADC1_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	ADC1_Handler.Init.BoostMode=ENABLE;							//BOOTģʽ�ر�
	ADC1_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//���µ����ݵ�����ֱ�Ӹ��ǵ�������
	ADC1_Handler.Init.OversamplingMode=DISABLE;					//�������ر�
	ADC1_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
    HAL_ADC_Init(&ADC1_Handler);                                 //��ʼ�� 
	
	HAL_ADCEx_Calibration_Start(&ADC1_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
//===
    ADC3_Handler.Instance=ADC3;
    ADC3_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4; 	//4��Ƶ��ADCCLK=PER_CK/4=64/4=16MHZ
    ADC3_Handler.Init.Resolution=ADC_RESOLUTION_16B;           	//16λģʽ
    ADC3_Handler.Init.ScanConvMode=DISABLE;                    	//��ɨ��ģʽ
    ADC3_Handler.Init.EOCSelection=ADC_EOC_SINGLE_CONV;       	//�ر�EOC�ж�
	ADC3_Handler.Init.LowPowerAutoWait=DISABLE;					//�Զ��͹��Ĺر�				
    ADC3_Handler.Init.ContinuousConvMode=DISABLE;               //�ر�����ת��
    ADC3_Handler.Init.NbrOfConversion=1;                        //1��ת���ڹ��������� Ҳ����ֻת����������1 
    ADC3_Handler.Init.DiscontinuousConvMode=DISABLE;            //��ֹ����������ģʽ
    ADC3_Handler.Init.NbrOfDiscConversion=0;                    //����������ͨ����Ϊ0
    ADC3_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;      //�������
    ADC3_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//ʹ���������
	ADC3_Handler.Init.BoostMode=ENABLE;							//BOOTģʽ�ر�
	ADC3_Handler.Init.Overrun=ADC_OVR_DATA_OVERWRITTEN;			//���µ����ݵ�����ֱ�Ӹ��ǵ�������
	ADC3_Handler.Init.OversamplingMode=DISABLE;					//�������ر�
	ADC3_Handler.Init.ConversionDataManagement=ADC_CONVERSIONDATA_DR;  //����ͨ�������ݽ���������DR�Ĵ�������
    HAL_ADC_Init(&ADC3_Handler);                                 //��ʼ�� 
	
	HAL_ADCEx_Calibration_Start(&ADC3_Handler,ADC_CALIB_OFFSET,ADC_SINGLE_ENDED); //ADCУ׼
}

//ADC�ײ��������������ã�ʱ��ʹ��
//�˺����ᱻHAL_ADC_Init()����
//hadc:ADC���
void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_ADC12_CLK_ENABLE();           //ʹ��ADC1/2ʱ��
    __HAL_RCC_ADC3_CLK_ENABLE();           //ʹ��ADC3ʱ��
    __HAL_RCC_GPIOA_CLK_ENABLE();			//����GPIOAʱ��
    __HAL_RCC_GPIOC_CLK_ENABLE();			//����GPIOCʱ��
	__HAL_RCC_ADC_CONFIG(RCC_ADCCLKSOURCE_CLKP); //ADC����ʱ��ѡ��
	
    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_3;            //PA0 PA3
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    GPIO_Initure.Pin=GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_2;            //PC0 PC4 PC2
    GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

    // GPIO_Initure.Pin=GPIO_PIN_4;            //PC4
    // GPIO_Initure.Mode=GPIO_MODE_ANALOG;     //ģ��
    // GPIO_Initure.Pull=GPIO_NOPULL;          //����������
    // HAL_GPIO_Init(GPIOC,&GPIO_Initure);
}

//���ADCֵ
//ch: ͨ��ֵ 0~16��ȡֵ��ΧΪ��ADC_CHANNEL_0~ADC_CHANNEL_16
//����ֵ:ת�����
u32 Get_Adc(u8 ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;
    
    ADC1_ChanConf.Channel=ch;                                   //ͨ��
    ADC1_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//����ʱ��       
	ADC1_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//���߲ɼ�          		
	ADC1_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	ADC1_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC1_Handler,&ADC1_ChanConf);        //ͨ������

    HAL_ADC_Start(&ADC1_Handler);                               //����ADC
	
    HAL_ADC_PollForConversion(&ADC1_Handler,10);                //��ѯת��
	// return (u16)HAL_ADC_GetValue(&ADC1_Handler);	            //�������һ��ADC1�������ת�����
    return HAL_ADC_GetValue(&ADC1_Handler);
}

u32 Get_Adc3(u8 ch)   
{
    ADC_ChannelConfTypeDef ADC3_ChanConf;
    
    ADC3_ChanConf.Channel=ch;                                   //ͨ��
    ADC3_ChanConf.Rank=ADC_REGULAR_RANK_1;                  	//1������
    ADC3_ChanConf.SamplingTime=ADC_SAMPLETIME_64CYCLES_5;      	//����ʱ��       
	ADC3_ChanConf.SingleDiff=ADC_SINGLE_ENDED;  				//���߲ɼ�          		
	ADC3_ChanConf.OffsetNumber=ADC_OFFSET_NONE;             	
	ADC3_ChanConf.Offset=0;   
    HAL_ADC_ConfigChannel(&ADC3_Handler,&ADC3_ChanConf);        //ͨ������

    HAL_ADC_Start(&ADC3_Handler);                               //����ADC
	
    HAL_ADC_PollForConversion(&ADC3_Handler,10);                //��ѯת��
	// return (u16)HAL_ADC_GetValue(&ADC3_Handler);	            //�������һ��ADC1�������ת�������
    return HAL_ADC_GetValue(&ADC3_Handler);
}

//��ȡָ��ͨ����ת��ֵ��ȡtimes��,Ȼ��ƽ�� 
//times:��ȡ����
//����ֵ:ͨ��ch��times��ת�����ƽ��ֵ
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










