#ifndef __BMP_H__
#define __BMP_H__
#include "sys.h"

//===1205
#include "malloc.h"
#include "ff.h"
#include "exfuns.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK STM32H7������
//ͼƬ���� ��������-bmp���벿��	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2018/8/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//����˵�� 
//��
//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////�û�������///////////////////////////////
#define BMP_USE_MALLOC		1 		//�����Ƿ�ʹ��malloc,��������ѡ��ʹ��malloc
#define BMP_DBUF_SIZE		2048	//����bmp��������Ĵ�С(����ӦΪLCD����*3)
//////////////////////////////////////////////END/////////////////////////////////
//===
//ͼ����Ϣ
typedef struct
{		
	u16 lcdwidth;	//LCD�Ŀ���
	u16 lcdheight;	//LCD�ĸ߶�
	u32 ImgWidth; 	//ͼ���ʵ�ʿ��Ⱥ͸߶�
	u32 ImgHeight;

	u32 Div_Fac;  	//����ϵ�� (������8192����)
	
	u32 S_Height; 	//�趨�ĸ߶ȺͿ���
	u32 S_Width;
	
	u32	S_XOFF;	  	//x���y���ƫ����
	u32 S_YOFF;

	u32 staticx; 	//��ǰ��ʾ���ģ�������
	u32 staticy;																 	
}_pic_info;
extern _pic_info picinfo;//ͼ����Ϣ

//BMP��Ϣͷ
typedef __packed struct
{
    u32 biSize ;		   	//˵��BITMAPINFOHEADER�ṹ����Ҫ��������
    long  biWidth ;		   	//˵��ͼ��Ŀ��ȣ�������Ϊ��λ 
    long  biHeight ;	   	//˵��ͼ��ĸ߶ȣ�������Ϊ��λ 
    u16  biPlanes ;	   		//ΪĿ���豸˵��λ��������ֵ�����Ǳ���Ϊ1 
    u16  biBitCount ;	   	//˵��������/���أ���ֵΪ1��4��8��16��24����32
    u32 biCompression ;  	//˵��ͼ������ѹ�������͡���ֵ����������ֵ֮һ��
	//BI_RGB��û��ѹ����
	//BI_RLE8��ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��  
    //BI_RLE4��ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����
  	//BI_BITFIELDS��ÿ�����صı�����ָ�������������
    u32 biSizeImage ;		//˵��ͼ��Ĵ�С�����ֽ�Ϊ��λ������BI_RGB��ʽʱ��������Ϊ0  
    long  biXPelsPerMeter ;	//˵��ˮƽ�ֱ��ʣ�������/�ױ�ʾ
    long  biYPelsPerMeter ;	//˵����ֱ�ֱ��ʣ�������/�ױ�ʾ
    u32 biClrUsed ;	  	 	//˵��λͼʵ��ʹ�õĲ�ɫ���е���ɫ������
    u32 biClrImportant ; 	//˵����ͼ����ʾ����ҪӰ�����ɫ��������Ŀ�������0����ʾ����Ҫ�� 
}BITMAPINFOHEADER ;
//BMPͷ�ļ�
typedef __packed struct
{
    u16  bfType ;     //�ļ���־.ֻ��'BM',����ʶ��BMPλͼ����
    u32  bfSize ;	  //�ļ���С,ռ�ĸ��ֽ�
    u16  bfReserved1 ;//����
    u16  bfReserved2 ;//����
    u32  bfOffBits ;  //���ļ���ʼ��λͼ����(bitmap data)��ʼ֮��ĵ�ƫ����
}BITMAPFILEHEADER ;
//��ɫ�� 
typedef __packed struct 
{
    u8 rgbBlue ;    //ָ����ɫǿ��
    u8 rgbGreen ;	//ָ����ɫǿ�� 
    u8 rgbRed ;	  	//ָ����ɫǿ�� 
    u8 rgbReserved ;//����������Ϊ0 
}RGBQUAD ;
//λͼ��Ϣͷ
typedef __packed struct
{ 
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bmiHeader;  
	u32 RGB_MASK[3];			//��ɫ�����ڴ��RGB����.
	//RGBQUAD bmiColors[256];  
}BITMAPINFO; 
typedef RGBQUAD * LPRGBQUAD;//��ɫ��  

//ͼ������ѹ��������
#define BI_RGB	 		0  //û��ѹ��.RGB 5,5,5.
#define BI_RLE8 		1  //ÿ������8���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����(�ظ����ؼ�������ɫ����)��
#define BI_RLE4 		2  //ÿ������4���ص�RLEѹ�����룬ѹ����ʽ��2�ֽ����
#define BI_BITFIELDS 	3  //ÿ�����صı�����ָ�������������  
		       
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BMP����뺯��
u8 stdbmp_decode(const u8 *filename);
u8 minibmp_decode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u16 acolor,u8 mode);//�ߴ�С��240*320��bmpͼƬ����.
u8 minibmp_decode2(u8 *filename,u16 x,u16 y,u16 width,u16 height,u16 acolor,uint16_t *data,u8 mode);
u8 bmp_encode(u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 mode);
u8 create_pathname(u8 *pname);
void *pic_memalloc (u32 size);
void pic_memfree (void* mf);	

u8 stdbmp_decode2(const u8 *filename,u16 *data);
u8 bmp_encode2(u8 *filename,u16 x,u16 y,u16 width,u16 height,u8 mode,u16 *data);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//==1205
#ifndef PIC_FORMAT_ERR
#define PIC_FORMAT_ERR		0x27	//��ʽ����
#endif
#ifndef PIC_SIZE_ERR
#define PIC_SIZE_ERR		0x28	//ͼƬ�ߴ����
#endif
#ifndef PIC_WINDOW_ERR
#define PIC_WINDOW_ERR		0x29	//�����趨����
#endif
#ifndef PIC_MEM_ERR
#define PIC_MEM_ERR			0x11	//�ڴ����
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif







#endif







