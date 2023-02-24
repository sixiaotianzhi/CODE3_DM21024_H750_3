#ifndef __ST7789_H
#define __ST7789_H

#include "fonts.h"
#include "main.h"
#include "sys.h"

#define ST7789_SPI_PORT             hspi1
extern SPI_HandleTypeDef            ST7789_SPI_PORT;

#define USING_320X240
#define ST7789_ROTATION             1//===0-3       

#ifdef USING_320X240
    #define ST7789_WIDTH 320
    #define ST7789_HEIGHT 240
		
    #if ST7789_ROTATION == 0
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #elif ST7789_ROTATION == 1
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #elif ST7789_ROTATION == 2
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #elif ST7789_ROTATION == 3
        #define X_SHIFT 0
        #define Y_SHIFT 0
    #endif
#endif

#define WHITE                       0xFFFF
#define BLACK                       0x0000
#define BLUE                        0x001F
#define RED                         0xF800
#define MAGENTA                     0xF81F
#define GREEN                       0x07E0
#define CYAN                        0x7FFF
#define YELLOW                      0xFFE0
#define GRAY                        0X8430
#define BRED                        0XF81F
#define GRED                        0XFFE0
#define GBLUE                       0X07FF
#define BROWN                       0XBC40
#define BRRED                       0XFC07
#define DARKBLUE                    0X01CF
#define LIGHTBLUE                   0X7D7C
#define GRAYBLUE                    0X5458
#define LIGHTGREEN                  0X841F
#define LGRAY                       0XC618
#define LGRAYBLUE                   0XA651
#define LBBLUE                      0X2B12

/* Control Registers and constant codes */
#define ST7789_NOP                  0x00
#define ST7789_SWRESET              0x01
#define ST7789_RDDID                0x04
#define ST7789_RDDST                0x09

#define ST7789_SLPIN                0x10
#define ST7789_SLPOUT               0x11
#define ST7789_PTLON                0x12
#define ST7789_NORON                0x13

#define ST7789_INVOFF               0x20
#define ST7789_INVON                0x21
#define ST7789_DISPOFF              0x28
#define ST7789_DISPON               0x29
#define ST7789_CASET                0x2A
#define ST7789_RASET                0x2B
#define ST7789_RAMWR                0x2C
#define ST7789_RAMRD                0x2E

#define ST7789_PTLAR                0x30
#define ST7789_COLMOD               0x3A
#define ST7789_MADCTL               0x36

/** 
 * Memory Data Access Control Register (0x36H)
 * MAP:     D7  D6  D5  D4  D3  D2  D1  D0 
 * param:   MY  MX  MV  ML  RGB MH  -   -
 * 
 */ 
/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY            0x80  
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX            0x40  
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV            0x20  
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML            0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB           0x00

/* Advanced options */
/**
 * Caution: Do not operate these settings
 * You know what you are doing 
 */
#define ST7789_COLOR_MODE_16bit     0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit     0x66    //  RGB666 (18bit)

/* Basic operations */
#define ST7789_RST_Clr()            HAL_GPIO_WritePin(ST7789_RST_PORT, ST7789_RST_PIN, GPIO_PIN_RESET)
#define ST7789_RST_Set()            HAL_GPIO_WritePin(ST7789_RST_PORT, ST7789_RST_PIN, GPIO_PIN_SET)

#define ST7789_DC_Clr()             HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_RESET)
#define ST7789_DC_Set()             HAL_GPIO_WritePin(ST7789_DC_PORT, ST7789_DC_PIN, GPIO_PIN_SET)

#define ST7789_Select()             HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_RESET)
#define ST7789_UnSelect()           HAL_GPIO_WritePin(ST7789_CS_PORT, ST7789_CS_PIN, GPIO_PIN_SET)

#define ST7789_LCDBL_Clr()          HAL_GPIO_WritePin(ST7789_LCDBL_PORT, ST7789_LCDBL_PIN, GPIO_PIN_RESET)
#define ST7789_LCDBL_Set()          HAL_GPIO_WritePin(ST7789_LCDBL_PORT, ST7789_LCDBL_PIN, GPIO_PIN_SET)

#define ABS(x) ((x) > 0 ? (x) : -(x))

/* Basic functions. */
void ST7789_Init(void);
void ST7789_SetRotation(uint8_t m);
void ST7789_Fill_Color(uint16_t color);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_Fill(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, uint16_t color);
void ST7789_DrawPixel_4px(uint16_t x, uint16_t y, uint16_t color);

/* Graphical functions. */
void ST7789_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7789_DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
void ST7789_DrawImage(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint16_t *data);
void ST7789_InvertColors(uint8_t invert);

/* Text functions. */
void ST7789_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);
void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor);

/* Extented Graphical functions. */
void ST7789_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
void ST7789_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789_DrawFilledTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, uint16_t color);
void ST7789_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);

/* Command functions */
void ST7789_TearEffect(uint8_t tear);

/* Simple test function. */
void ST7789_Test(void);

// void ImageScalingDoubleLine( uint8_t *src_rgb, uint8_t *dst_rgb, int src_width, int src_height, int dst_width, int dst_height);
void ImageScalingDoubleLine( u16 *src_rgb, u16 *dst_rgb, int src_width, int src_height, int dst_width, int dst_height);
void compress_yuyv_to_rgb(unsigned char *framebuffer,uint16_t *tempbuffer);
void Y16toY8(u8 *pY8,u16 *pY16);
void Y8toRGB(u16 *pRGB,u8 *pY8,u8 color_palette);

u8 MedianFilter(u8 D[9]);
void Y8MedianFilter(u8 *pY8Filter,u8 *pY8);
void Y8AverageFilter(u8 *pY8Filter,u8 *pY8);
void Y16MedianFilter(u16 *pY16Filter,u16 *pY16);
void Y16AverageFilter(u16 *pY16Filter,u16 *pY16);
void ST7789_DrawImage2(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t *data);
void FindClosest(const u16 *pClut, u16 *pRGB);
void image120x90to320x240(u8 *psrc,u8 *pdst);
// char *myitoa(int num,char *str,int radix);
void ST7789_DrawImage3(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
void ST7789_WriteString2(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor,u8 mode);
void ST7789_WriteString3(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor, uint16_t *data, u8 mode);
void ST7789_DrawCircle2(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color, u16 *data);
void ST7789_DrawLine2(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1,uint16_t color, u16 *data);
void ST7789_Fill2(uint16_t xSta, uint16_t ySta, uint16_t xEnd, uint16_t yEnd, u8 color_palette, uint16_t *data);
void ST7789_Fill_Color2(uint16_t color);
u16 piclib_alpha_blend(u16 src,u16 dst,u8 alpha);


#ifndef ST7789_ROTATION
    #error You should at least choose a display rotation!
#endif


#endif
