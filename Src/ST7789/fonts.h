#ifndef __FONT_H
#define __FONT_H

#include "stdint.h"

typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

//Font lib.
// extern FontDef Font_7x10;
extern FontDef Font_11x18;
// extern FontDef Font_16x26;

//16-bit(RGB565) Image lib.
/*******************************************
 *             CAUTION:
 *   If the MCU onchip flash cannot
 *  store such huge image data,please
 *           do not use it.
 * These pics are for test purpose only.
 *******************************************/

/* 128x128 pixel RGB565 image */
extern const uint16_t saber[][128];
extern  uint8_t saber2[115200];

/* 240x240 pixel RGB565 image 
extern const uint16_t knky[][240];
extern const uint16_t tek[][240];
extern const uint16_t adi1[][240];
*/

extern const unsigned char gImage_Hand[5120];
extern const unsigned char gImage_set[5120];
extern const unsigned char gImage_Hand2[5120];

extern const unsigned char gImage_menu0[19200];
extern const unsigned char gImage_menu11[19200];
extern const unsigned char gImage_menu22[19200];
extern const unsigned char gImage_menu1[19200];
extern const unsigned char gImage_menu2[19200];





#endif
