#include "lcd.h"
#include "cmsis_os.h"

void lcd_pin_init(void);
void EnterSleep (void);
void ExitSleep (void);


void lcd_pin_init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    __HAL_RCC_GPIOA_CLK_ENABLE();					//开启GPIOB时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
	
    GPIO_Initure.Pin=GPIO_PIN_7;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);     		//初始化

    GPIO_Initure.Pin=GPIO_PIN_13|GPIO_PIN_15;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//高速
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);     		//初始化

    GPIO_Initure.Pin=GPIO_PIN_9|GPIO_PIN_13;			
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
    GPIO_Initure.Pull=GPIO_NOPULL;         			//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//高速
    HAL_GPIO_Init(GPIOD,&GPIO_Initure);     		//初始化

    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13|GPIO_PIN_15,GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9,GPIO_PIN_SET);//===低有效 拉高	
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);	
}


//NOTE:VCI=3.3V,使用RGB接口必需要用SPI对LCD进行初始化.
//************************指令*****************************
void write_command (unsigned char y) // (uchar y,uchar x)
{
    unsigned char i;
    LCD_CS(0);//===cs=0;
    LCD_RS(0);//===rs=0;
    for(i=0;i<8;i++)
    {
        LCD_SCL(0);//===scl=0;
        if (y&0x80)
        {
            LCD_SDI(1);//===sdi=1;
        }
        else
        {
            LCD_SDI(0);//===sdi=0;
        }
        LCD_SCL(1);//===scl=1;
        y=y<<1;
    }
    LCD_CS(1);//===cs=1;
}
//************************************数据***************************
void write_data(unsigned char w) // (uchar w, uchar v)
{
    unsigned char i;
    LCD_CS(0);//===cs=0;
    LCD_RS(1);//===rs=1;
    for(i=0;i<8;i++)
    {
        LCD_SCL(0);//===scl=0;
        if (w&0x80)
        {
            LCD_SDI(1);//===sdi=1;
        }
        else
        {
            LCD_SDI(0);//===sdi=0;
        } 
        LCD_SCL(1);//===scl=1;
        w=w<<1;
    }
    LCD_CS(1);//===cs=1;
}
//**************LCD*******************************
void lcd_init(void)
{
    lcd_pin_init();

    LCD_RS(1);//===res=1;
    HAL_Delay(1);//===osDelay(1);//===delay(1);
    LCD_RS(0);//===res=0;
    HAL_Delay(10);//===osDelay(10);//===delay(10);
    LCD_RS(1);//===res=1;
    HAL_Delay(120);//===osDelay(120);//===delay(120);
    //--------------------------------ST7789S Frame rate setting----------------------------------//
    write_command(0xb2);
    write_data(0x0c);
    write_data(0x0c);
    write_data(0x00);
    write_data(0x33);
    write_data(0x33);

    write_command(0xb7);
    write_data(0x35);

    write_command(0x3A);
    write_data(0x55);
    //---------------------------------ST7789S Power setting--------------------------------------//
    write_command(0xbb);
    write_data(0x30);//vcom

    write_command(0xc3);
    write_data(0x1C);//17调深浅

    write_command(0xc4);
    write_data(0x18);

    write_command(0xc6);
    write_data(0x0f);

    write_command(0xd0);
    write_data(0xa4);
    write_data(0xa2);
    //--------------------------------ST7789S gamma setting---------------------------------------//
    write_command(0xe0);
    write_data(0xf0);
    write_data(0x00);
    write_data(0x0a);
    write_data(0x10);
    write_data(0x12);
    write_data(0x1b);
    write_data(0x39);
    write_data(0x44);
    write_data(0x47);
    write_data(0x28);
    write_data(0x12);
    write_data(0x10);
    write_data(0x16);
    write_data(0x1b);

    write_command(0xe1);
    write_data(0xf0);
    write_data(0x00);
    write_data(0x0a);
    write_data(0x10);
    write_data(0x11);
    write_data(0x1a);
    write_data(0x3b);
    write_data(0x34);
    write_data(0x4e);
    write_data(0x3a);
    write_data(0x17);
    write_data(0x16);
    write_data(0x21);
    write_data(0x22);

    //*********SET RGB Interfae***************
    write_command(0xB0); 
    write_data(0x11); //set RGB interface and DE mode.
    write_data(0x00); 
    write_data(0x00); 

    write_command(0xB1); 
    write_data(0x40); // set DE mode ; SET Hs,Vs,DE,DOTCLK signal polarity 
    write_data(0x00); 
    write_data(0x00); 

    write_command(0x3a); 
    write_data(0x66); //18 RGB ,55-16BIT RGB

    // //===
    // write_command(0x36); //MADCTL Memory Data Access Control
    // write_data(0xc0);    //X-Mirror Y-Mirror

    // write_command(0x3a); 
    // write_data(0x55); //18 RGB ,55-16BIT RGB
    // //+++

    //************************
    write_command(0x11); 
    //===delay(120);      //Delay 120ms 

    write_command(0x29); //display on
    write_command(0x2c);

    LCD_BL(1);

}
//*********************************************************************************************
void EnterSleep (void)
{
    write_command(0x28);
    //===delay(20);
    write_command(0x10);
    LCD_BL(0);//===RGB_OFF();
}
//*********************************************************
void ExitSleep (void)
{
    LCD_BL(1);//===RGB_ON();
    //===delay(30);
    write_command(0x11);
    //===delay(120);
    write_command(0x29);

}
