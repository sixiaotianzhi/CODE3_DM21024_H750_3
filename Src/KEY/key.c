#include "key.h"
//#include "delay.h"
#include "main.h"
// #include "cmsis_os.h"

void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();           
    __HAL_RCC_GPIOB_CLK_ENABLE();       
    __HAL_RCC_GPIOD_CLK_ENABLE(); 
    __HAL_RCC_GPIOE_CLK_ENABLE();      

    GPIO_Initure.Pin=GPIO_PIN_15;           
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
    
    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_9;           
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    // GPIO_Initure.Pin=GPIO_PIN_9;           
    // GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    // GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    // GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
    // HAL_GPIO_Init(GPIOB,&GPIO_Initure);

    GPIO_Initure.Pin=GPIO_PIN_0;           
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);

    GPIO_Initure.Pin=GPIO_PIN_8|GPIO_PIN_10;           
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
    GPIO_Initure.Pull=GPIO_NOPULL;          //
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;     //高速
    HAL_GPIO_Init(GPIOE,&GPIO_Initure);
}

//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，WKUP按下 WK_UP
//===注意此函数有响应优先级,KEY0>KEY1>KEY2>WK_UP===!!!
extern u8 usb_flag;
u8 KEY_Scan(u8 mode)
{
    static u8 key_up=0;
    if(key_up&&(KEY1==0||KEY2==0||KEY3==0||KEY4==0))    
    {
        HAL_Delay(10);
        key_up=0;
        
        if(KEY1==0)       
            return KEY1_PRES;
        else if(KEY2==0)  
            return KEY2_PRES;
        else if(KEY3==0)  
            return KEY3_PRES;
        else if(KEY4==0) 
            return KEY4_PRES;                      
    }
    else if(KEY1==1&&KEY2==1&&KEY3==1&&KEY4==1)
        key_up=1;
        
    if(USB_IN==1)
    {
        HAL_Delay(2);
        if(USB_IN==1)
            usb_flag=1;        
    }
    else
        usb_flag=0;

    return 0;   //无按键按下
}





