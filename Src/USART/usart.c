#include "usart.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��os,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//os ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32H7������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2017/6/8
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.0�޸�˵�� 
////////////////////////////////////////////////////////////////////////////////// 	  
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)  
//���HAL��ʹ��ʱ,ĳЩ������ܱ����bug
int _ttywrch(int ch)    
{
    ch=ch;
	return ch;
}
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
	/* Whatever you require here. If the only file you are using is */ 
	/* standard output using printf() for debugging, no file handling */ 
	/* is required. */ 
}; 
/* FILE is typedef�� d in stdio.h. */ 
FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART2->ISR&0X40)==0);//ѭ������,ֱ���������   
	USART2->TDR = (u8) ch;      
	return ch;
}
#endif 
//end
//////////////////////////////////////////////////////////////////

#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u32 USART_RX_STA=0;       //����״̬���	

u8 aRxBuffer[RXBUFFERSIZE];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART2_Handler; //UART���

//��ʼ��IO ����1 
//bound:������
void uart_init(u32 bound)
{	
	//UART ��ʼ������
	UART2_Handler.Instance=USART2;					    //USART2
	UART2_Handler.Init.BaudRate=bound;				    //������
	UART2_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART2_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART2_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART2_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART2_Handler.Init.Mode=UART_MODE_TX;//===UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART2_Handler);					    //HAL_UART_Init()��ʹ��UART1
	
	//=== HAL_UART_Receive_IT(&UART2_Handler, (u8 *)aRxBuffer, RXBUFFERSIZE);//�ú����Ὺ�������жϣ���־λUART_IT_RXNE���������ý��ջ����Լ����ջ���������������
  
}

//UART�ײ��ʼ����ʱ��ʹ�ܣ��������ã��ж�����
//�˺����ᱻHAL_UART_Init()����
//huart:���ھ��

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART2)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOD_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART2_CLK_ENABLE();			//ʹ��USART2ʱ��
	
		GPIO_Initure.Pin=GPIO_PIN_5;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
		GPIO_Initure.Alternate=GPIO_AF7_USART2;	//����ΪUSART2
		HAL_GPIO_Init(GPIOD,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=GPIO_PIN_6;			//PA10
		HAL_GPIO_Init(GPIOD,&GPIO_Initure);	   	//��ʼ��PA10
		
#if EN_USART2_RX
		HAL_NVIC_EnableIRQ(USART2_IRQn);				//ʹ��USART2�ж�ͨ��
		HAL_NVIC_SetPriority(USART2_IRQn,5,0);			//��ռ���ȼ�3�������ȼ�3
#endif	
	}

}

// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
// 	if(huart->Instance==USART2)//����Ǵ���1
// 	{
// 		if((USART_RX_STA&0x800000)==0)//����δ���
// 		{
// 			if(USART_RX_STA&0x400000)//���յ���0x0d
// 			{
// 				if(aRxBuffer[0]!=0x0a)
//                     USART_RX_STA=0;//���մ���,���¿�ʼ
// 				else 
//                     USART_RX_STA|=0x800000;	//��������� 
// 			}
// 			else //��û�յ�0X0D
// 			{	
// 				if(aRxBuffer[0]==0x0d)
//                     USART_RX_STA|=0x400000;
// 				else
// 				{
// 					USART_RX_BUF[USART_RX_STA&0X3FFFFF]=aRxBuffer[0] ;
// 					USART_RX_STA++;
// 					if(USART_RX_STA>(USART_REC_LEN-1))
//                         USART_RX_STA=0;//�������ݴ���,���¿�ʼ����	  
// 				}		 
// 			}
// 		}

// 	}
// }
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART2)//����Ǵ���1
	{
		if((USART_RX_STA&0x800000)==0)//����δ���
		{
			USART_RX_BUF[USART_RX_STA&0X3FFFFF]=aRxBuffer[0] ;
            USART_RX_STA++;
            if(USART_RX_STA>=USART_REC_LEN)
                USART_RX_STA|=0x800000;	//���������	 
		}

	}
}
 
//����1�жϷ������
void USART2_IRQHandler(void)                	
{ 
	u32 timeout=0;
    u32 maxDelay=0x1FFFF;

    //===OS
    //+++
    HAL_UART_IRQHandler(&UART2_Handler);	//����HAL���жϴ����ú���

    timeout=0;
    while (HAL_UART_GetState(&UART2_Handler)!=HAL_UART_STATE_READY)//�ȴ�����
    {
        timeout++;////��ʱ����
        if(timeout>maxDelay) break;		
    }
    
    timeout=0;
    while(HAL_UART_Receive_IT(&UART2_Handler,(u8 *)aRxBuffer, RXBUFFERSIZE)!=HAL_OK)//һ�δ������֮�����¿����жϲ�����RxXferCountΪ1
    {
        timeout++; //��ʱ����
        if(timeout>maxDelay) break;	
    }
    //===OS
    //+++
} 



#endif	


 

 




