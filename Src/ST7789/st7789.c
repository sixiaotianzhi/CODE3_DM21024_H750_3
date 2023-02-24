#include "st7789.h"
#include "icon.h"
#include "cmsis_os.h"
#include "Varible.h"
#include "main.h"

static void ST7789_WriteCommand(uint8_t cmd)
{
	ST7789_Select();
	ST7789_DC_Clr();     	
	HAL_SPI_Transmit(&ST7789_SPI_PORT, &cmd, sizeof(cmd), HAL_MAX_DELAY);    
	ST7789_UnSelect();
}

static void ST7789_WriteData(uint8_t *buff, size_t buff_size)
{
	ST7789_Select();
	ST7789_DC_Set();

	// split data in small chunks because HAL can't send more than 64K at once

	while (buff_size > 0) 
    {
		uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;
		        
		HAL_SPI_Transmit(&ST7789_SPI_PORT, buff, chunk_size, HAL_MAX_DELAY);
		
		buff += chunk_size;
		buff_size -= chunk_size;
	}

	ST7789_UnSelect();
}

static void ST7789_WriteData_DMA(uint8_t *buff, size_t buff_size)
{
	ST7789_Select();
	ST7789_DC_Set();

	// split data in small chunks because HAL can't send more than 64K at once
	// while (buff_size > 0) 
    {
		// uint16_t chunk_size = buff_size > 65535 ? 65535 : buff_size;		        
        HAL_SPI_Transmit_DMA(&ST7789_SPI_PORT, buff, buff_size);//===chunk_size);                
        while (ST7789_SPI_PORT.hdmatx->State != HAL_DMA_STATE_READY)
        {
            ;
        }			
		// buff += chunk_size;
		// buff_size -= chunk_size;
	}

	ST7789_UnSelect();
}

static void ST7789_WriteSmallData(uint8_t data)
{
	ST7789_Select();
	ST7789_DC_Set();
	HAL_SPI_Transmit(&ST7789_SPI_PORT, &data, sizeof(data), HAL_MAX_DELAY);   
	ST7789_UnSelect();
}

void ST7789_SetRotation(uint8_t m)
{
	ST7789_WriteCommand(ST7789_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		break;
	case 1:
		ST7789_WriteSmallData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	case 2:
		ST7789_WriteSmallData(ST7789_MADCTL_RGB);
		break;
	case 3:
		ST7789_WriteSmallData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	default:
		break;
	}
}

static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	// ST7789_Select();
	uint16_t x_start = x0 + X_SHIFT, x_end = x1 + X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT, y_end = y1 + Y_SHIFT;
	
	/* Column Address set */
	ST7789_WriteCommand(ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		ST7789_WriteData(data, sizeof(data));
	}

	/* Row Address set */
	ST7789_WriteCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		ST7789_WriteData(data, sizeof(data));
	}
	/* Write to RAM */
	ST7789_WriteCommand(ST7789_RAMWR);
	// ST7789_UnSelect();
}

#if 0
void ST7789_Init(void)
{
	HAL_Delay(25);
    ST7789_RST_Clr();
    HAL_Delay(25);
    ST7789_RST_Set();
    HAL_Delay(50);
		
    ST7789_WriteCommand(ST7789_COLMOD);		//	Set color mode
    ST7789_WriteSmallData(ST7789_COLOR_MODE_16bit);
  	ST7789_WriteCommand(0xB2);				//	Porch control
	{
		uint8_t data[] = {0x0C, 0x0C, 0x00, 0x33, 0x33};
		ST7789_WriteData(data, sizeof(data));
	}
	ST7789_SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)
	
	/* Internal LCD Voltage generator settings */
    ST7789_WriteCommand(0XB7);				//	Gate Control
    ST7789_WriteSmallData(0x35);			//	Default value
    ST7789_WriteCommand(0xBB);				//	VCOM setting
    ST7789_WriteSmallData(0x19);			//	0.725v (default 0.75v for 0x20)
    ST7789_WriteCommand(0xC0);				//	LCMCTRL	
    ST7789_WriteSmallData (0x2C);			//	Default value
    ST7789_WriteCommand (0xC2);				//	VDV and VRH command Enable
    ST7789_WriteSmallData (0x01);			//	Default value
    ST7789_WriteCommand (0xC3);				//	VRH set
    ST7789_WriteSmallData (0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
    ST7789_WriteCommand (0xC4);				//	VDV set
    ST7789_WriteSmallData (0x20);			//	Default value
    ST7789_WriteCommand (0xC6);				//	Frame rate control in normal mode
    ST7789_WriteSmallData (0x0F);			//	Default value (60HZ)
    ST7789_WriteCommand (0xD0);				//	Power control
    ST7789_WriteSmallData (0xA4);			//	Default value
    ST7789_WriteSmallData (0xA1);			//	Default value
	/**************** Division line ****************/

	ST7789_WriteCommand(0xE0);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
		ST7789_WriteData(data, sizeof(data));
	}

    ST7789_WriteCommand(0xE1);
	{
		uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
		ST7789_WriteData(data, sizeof(data));
	}
    ST7789_WriteCommand (ST7789_INVOFF);//===(ST7789_INVON);		//	Inversion ON
	ST7789_WriteCommand (ST7789_SLPOUT);	//	Out of sleep mode
  	ST7789_WriteCommand (ST7789_NORON);		//	Normal Display on
  	ST7789_WriteCommand (ST7789_DISPON);	//	Main screen turned on	

	HAL_Delay(50);
	ST7789_Fill_Color(BLACK);	//BLACK		//	Fill with Black.
}
#endif

void ST7789_Init(void)
{
    HAL_Delay(25);
    ST7789_RST_Clr();
    HAL_Delay(25);
    ST7789_RST_Set();
    HAL_Delay(50);
//--------------------------------ST7789S Frame rate setting----------------------------------//
    ST7789_WriteCommand(0xb2);
    ST7789_WriteSmallData(0x0c);
    ST7789_WriteSmallData(0x0c);
    ST7789_WriteSmallData(0x00);
    ST7789_WriteSmallData(0x33);
    ST7789_WriteSmallData(0x33);

    ST7789_WriteCommand(0xb7);
    ST7789_WriteSmallData(0x35);

    ST7789_WriteCommand(0x3A);
    ST7789_WriteSmallData(0x55);

    ST7789_SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)
//---------------------------------ST7789S Power setting--------------------------------------//
    ST7789_WriteCommand(0xbb);
    ST7789_WriteSmallData(0x30);//vcom

    ST7789_WriteCommand(0xc3);
    ST7789_WriteSmallData(0x1C);//17调深浅

    ST7789_WriteCommand(0xc4);
    ST7789_WriteSmallData(0x18);

    ST7789_WriteCommand(0xc6);//===/* 普通模式下显存速率设置 111hz */
    ST7789_WriteSmallData(0x01);
    
    ST7789_WriteCommand(0xd0);
    ST7789_WriteSmallData(0xa4);
    ST7789_WriteSmallData(0xa2);
    //--------------------------------ST7789S gamma setting---------------------------------------//
    ST7789_WriteCommand(0xe0);
    ST7789_WriteSmallData(0xf0);
    ST7789_WriteSmallData(0x00);
    ST7789_WriteSmallData(0x0a);
    ST7789_WriteSmallData(0x10);
    ST7789_WriteSmallData(0x12);
    ST7789_WriteSmallData(0x1b);
    ST7789_WriteSmallData(0x39);
    ST7789_WriteSmallData(0x44);
    ST7789_WriteSmallData(0x47);
    ST7789_WriteSmallData(0x28);
    ST7789_WriteSmallData(0x12);
    ST7789_WriteSmallData(0x10);
    ST7789_WriteSmallData(0x16);
    ST7789_WriteSmallData(0x1b);

    ST7789_WriteCommand(0xe1);
    ST7789_WriteSmallData(0xf0);
    ST7789_WriteSmallData(0x00);
    ST7789_WriteSmallData(0x0a);
    ST7789_WriteSmallData(0x10);
    ST7789_WriteSmallData(0x11);
    ST7789_WriteSmallData(0x1a);
    ST7789_WriteSmallData(0x3b);
    ST7789_WriteSmallData(0x34);
    ST7789_WriteSmallData(0x4e);
    ST7789_WriteSmallData(0x3a);
    ST7789_WriteSmallData(0x17);
    ST7789_WriteSmallData(0x16);
    ST7789_WriteSmallData(0x21);
    ST7789_WriteSmallData(0x22);

    //===MCU INTERFACD 不需!!!!
    // //*********SET RGB Interfae***************
    // ST7789_WriteCommand(0xB0); 
    // ST7789_WriteSmallData(0x11); //set RGB interface and DE mode.
    // ST7789_WriteSmallData(0x00); 
    // ST7789_WriteSmallData(0x00); 
    // ST7789_WriteCommand(0xB1); 
    // ST7789_WriteSmallData(0x40); // set DE mode ; SET Hs,Vs,DE,DOTCLK signal polarity 
    // ST7789_WriteSmallData(0x00); 
    // ST7789_WriteSmallData(0x00); 
    //+++
    //************************
    ST7789_WriteCommand(0x3a); 
    ST7789_WriteSmallData(0x55); //0x66>>18 RGB ,55-16BIT RGB

    ST7789_WriteCommand(0x11); //===Out of sleep mode
    HAL_Delay(120);      //Delay 120ms 

    ST7789_Fill_Color2(BLACK);//===1011
    ST7789_WriteCommand(0x29); //display on
    ST7789_WriteCommand(0x2c); 

    // ST7789_Fill_Color2(RED);
    // ST7789_LCDBL_Set();  
}

void ST7789_Fill_Color(uint16_t color)
{
	uint16_t i;
    uint16_t j;

	ST7789_SetAddressWindow(0, 0, ST7789_WIDTH - 1, ST7789_HEIGHT - 1);
	// ST7789_Select();
    for (i = 0; i < ST7789_WIDTH; i++)
            for (j = 0; j < ST7789_HEIGHT; j++) {
                uint8_t data[] = {color >> 8, color & 0xFF};
                ST7789_WriteData(data, sizeof(data));
            }
	// ST7789_UnSelect();
}

void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	// if ((x < 0) || (x >= ST7789_WIDTH) ||
	// 	 (y < 0) || (y >= ST7789_HEIGHT))	return;	
	ST7789_SetAddressWindow(x, y, x, y);
	uint8_t data[] = {color >> 8, color & 0xFF};
	// ST7789_Select();
	ST7789_WriteData(data, sizeof(data));
	// ST7789_UnSelect();
}

void ST7789_DrawPixel2(uint16_t x, uint16_t y, uint16_t color,u16 *data)
{
	u16 *ptemp;
    // u16 pix_dst,pix_src,pix_ret;
    ptemp=data;
    
	// ST7789_SetAddressWindow(x, y, x, y);
	// uint8_t data[] = {color >> 8, color & 0xFF};
	// ST7789_Select();
	// ST7789_WriteData(data, sizeof(data));
	// ST7789_UnSelect();

    // pix_dst=*(ptemp+(x)+320*(y));
    // pix_src=color;
    // pix_ret=piclib_alpha_blend(pix_src,pix_dst,0);
    // *(ptemp+(x)+320*(y))=pix_ret;

    // *(ptemp+(x)+320*(y))=((color>>8)&0xff) | ((color<<8)&0xff00);
    *(ptemp+(x)+320*(y))=color;
}

void ST7789_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color)
{
	// if ((xEnd < 0) || (xEnd >= ST7789_WIDTH) ||
	// 	 (yEnd < 0) || (yEnd >= ST7789_HEIGHT))	return;
	ST7789_Select();
	uint16_t i, j;
	ST7789_SetAddressWindow(xSta, ySta, xEnd, yEnd);
	for (i = ySta; i <= yEnd; i++)
		for (j = xSta; j <= xEnd; j++) {
			uint8_t data[] = {color >> 8, color & 0xFF};
			ST7789_WriteData(data, sizeof(data));
		}
	ST7789_UnSelect();
}
#if 0
void ST7789_Fill2(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, u8 color_palette, uint16_t *data)
{
	uint16_t i, j;
	u16 *ptemp;
    ptemp=data;
	uint16_t color2;
	u8 color_index;
	color_index=yEnd;
	
	for (i = ySta; i <= yEnd; i++)
	{
		switch(color_palette)
        {
            case PALETTE_NUM0:
				color2=Clut0_CLUT1[color_index--];
				break;
			case PALETTE_NUM1:
				color2=Clut1_HOT[color_index--];
				break;
			case PALETTE_NUM2:
				color2=Clut2_Grayscale[color_index--];
				break;
			case PALETTE_NUM3:
				color2=Clut3_JET[color_index--];
				break;
			// case PALETTE_NUM4:
			// 	color2=Clut4_HSV[color_index--];
			// 	break;
			// case PALETTE_NUM5:
			// 	color2=Clut5_Grayscale2[color_index--];
			// 	break;
			default:
				break;
		}
		for (j = xSta; j <= xEnd; j++) 
		{
			*(ptemp+(j)+320*(i))=((color2>>8)&0xff) | ((color2<<8)&0xff00);
		}
	}
}
#else
void ST7789_Fill2(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, u8 color_palette, uint16_t *data)
{
	uint16_t i, j;
	u16 *ptemp;
    ptemp=data;
	uint16_t color2;
	// u8 color_index;
	// color_index=yEnd;

    u16 array_color[64];
    u8 cnt1=0;
    u8 cnt2=0;

    for (cnt1 = 255; cnt1 > 0; cnt1=cnt1-4)//===0-255 256/4=64级
	{
        if(cnt2>=64)
            break;//===!!!
		switch(color_palette)
        {
            case PALETTE_NUM0:
				array_color[cnt2++]=Clut0_CLUT1[cnt1];
				break;
			case PALETTE_NUM1:
				array_color[cnt2++]=Clut1_HOT[cnt1];
				break;
			case PALETTE_NUM2:
				array_color[cnt2++]=Clut2_Grayscale[cnt1];
				break;
			case PALETTE_NUM3:
				array_color[cnt2++]=Clut3_JET[cnt1];
				break;			
			default:
				break;
		}    		
	}

	for (i = ySta; i <= yEnd; i++)//===64*3=192=yEnd-ySta
	{
        cnt2=(i-ySta)/3;//===0-63 >> 0-192
        if(cnt2>=64)
            break;//===!!!
		switch(color_palette)
        {
            case PALETTE_NUM0:
				color2=array_color[cnt2];
				break;
			case PALETTE_NUM1:
				color2=array_color[cnt2];
				break;
			case PALETTE_NUM2:
				color2=array_color[cnt2];
				break;
			case PALETTE_NUM3:
				color2=array_color[cnt2];
				break;			
			default:
				break;
		}
		for (j = xSta; j <= xEnd; j++) 
		{
			// *(ptemp+(j)+320*(i))=((color2>>8)&0xff) | ((color2<<8)&0xff00);
            *(ptemp+(j)+320*(i))=color2;            
		}
	}
}
#endif

void ST7789_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color)
{
	// if ((x <= 0) || (x > ST7789_WIDTH) ||
	// 	 (y <= 0) || (y > ST7789_HEIGHT))	return;
	ST7789_Select();
	ST7789_Fill(x - 1, y - 1, x + 1, y + 1, color);
	ST7789_UnSelect();
}

void ST7789_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color) 
{
	uint16_t swap;
    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);
    if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
        //_swap_int16_t(x0, y0);
        //_swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
        //_swap_int16_t(x0, x1);
        //_swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            ST7789_DrawPixel(y0, x0, color);
        } else {
            ST7789_DrawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void ST7789_DrawLine2(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color, u16 *data) 
{
	uint16_t swap;
    uint16_t steep = ABS(y1 - y0) > ABS(x1 - x0);

    u16 *ptemp;
    ptemp=data;

    if (steep) {
		swap = x0;
		x0 = y0;
		y0 = swap;

		swap = x1;
		x1 = y1;
		y1 = swap;
        //_swap_int16_t(x0, y0);
        //_swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
		swap = x0;
		x0 = x1;
		x1 = swap;

		swap = y0;
		y0 = y1;
		y1 = swap;
        //_swap_int16_t(x0, x1);
        //_swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = ABS(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0<=x1; x0++) {
        if (steep) {
            ST7789_DrawPixel2(y0, x0, color, ptemp);
        } else {
            ST7789_DrawPixel2(x0, y0, color, ptemp);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void ST7789_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
	ST7789_Select();
	ST7789_DrawLine(x1, y1, x2, y1, color);
	ST7789_DrawLine(x1, y1, x1, y2, color);
	ST7789_DrawLine(x1, y2, x2, y2, color);
	ST7789_DrawLine(x2, y1, x2, y2, color);
	ST7789_UnSelect();
}

void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_Select();
	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawPixel(x0 + x, y0 + y, color);
		ST7789_DrawPixel(x0 - x, y0 + y, color);
		ST7789_DrawPixel(x0 + x, y0 - y, color);
		ST7789_DrawPixel(x0 - x, y0 - y, color);

		ST7789_DrawPixel(x0 + y, y0 + x, color);
		ST7789_DrawPixel(x0 - y, y0 + x, color);
		ST7789_DrawPixel(x0 + y, y0 - x, color);
		ST7789_DrawPixel(x0 - y, y0 - x, color);
	}
	ST7789_UnSelect();
}

void ST7789_DrawCircle2(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color, u16 *data)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

    u16 *ptemp;
    ptemp=data;

	// ST7789_Select();
	ST7789_DrawPixel2(x0, y0 + r, color, ptemp);
	ST7789_DrawPixel2(x0, y0 - r, color, ptemp);
	ST7789_DrawPixel2(x0 + r, y0, color, ptemp);
	ST7789_DrawPixel2(x0 - r, y0, color, ptemp);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawPixel2(x0 + x, y0 + y, color, ptemp);
		ST7789_DrawPixel2(x0 - x, y0 + y, color, ptemp);
		ST7789_DrawPixel2(x0 + x, y0 - y, color, ptemp);
		ST7789_DrawPixel2(x0 - x, y0 - y, color, ptemp);

		ST7789_DrawPixel2(x0 + y, y0 + x, color, ptemp);
		ST7789_DrawPixel2(x0 - y, y0 + x, color, ptemp);
		ST7789_DrawPixel2(x0 + y, y0 - x, color, ptemp);
		ST7789_DrawPixel2(x0 - y, y0 - x, color, ptemp);
	}
	// ST7789_UnSelect();
}

void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data)
{
	if ((x >= ST7789_WIDTH) || (y >= ST7789_HEIGHT))
		return;
	if ((x + w - 1) >= ST7789_WIDTH)
		return;
	if ((y + h - 1) >= ST7789_HEIGHT)
		return;

	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + w - 1, y + h - 1);
	ST7789_WriteData((uint8_t *)data, sizeof(uint16_t) * w * h);    
	ST7789_UnSelect();
}

void ST7789_InvertColors(uint8_t invert)
{
	ST7789_Select();
	ST7789_WriteCommand(invert ? 0x21 /* INVON */ : 0x20 /* INVOFF */);
	ST7789_UnSelect();
}

void ST7789_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;
	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) {
			if ((b << j) & 0x8000) 
            {
				uint8_t data[] = {color >> 8, color & 0xFF};
				ST7789_WriteData(data, sizeof(data));
			}
			else {
				uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
				ST7789_WriteData(data, sizeof(data));
			}
		}
	}
	ST7789_UnSelect();
}

void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	HAL_SPI_Abort(&ST7789_SPI_PORT);
    MX_SPI1_Init();
    ST7789_SetAddressWindow(0, 0, 320 - 1, 240 - 1);

	while (*str) {
		if (x + font.width >= ST7789_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		ST7789_WriteChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
}

void ST7789_WriteChar2(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor,u8 mode)
{
	uint32_t i, b, j;
	
	for (i = 0; i < font.height; i++) 
    {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) 
        {
			if ((b << j) & 0x8000) 
            {                
				// ST7789_DrawPixel(x+j, y+i,color);
                ST7789_WriteCommand(ST7789_CASET);
                uint8_t data1[] = {x+j >> 8, x+j & 0xFF, x+j >> 8, x+j & 0xFF};
                ST7789_WriteData(data1, sizeof(data1));
                ST7789_WriteCommand(ST7789_RASET);
                uint8_t data2[] = {y+i >> 8, y+i & 0xFF, y+i >> 8, y+i & 0xFF};
                ST7789_WriteData(data2, sizeof(data2));
                ST7789_WriteCommand(ST7789_RAMWR);
                uint8_t data3[] = {color >> 8, color & 0xFF};
                ST7789_WriteData(data3, sizeof(data3));
			}
			else
            {
                if(mode)
                {
                    ST7789_DrawPixel(x+j, y+i,bgcolor);
                    ST7789_WriteCommand(ST7789_CASET);
                    uint8_t data1[] = {x+j >> 8, x+j & 0xFF, x+j >> 8, x+j & 0xFF};
                    ST7789_WriteData(data1, sizeof(data1));
                    ST7789_WriteCommand(ST7789_RASET);
                    uint8_t data2[] = {y+i >> 8, y+i & 0xFF, y+i >> 8, y+i & 0xFF};
                    ST7789_WriteData(data2, sizeof(data2));
                    ST7789_WriteCommand(ST7789_RAMWR);
                    uint8_t data3[] = {bgcolor >> 8, bgcolor & 0xFF};
                    ST7789_WriteData(data3, sizeof(data3));         
                }                
			}
		}
	}
}

void ST7789_WriteString2(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor,u8 mode)
{
	HAL_SPI_Abort(&ST7789_SPI_PORT);
    MX_SPI1_Init();
    ST7789_SetAddressWindow(0, 0, 320 - 1, 240 - 1);

	while (*str) {
		if (x + font.width >= ST7789_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		ST7789_WriteChar2(x, y, *str, font, color, bgcolor ,mode);
		x += font.width;
		str++;
	}
}

void ST7789_WriteChar3(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor, uint16_t *data, u8 mode)
{
	uint32_t i, b, j;
    u16 *ptemp;
    ptemp=data;
    // u16 pix_src;
    // u16 pix_dst;
    // u16 pix_ret;
	
	for (i = 0; i < font.height; i++) 
    {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) 
        {
			if ((b << j) & 0x8000) 
            {                				
                // pix_dst=*(ptemp+(x+j)+320*(y+i));
                // pix_src=color;
                // pix_ret=piclib_alpha_blend(pix_src,pix_dst,0);
                // *(ptemp+(x+j)+320*(y+i))=((pix_ret>>8)&0xff) | ((pix_ret<<8)&0xff00);

                // *(ptemp+(x+j)+320*(y+i))=((color>>8)&0xff) | ((color<<8)&0xff00);
                *(ptemp+(x+j)+320*(y+i))=color;
			}
			else if(mode)
            {                
                // pix_dst=*(ptemp+(x+j)+320*(y+i));
                // pix_src=bgcolor;
                // pix_ret=piclib_alpha_blend(pix_src,pix_dst,32);
                // *(ptemp+(x+j)+320*(y+i))=((pix_ret>>8)&0xff) | ((pix_ret<<8)&0xff00);

                // *(ptemp+(x+j)+320*(y+i))=((bgcolor>>8)&0xff) | ((bgcolor<<8)&0xff00); 
                *(ptemp+(x+j)+320*(y+i))=bgcolor;            
			}
		}
	}
}

void ST7789_WriteString3(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor, uint16_t *data, u8 mode)
{
	u16 *ptemp;
    ptemp=data;

	while (*str) {
		if (x + font.width >= ST7789_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		ST7789_WriteChar3(x, y, *str, font, color, bgcolor ,ptemp, mode);
		x += font.width;
		str++;
	}
	// ST7789_UnSelect();
}

void ST7789_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
	ST7789_Select();
	uint8_t i;

	/* Check input parameters */
	if (x >= ST7789_WIDTH ||
		y >= ST7789_HEIGHT) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= ST7789_WIDTH) {
		w = ST7789_WIDTH - x;
	}
	if ((y + h) >= ST7789_HEIGHT) {
		h = ST7789_HEIGHT - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		ST7789_DrawLine(x, y + i, x + w, y + i, color);
	}
	ST7789_UnSelect();
}

void ST7789_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
	ST7789_Select();
	/* Draw lines */
	ST7789_DrawLine(x1, y1, x2, y2, color);
	ST7789_DrawLine(x2, y2, x3, y3, color);
	ST7789_DrawLine(x3, y3, x1, y1, color);
	ST7789_UnSelect();
}

void ST7789_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color)
{
	ST7789_Select();
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	}
	else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	}
	else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay) {
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	}
	else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++) {
		ST7789_DrawLine(x, y, x3, y3, color);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
	ST7789_UnSelect();
}

void ST7789_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color)
{
	ST7789_Select();
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);
	ST7789_DrawLine(x0 - r, y0, x0 + r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
		ST7789_DrawLine(x0 + x, y0 - y, x0 - x, y0 - y, color);

		ST7789_DrawLine(x0 + y, y0 + x, x0 - y, y0 + x, color);
		ST7789_DrawLine(x0 + y, y0 - x, x0 - y, y0 - x, color);
	}
	ST7789_UnSelect();
}

void ST7789_TearEffect(uint8_t tear)
{
	ST7789_Select();
	ST7789_WriteCommand(tear ? 0x35 /* TEON */ : 0x34 /* TEOFF */);
	ST7789_UnSelect();
}

void ST7789_Test(void)
{   
#if 0       
	ST7789_Fill_Color(WHITE);
	osDelay(1000);//HAL_Delay(1000);
	ST7789_WriteString(10, 20, "Speed Test", Font_16x26, RED, WHITE);
	osDelay(1000);//HAL_Delay(1000);
	ST7789_Fill_Color(CYAN);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(RED);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(BLUE);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(GREEN);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(YELLOW);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(BROWN);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(DARKBLUE);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(MAGENTA);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(LIGHTGREEN);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(LGRAY);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(LBBLUE);
    osDelay(250);//HAL_Delay(500);
	ST7789_Fill_Color(WHITE);
	osDelay(250);//HAL_Delay(500);

	ST7789_WriteString(10, 10, "Font test.", Font_16x26, GBLUE, WHITE);
	ST7789_WriteString(10, 50, "Hello Steve!", Font_7x10, RED, WHITE);
	ST7789_WriteString(10, 75, "Hello Steve!", Font_11x18, YELLOW, WHITE);
	ST7789_WriteString(10, 100, "Hello Steve!", Font_16x26, MAGENTA, WHITE);
	osDelay(3000);//HAL_Delay(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Rect./Line.", Font_11x18, YELLOW, RED);
	ST7789_DrawRectangle(30, 30, 100, 100, WHITE);
	osDelay(1000);//HAL_Delay(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Filled Rect.", Font_11x18, YELLOW, RED);
	ST7789_DrawFilledRectangle(30, 30, 50, 50, WHITE);
	osDelay(1000);//HAL_Delay(1000);


	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Circle.", Font_11x18, YELLOW, RED);
	ST7789_DrawCircle(60, 60, 25, WHITE);
	osDelay(1000);//HAL_Delay(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Filled Cir.", Font_11x18, YELLOW, RED);
	ST7789_DrawFilledCircle(60, 60, 25, WHITE);
	osDelay(1000);//HAL_Delay(1000);

	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Triangle", Font_11x18, YELLOW, RED);
	ST7789_DrawTriangle(30, 30, 30, 70, 60, 40, WHITE);
	osDelay(1000);//HAL_Delay(1000);
#endif
#if 0
	ST7789_Fill_Color(RED);
	ST7789_WriteString(10, 10, "Filled Tri", Font_11x18, YELLOW, RED);
	ST7789_DrawFilledTriangle(30, 30, 30, 70, 60, 40, WHITE);
	osDelay(1000);//HAL_Delay(1000);
    

	//	If FLASH cannot storage anymore datas, please delete codes below.
	ST7789_Fill_Color(WHITE);
    ImageScalingDoubleLine((uint8_t *)saber, dst_rgb, 128, 128, 240, 320);  
    ST7789_DrawImage(0, 0, 240, 320, (uint16_t *)dst_rgb);
	// ST7789_DrawImage(0, 0, 128, 128, (uint16_t *)saber);
	osDelay(3000);//HAL_Delay(3000);
#endif
}

//双线性插值
// void ImageScalingDoubleLine( uint8_t *src_rgb, uint8_t *dst_rgb, int src_width, int src_height, int dst_width, int dst_height)
void ImageScalingDoubleLine( u16 *src_rgb, u16 *dst_rgb, int src_width, int src_height, int dst_width, int dst_height)
{
    //rgb 格式 channel=3, rgba 格式 channel=4
    u8 channel = 1;//===3;
    // float scale_y=dst_height/src_height;//===320/90=3.56
    // float scale_x=dst_width/src_width;//===240/120=2

    // 图像每一行的字节数
    u16 src_line_byte_num = src_width * channel;
    u16 dst_line_byte_num = dst_width * channel;

    u32 pixel_point;
    float src_point_y;
    float src_point_x;
    u16 src_left_point_y;
    u16 src_left_point_x;
    float distance_y;
    float distance_x;
    u16 point_a;
    u16 point_b;
    u16 point_c;
    u16 point_d;
    u16 dst_y;
    u16 dst_x;

    for (dst_y = 0; dst_y < dst_height; dst_y++) 
    {
        for (dst_x = 0; dst_x < dst_width; dst_x++) 
        {
            src_point_y = dst_y*src_height/dst_height;
            src_point_x = dst_x*src_width/dst_width;            
            src_left_point_y = (u16)src_point_y;
            src_left_point_x = (u16)src_point_x;            
            distance_y = src_point_y - src_left_point_y;
            distance_x = src_point_x - src_left_point_x;
            
            point_a = src_left_point_y       * src_line_byte_num + src_left_point_x       * channel;//偏移量a点的rgb起点。
            point_b = src_left_point_y       * src_line_byte_num + (src_left_point_x + 1) * channel;//偏移量b点的rgb起点。 
            point_c = (src_left_point_y + 1) * src_line_byte_num + src_left_point_x       * channel;//偏移量c点的rgb起点。  
            point_d = (src_left_point_y + 1) * src_line_byte_num + (src_left_point_x + 1) * channel;//偏移量d点的rgb起点。
            if (src_left_point_y == src_height - 1) {
                point_c = point_a;
                point_d = point_b;
            }
            if (src_left_point_x == src_width - 1) {
                point_b = point_a;
                point_d = point_c;
            }

            pixel_point = dst_y * dst_line_byte_num + dst_x * channel;

            // for (u16 k = 0; k < channel; k++)
            u8 k=0;
            {
                dst_rgb[pixel_point + k] =
                    src_rgb[point_a + k] * (1 - distance_x) * (1 - distance_y) +
                    src_rgb[point_b + k] * distance_x       * (1 - distance_y) +
                    src_rgb[point_c + k] * distance_y       * (1 - distance_x) +
                    src_rgb[point_d + k] * distance_y       * distance_x;
                
            }
        }
    }
}

void Y16toY8(u8 *pY8,u16 *pY16)
{
    u16 *pPixels16;
    u8 *pPixels8;
    u16 pixel_max;
    u16 pixel_min;
    u32 i;
    
    pPixels16=pY16;
    pPixels8=pY8;

    for(i=0;i<120*90;i++)
    {
        *pPixels16=(u16)((*pPixels16)/1);
        if(i==0)
        {
            pixel_max=*pPixels16;
            pixel_min=*pPixels16;
        }
        if(*pPixels16>pixel_max)
        {
            pixel_max=*pPixels16;                           
        }
        if(*pPixels16<pixel_min)
        {
            pixel_min=*pPixels16;                           
        }
        pPixels16++;        
    }

    pPixels16=pY16;    
    for(i=0;i<120*90;i++)
    {
        *pPixels8=(u8)((*pPixels16-pixel_min)*255.0/(pixel_max-pixel_min));                  	
        pPixels16++;
        pPixels8++;
    }
}
#if 0
void Y8toRGB(u16 *pRGB,u8 *pY8,u8 color_palette)
{
    u16 *pPixels16;
    u8 *pPixels8;
    u32 i;
    u8 index;

    pPixels16=pRGB;
    pPixels8=pY8;
    
    for(i=0;i<120*90;i++)
    {
        *pPixels8 = *pPixels8 < 255 ? *pPixels8 : 255;
        index=*pPixels8;

        switch(color_palette)
        {
            case PALETTE_NUM0:                
                *pPixels16=(Clut0_CLUT1[index]>>8)+(Clut0_CLUT1[index]<<8);
                break;
            case PALETTE_NUM1:
                *pPixels16=(Clut1_HOT[index]>>8)+(Clut1_HOT[index]<<8);
                break;
            case PALETTE_NUM2:
                *pPixels16=(Clut2_Grayscale[index]>>8)+(Clut2_Grayscale[index]<<8);
                break;
            case PALETTE_NUM3:
                *pPixels16=(Clut3_JET[index]>>8)+(Clut3_JET[index]<<8);
                break;
            case PALETTE_NUM4:
                *pPixels16=(Clut4_HSV[index]>>8)+(Clut4_HSV[index]<<8);
                break;
            case PALETTE_NUM5:
                *pPixels16=(Clut5_Grayscale2[index]>>8)+(Clut5_Grayscale2[index]<<8);
                break;
        }            
        
        pPixels16++;
        pPixels8++;
    }   
}
#else
void Y8toRGB(u16 *pRGB,u8 *pY8,u8 color_palette)
{
    u16 *pPixels16;
    u8 *pPixels8;
    u32 i;
    u8 index;

    pPixels16=pRGB;
    pPixels8=pY8;
    
    for(i=0;i<120*90;i++)
    {        
        *pPixels8 = *pPixels8 < 255 ? *pPixels8 : 255;
        index=*pPixels8;

        switch(color_palette)
        {
            case PALETTE_NUM0:                
                *pPixels16=(Clut0_CLUT1[index]>>0);
                break;
            case PALETTE_NUM1:
                *pPixels16=(Clut1_HOT[index]>>0);
                break;
            case PALETTE_NUM2:
                *pPixels16=(Clut2_Grayscale[index]>>0);
                break;
            case PALETTE_NUM3:
                *pPixels16=(Clut3_JET[index]>>0);
                break;
            case PALETTE_NUM4:
                *pPixels16=(Clut4_HSV[index]>>0);
                break;
            case PALETTE_NUM5:
                *pPixels16=(Clut5_Grayscale2[index]>>0);
                break;
        }            
        
        pPixels16++;
        pPixels8++;
    }   
}
#endif
u8 MedianFilter(u8 D[9])
{
    u8 temp;
    u8 i,j;

    for(i=0;i<9;i++)
    {
        for(j=0;j<9-i;j++)
        {
            if(D[i]>D[j+1])
            {
                temp=D[i];
                D[i]=D[j+1];
                D[j+1]=temp;
            }
        }    
    }
        
    return D[4];
}

void Y8MedianFilter(u8 *pY8Filter,u8 *pY8)
{
	u8 i,j;
	u8 D[9]={0}; 

	for(i=1;i<90-1;i++)
    {
        for(j=1;j<120-1;j++)
        {
            D[0]=pY8[(i-1)*120+j+1];
            D[1]=pY8[(i)*120+j+1];
            D[2]=pY8[(i+1)*120+j+1];

			D[3]=pY8[(i-1)*120+j];
            D[4]=pY8[(i)*120+j];
            D[5]=pY8[(i+1)*120+j];

			D[6]=pY8[(i-1)*120+j-1];
            D[7]=pY8[(i)*120+j-1];
            D[8]=pY8[(i+1)*120+j-1];
	
            pY8Filter[(i)*120+j]=MedianFilter(D);
        }
    }
    #if 0
    //===边界 0
    for(i=0;i<90;i++)
    {
        pY8Filter[(i)*120+0]=0;
        pY8Filter[(i)*120+119]=0;
    }
    for(j=0;j<120;j++)
    {
        pY8Filter[j]=0;
        pY8Filter[89*120+j]=0;
    }
    #else
    //===边界 临近值
    for(i=0;i<90;i++)
    {
        pY8Filter[(i)*120+0]=pY8Filter[(i)*120+1];
        pY8Filter[(i)*120+119]=pY8Filter[(i)*120+119-1];
    }
    for(j=0;j<120;j++)
    {
        pY8Filter[j]=pY8Filter[j+120];
        pY8Filter[89*120+j]=pY8Filter[89*120+j-120];
    }
    #endif
}

void Y8AverageFilter(u8 *pY8Filter,u8 *pY8)
{
	u8 i,j;
	u8 D[9]={0}; 

	for(i=1;i<90-1;i++)
    {
        for(j=1;j<120-1;j++)
        {
            D[0]=pY8[(i-1)*120+j+1];
            D[1]=pY8[(i)*120+j+1];
            D[2]=pY8[(i+1)*120+j+1];

			D[3]=pY8[(i-1)*120+j];
            D[4]=pY8[(i)*120+j];
            D[5]=pY8[(i+1)*120+j];

			D[6]=pY8[(i-1)*120+j-1];
            D[7]=pY8[(i)*120+j-1];
            D[8]=pY8[(i+1)*120+j-1];
	
            pY8Filter[(i)*120+j]=(D[0]+D[1]+D[2]+D[3]+D[4]+D[5]+D[6]+D[7]+D[8])/9;
        }
    }
    #if 0
    //===边界 0
    for(i=0;i<90;i++)
    {
        pY8Filter[(i)*120+0]=0;
        pY8Filter[(i)*120+119]=0;
    }
    for(j=0;j<120;j++)
    {
        pY8Filter[j]=0;
        pY8Filter[89*120+j]=0;
    }
    #else
    //===边界 临近值
    for(i=0;i<90;i++)
    {
        pY8Filter[(i)*120+0]=pY8Filter[(i)*120+1];
        pY8Filter[(i)*120+119]=pY8Filter[(i)*120+119-1];
    }
    for(j=0;j<120;j++)
    {
        pY8Filter[j]=pY8Filter[j+120];
        pY8Filter[89*120+j]=pY8Filter[89*120+j-120];
    }
    #endif
}

// void ST7789_DrawImage2(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data)
// {
//     u16 *ptemp;
//     ptemp=data;
      
//     HAL_SPI_Abort(&ST7789_SPI_PORT);
//     MX_SPI1_Init();
//     ST7789_SetAddressWindow(0, 0, 320 - 1, 240 - 1);

//     HAL_SPI_Abort(&ST7789_SPI_PORT); 
//     MX_SPI1_Init2();  
// 	ST7789_WriteData_DMA((u8 *)ptemp, w*h);               
// }

void ST7789_DrawImage2(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data)
{
    u16 *ptemp;
    ptemp=data;
      
    HAL_SPI_Abort(&ST7789_SPI_PORT);
    MX_SPI1_Init();
    ST7789_SetAddressWindow(0, 0, 320 - 1, 120 - 1);

    HAL_SPI_Abort(&ST7789_SPI_PORT); 
    MX_SPI1_Init2();  
	ST7789_WriteData_DMA((u8 *)ptemp, w*h/2); 

    HAL_SPI_Abort(&ST7789_SPI_PORT);
    MX_SPI1_Init();
    ST7789_SetAddressWindow(0, 120, 320 - 1, 240 - 1);

    HAL_SPI_Abort(&ST7789_SPI_PORT); 
    MX_SPI1_Init2();  
	ST7789_WriteData_DMA((u8 *)(ptemp+w*h/2), w*h/2);                  
}

void ST7789_DrawImage3(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data)
{
    u8 *ptemp;
    ptemp=data;

    HAL_SPI_Abort(&ST7789_SPI_PORT); 
    MX_SPI1_Init();    
    ST7789_SetAddressWindow(x, y, w - 1, h - 1); 

	HAL_SPI_Abort(&ST7789_SPI_PORT); 
    MX_SPI1_Init2();
	ST7789_WriteData_DMA(ptemp, w*h); 
}

void ST7789_Fill_Color2(uint16_t color)
{
	uint16_t i;
    uint16_t j;

	HAL_SPI_Abort(&ST7789_SPI_PORT);
    MX_SPI1_Init();
    ST7789_SetAddressWindow(0, 0, 320 - 1, 240 - 1);

    for (i = 0; i < ST7789_WIDTH; i++)
        for (j = 0; j < ST7789_HEIGHT; j++) 
        {
            uint8_t data[] = {color >> 8, color & 0xFF};
            ST7789_WriteData(data, sizeof(data));
        }
}

#define IMAGE_W  640
#define IMAGE_H  0
#define SRC_W   240
#define SRC_H   90
void image120x90to320x240(u8 *psrc,u8 *pdst)
{
    u16 i,j;
    u16 row=0,row_temp=0;
    u16 col=0;

    for(i=0;i<SRC_H;i++)
    {
        col=0;
        for(j=0;j<SRC_W;j+=6)
        {
            //pix1
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+1];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+1];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+1];

            //pix2
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+2];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+3];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+2];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+3];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+2];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+3];

            //pix3
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+4];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+5];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+4];
            pdst[row*IMAGE_W+col++]=psrc[SRC_W*i+j+5];            
        }

        //对扩大的行进行临近像素值填充
        row_temp=row;
        row++;
        if((i+1)%3!=0)
        {
            memcpy(&pdst[row*IMAGE_W],&pdst[row_temp*IMAGE_W],IMAGE_W);    
            row++;
            memcpy(&pdst[row*IMAGE_W],&pdst[row_temp*IMAGE_W],IMAGE_W);    
            row++;
        }
        else
        {
            memcpy(&pdst[row*IMAGE_W],&pdst[row_temp*IMAGE_W],IMAGE_W);    
            row++;
        }
    }
}

#if 0
char *myitoa(int num,char *str,int radix) 
{  
	/* 索引表 */ 
	char index[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"; 
	unsigned unum; /* 中间变量 */ 
	int i=0,j,k; 
 
	if(radix==10&&num<0) /* 十进制负数 */ 
	{ 
		unum=(unsigned)-num; 
		str[i++]='-'; 
	} 
	else 
        unum=(unsigned)num; /* 其它情况 */ 
	/* 逆序 */ 
	do  
	{ 
		str[i++]=index[unum%(unsigned)radix]; 
		unum/=radix; 
	}while(unum); 
	str[i]='\0';

	/* 转换 */ 
	if(str[0]=='-') 
        k=1; /* 十进制负数 */ 
	else 
        k=0; 

	/* 将原来的“/2”改为“/2.0”，保证当num在16~255之间，radix等于16时，也能得到正确结果 */ 
	char temp; 
	for(j=k;j<=(i-k-1)/2.0;j++) 
	{ 
		temp=str[j]; 
		str[j]=str[i-j-1]; 
		str[i-j-1]=temp; 
	} 
	return str; 
} 
#endif
//快速ALPHA BLENDING算法.
//src:源颜色
//dst:目标颜色
//alpha:透明程度(0~32)
//返回值:混合后的颜色.
u16 piclib_alpha_blend(u16 src,u16 dst,u8 alpha)
{
	u32 src2;
	u32 dst2;	 
	//Convert to 32bit |-----GGGGGG-----RRRRR------BBBBB|
	src2=((src<<16)|src)&0x07E0F81F;
	dst2=((dst<<16)|dst)&0x07E0F81F;   
	//Perform blending R:G:B with alpha in range 0..32
	//Note that the reason that alpha may not exceed 32 is that there are only
	//5bits of space between each R:G:B value, any higher value will overflow
	//into the next component and deliver ugly result.
	// dst2=((((dst2-src2)*alpha)>>5)+src2)&0x07E0F81F;
    dst2=((((dst2-src2)*alpha)>>5)+src2)&0x07E0F81F;
	return (dst2>>16)|dst2;  
}




#if 0 //Y16MedianFilter
u8 MedianFilter2(u16 D[9])
{
    u16 temp;
    u8 i,j;

    for(i=0;i<9;i++)
    {
        for(j=0;j<9-i;j++)
        {
            if(D[i]>D[j+1])
            {
                temp=D[i];
                D[i]=D[j+1];
                D[j+1]=temp;
            }
        }    
    }
        
    return D[4];
}

void Y16MedianFilter(u16 *pY16Filter,u16 *pY16)
{
	u8 i,j;
	u16 D[9]={0}; 

	for(i=1;i<90-1;i++)
    {
        for(j=1;j<120-1;j++)
        {
            D[0]=pY16[(i-1)*120+j+1];
            D[1]=pY16[(i)*120+j+1];
            D[2]=pY16[(i+1)*120+j+1];

			D[3]=pY16[(i-1)*120+j];
            D[4]=pY16[(i)*120+j];
            D[5]=pY16[(i+1)*120+j];

			D[6]=pY16[(i-1)*120+j-1];
            D[7]=pY16[(i)*120+j-1];
            D[8]=pY16[(i+1)*120+j-1];
	
            pY16Filter[(i)*120+j]=MedianFilter2(D);
        }
    }
}
# endif

#if 0 //FindClosest
//===二分法查找递增表中最接近的数
void FindClosest(const u16 *pClut, u16 *pRGB)
{           
    u8 low = 0;
    u8 high = 255; 
    u16 middle = 0; 
    u8 index = 0;
    u32 i;
    u16 *pRGBCloset;
    u16 testdata;

    pRGBCloset=pRGB;

    for(i=0;i<LCD_W*LCD_H;i++)
    {
        if(*pRGBCloset <= pClut[0]) 
        {    
            *pRGBCloset=pClut[0];
        }   
        else if(*pRGBCloset >= pClut[255])  
        {      
            *pRGBCloset=pClut[255];  
        }
        else
        {
            while(low <= high) 
            {            
                middle = (u8)((low + high)/2);                  
                if(abs(pClut[middle+1] - *pRGBCloset) > abs(pClut[middle] - *pRGBCloset)) 
                {        
                    high = middle - 1;    
                }      
                else  
                {         
                    low = middle + 1;    
                }         
            }          
            index=(abs(pClut[middle+1] - *pRGBCloset) > abs(pClut[middle] - *pRGBCloset)) ? middle : (middle+1);
            testdata=*pRGBCloset;
            *pRGBCloset=pClut[index];
            testdata=*pRGBCloset;
        }
        pRGBCloset++;
        low = 0;
        high = 255; 
        middle = 0; 
        index = 0;
    }            
}
#endif

#if 0 //Video_createHistogram
u16 histogram[TABLE_LENGTH];
//直方图统计
void Video_createHistogram(){
	u16 i;
//	u16 k = (Video_maxValue - Video_minValue);
//	step	= (k > 256 ? k : 256) / 256;
	for (i = 0; i < TABLE_LENGTH; i++){
		histogram[i] = 0;
	}
	for (i = 0; i < L_W * L_H; i++){
//		if (lbuf[i] >= 0x1fff){
//			histogram[TABLE_LENGTH - 1]++;
//		} else {
//			histogram[lbuf[i] >> 5]++;
//		}
		
		histogram[(lbuf[i] - Video_minValue) * 10 / STEP_VALUE]++;
	}
}

void Video_equalizeHistogram(){
	u32 sum = 0;
	u16 i;
	Video_createHistogram();
	for (i = 0; i < TABLE_LENGTH; i++){
		sum += histogram[i];
		Video_eqhLUT[i] = (Clut_TABLE[Clut_index])[((u32)(TABLE_LENGTH - 1) * sum) / (L_W * L_H)];
	}
	Video_lutTable = Video_eqhLUT;
}
#endif

#if 0 //compress_yuyv_to_rgb
// int compress_yuyv_to_rgb(unsigned char *framebuffer,unsigned char *tempbuffer) 
void compress_yuyv_to_rgb(unsigned char *framebuffer,uint16_t *tempbuffer) 
{
	unsigned char *yuyv = framebuffer;//vd->framebuffer为开辟的640*480*2字节的内存空间
	int z = 0 , i = 0, j = 0;

	// for(i = 0 ; i < 272 ; i++) //宽度的像素点数  480*0.6=288  //此处最大值可以增加1
    for(i = 0 ; i < 90 ; i++)
	{
		// for (j = 0; j < 480; j++) //长度的像素点数 640*0.75=480 //此处值如果减半，即240，则最后yuyv += 4相应的改为yuyv += 8，可达到双屏的效果，而且，外循环的272可以为480，扩大了摄像头的采集范围
		for (j = 0; j < 120; j++)
        {
			int r, g, b;
			int y, u, v;
			uint16_t rgb;//===short rgb;

			//此处代码没研究明白，可能跟采样相关吧，大概执行为(yuy1v): yuvy1uvy2u1v1y3u1v1....

			if (!z)
				y = yuyv[0] << 8;
			else
				y = yuyv[2] << 8;
			u = yuyv[1] - 128;
			v = yuyv[3] - 128;
			//yuv转rgb888

			r = (y + (359 * v)) >> 8;

			g = (y - (88 * u) - (183 * v)) >> 8;

			b = (y + (454 * u)) >> 8;

			r = (r > 255) ? 255 : ((r < 0) ? 0 : r); // RGB的范围为0~255，0为最弱，255为最亮
			g = (g > 255) ? 255 : ((g < 0) ? 0 : g);
			b = (b > 255) ? 255 : ((b < 0) ? 0 : b);
			//rgb888转rgb565
			r >>= 3; //RGB565格式中red占5位，green占6位，blue占5位，故需进行移位
			g >>= 2;
			b >>= 3;

			rgb = (r<<11)|(g<<5)|(b);//生成一个2个字节长度的像素点，即rgb565
			 //赋给lcd映射的buffer
			// *tempbuffer++ = (char)rgb;   //第一个字节
			// *tempbuffer++ = (char)(rgb >> 8); //第二个字节
            *tempbuffer++=rgb;

			if (z++) 
			{
				z = 0;
				yuyv += 4; //yuyv为四个，所以下一像素点从第五个字节开始
			}
		}
		// yuyv += (640-480)*2;//换行，即裁剪，因为摄像头采集的是640*480的，横屏640，但lcd横屏为480，多余的就裁剪了。
        yuyv += (120-120)*2;
    }
}
#endif



