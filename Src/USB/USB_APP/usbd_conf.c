#include "main.h"
#include "delay.h"
//===#include "pcf8574.h"

#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//usbd_conf ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/23
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//*******************************************************************************
//�޸���Ϣ
//��
////////////////////////////////////////////////////////////////////////////////// 	   
 

PCD_HandleTypeDef hpcd;

//��ʾUSB����״̬
//0,û������;
//1,�Ѿ�����;
vu8 bDeviceState=0;		//Ĭ��û������ 

//��ʼ��PCD MSP 
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_MspInit(PCD_HandleTypeDef * hpcd)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef USBClkInit;
      
    //USBʱ�����ã�ʹ��HSI48MHz
    USBClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
    USBClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;
    HAL_RCCEx_PeriphCLKConfig(&USBClkInit);

    if(hpcd->Instance == USB2_OTG_FS)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();       //ʹ��GPIOAʱ��
        
        //����PA11��PA12
        GPIO_InitStruct.Pin=GPIO_PIN_11|GPIO_PIN_12;
        GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull=GPIO_NOPULL;
        GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        //===PCF8574_WriteBit(USB_PWR_IO,1);		    //����USB HOST��Դ����
        
        __HAL_RCC_USB2_OTG_FS_CLK_ENABLE();     //ʹ��OTG FSʱ��
        HAL_NVIC_SetPriority(OTG_FS_IRQn,5,0); //===HAL_NVIC_SetPriority(OTG_FS_IRQn,1,0);  //�����ж����ȼ�
        HAL_NVIC_EnableIRQ(OTG_FS_IRQn);        //ʹ��OTG FS�ж�
    }
    else if (hpcd->Instance == USB1_OTG_HS)
    {
        //USB1 OTG������û�õ�,�ʲ�������
    }
}


//USB OTG �жϷ�����
//��������USB�ж�
void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd);
}

///////////////////////USBD LL PCD�����ص�����(PCD->USB Device Library)///////////////////////
 
//USBD���ý׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef * hpcd)
{
    USBD_LL_SetupStage(hpcd->pData, (uint8_t *) hpcd->Setup);
}

//USBD OUT�׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
    USBD_LL_DataOutStage(hpcd->pData, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

//USBD IN�׶λص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
    USBD_LL_DataInStage(hpcd->pData, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

//USBD SOF�ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SOFCallback(PCD_HandleTypeDef * hpcd)
{
    USBD_LL_SOF(hpcd->pData);
}

//USBD ��λ�ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ResetCallback(PCD_HandleTypeDef * hpcd)
{
    USBD_SpeedTypeDef speed = USBD_SPEED_FULL;

    /* Set USB Current Speed */
    switch (hpcd->Init.speed)
    {
        case PCD_SPEED_HIGH:
            printf("USB Device Library  [HS]\r\n");
            speed = USBD_SPEED_HIGH;
            break;
        case PCD_SPEED_FULL:
            printf("USB Device Library  [FS]\r\n");
            speed = USBD_SPEED_FULL;
            break;
        default:
            printf("USB Device Library  [FS?]\r\n");
            speed = USBD_SPEED_FULL;
            break;
    }
    USBD_LL_Reset(hpcd->pData); //��λ�豸
    USBD_LL_SetSpeed(hpcd->pData, speed);
}

//USBD ����ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef * hpcd)
{
    bDeviceState=0;
	printf("Device In suspend mode.\r\n");
    USBD_LL_Suspend(hpcd->pData);
}

//USBD �ָ��ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef * hpcd)
{
    printf("Device Resumed\r\n");
    USBD_LL_Resume(hpcd->pData);
}

//USBD ISO OUT������ɻص�����
//hpcd:PCD�ṹ��ָ��
//epnum:�˵��
//����ֵ:��
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
    USBD_LL_IsoOUTIncomplete(hpcd->pData, epnum);
}

//USBD ISO IN������ɻص�����
//hpcd:PCD�ṹ��ָ��
//epnum:�˵��
//����ֵ:��
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef * hpcd, uint8_t epnum)
{
    USBD_LL_IsoINIncomplete(hpcd->pData, epnum);
}

//USBD ���ӳɹ��ص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef * hpcd)
{
    bDeviceState=1; 
    USBD_LL_DevConnected(hpcd->pData);
}

//USBD �Ͽ����ӻص�����
//hpcd:PCD�ṹ��ָ��
//����ֵ:��
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef * hpcd)
{
    bDeviceState=0;
	printf("USB Device Disconnected.\r\n");
    USBD_LL_DevDisconnected(hpcd->pData);
}


/*******************************************************************************
                       LL Driver Interface (USB Device Library --> PCD)
*******************************************************************************/

/////////////////////USBD LL �����ӿں���(USB Device Library --> PCD)/////////////////////
 
//USBD�ײ��ʼ������
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef * pdev)
{
#ifdef USE_USB_FS   //���USB FS,ִ��FS�ĳ�ʼ��
    //����LL������ز���
    hpcd.Instance = USB2_OTG_FS;                //ʹ��USB2 OTG
    hpcd.Init.dev_endpoints = 8;                //�˵���Ϊ8    
    hpcd.Init.use_dedicated_ep1 = 0;            //��ֹEP1 dedicated�ж�
    hpcd.Init.ep0_mps = 0x40;                   //���ö˵�0��������СΪ0X40(64�ֽ�)
    hpcd.Init.low_power_enable = 0;             //��ʹ�ܵ͹���ģʽ
    hpcd.Init.phy_itface = PCD_PHY_EMBEDDED;    //ʹ���ڲ�PHY
    hpcd.Init.Sof_enable = 0;                   //ʹ��SOF�ж�
    hpcd.Init.speed = PCD_SPEED_FULL;           //USBȫ��(12Mbps)	
    hpcd.Init.vbus_sensing_enable = 0;          //��ʹ��VBUS���
    hpcd.Init.lpm_enable = 0;                   //ʹ�����ӵ�Դ����

    hpcd.pData = pdev;                          //hpcd��pDataָ��pdev
    pdev->pData = &hpcd;                        //pdev��pDataָ��hpcd

    HAL_PCD_Init(&hpcd);                        //��ʼ��LL����

    HAL_PCDEx_SetRxFiFo(&hpcd, 0x80);           //���ý���FIFO��СΪ0X80(128�ֽ�)
    HAL_PCDEx_SetTxFiFo(&hpcd, 0, 0x40);        //���÷���FIFO 0�Ĵ�СΪ0X40(64�ֽ�)
    HAL_PCDEx_SetTxFiFo(&hpcd, 1, 0x80);        //���÷���FIFO 1�Ĵ�СΪ0X80(128�ֽ�)

#endif

#ifdef USE_USB_HS//���USB HS,ִ��HS�ĳ�ʼ��
    /δʵ��
#endif
    return USBD_OK;
}

//USBD�ײ�ȡ����ʼ��(�ظ�Ĭ�ϸ�λ״̬)���� 
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef * pdev)
{
    HAL_PCD_DeInit(pdev->pData);
    return USBD_OK;
}

//USBD�ײ�������ʼ����
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef * pdev)
{
    HAL_PCD_Start(pdev->pData);
    return USBD_OK;
}

//USBD�ײ�����ֹͣ����
//pdev:USBD���ָ��
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef * pdev)
{
    HAL_PCD_Stop(pdev->pData);
    return USBD_OK;
}

//USBD��ʼ��(��)ĳ���˵�
//pdev:USBD���ָ��
//ep_addr:�˵��
//ep_mps:�˵���������(�ֽ�)
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef * pdev,
                                  uint8_t ep_addr,
                                  uint8_t ep_type, uint16_t ep_mps)
{
    HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

    return USBD_OK;
}

//USBDȡ����ʼ��(�ر�)ĳ���˵�
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
    HAL_PCD_EP_Close(pdev->pData, ep_addr);
    return USBD_OK;
}

//USBD���ĳ���˵������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
    HAL_PCD_EP_Flush(pdev->pData, ep_addr);
    return USBD_OK;
}

//USBD��ĳ���˵�����һ����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
    HAL_PCD_EP_SetStall(pdev->pData, ep_addr);
    return USBD_OK;
}

//USBDȡ��ĳ���˵����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef * pdev,
                                        uint8_t ep_addr)
{
    HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);
    return USBD_OK;
}

//USBD�����Ƿ�����ͣ״̬
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//����ֵ:0,����ͣ; 1,��ͣ.
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *hpcd = pdev->pData;

    if ((ep_addr & 0x80) == 0x80)
    {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    }
    else
    {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

//USBDΪ�豸ָ���µ�USB��ַ
//pdev:USBD���ָ��
//dev_addr:�µ��豸��ַ,USB1_OTG_HS/USB2_OTG_HS
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef * pdev,
                                         uint8_t dev_addr)
{
    bDeviceState=1;	//��ִ�е��ú���,˵��USB���ӳɹ��� 
    HAL_PCD_SetAddress(pdev->pData, dev_addr);
    return USBD_OK;
}

//USBDͨ���˵㷢������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//pbuf:���ݻ������׵�ַ
//size:Ҫ���͵����ݴ�С
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef * pdev,
                                    uint8_t ep_addr,
                                    uint8_t * pbuf, uint16_t size)
{
    HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;
}

//USBD׼��һ���˵��������
//pdev:USBD���ָ��
//ep_addr:�˵�� 
//pbuf:���ݻ������׵�ַ
//size:Ҫ���յ����ݴ�С
//����ֵ:0,����
//      1,æ
//      2,ʧ��
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef * pdev,
                                          uint8_t ep_addr,
                                          uint8_t * pbuf, uint16_t size)
{
    HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);
    return USBD_OK;
}

//USBD��ȡ���һ��������Ĵ�С
//pdev:USBD���ָ��
//ep_addr:�˵��  
//����ֵ:����С
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef * pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount(pdev->pData, ep_addr);
}

//USBD ��ʱ����(��msΪ��λ)
//Delay:��ʱ��ms��.
void USBD_LL_Delay(uint32_t Delay)
{
    // delay_ms(Delay);
    HAL_Delay(Delay);
}
