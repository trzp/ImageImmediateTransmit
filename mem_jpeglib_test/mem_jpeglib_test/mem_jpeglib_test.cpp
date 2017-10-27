// mem_jpeglib_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "lib\jpeglib.h"
#include "memory.h"
#include "string.h"

#pragma comment (lib,"lib/mem_jpeglib.lib")

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef long				LONG;

// 调色板项
typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
// bmp头信息
typedef struct tagBITMAPINFOHEADER{
        DWORD      biSize;
        LONG       biWidth;
        LONG       biHeight;
        WORD       biPlanes;
        WORD       biBitCount;
        DWORD      biCompression;
        DWORD      biSizeImage;
        LONG       biXPelsPerMeter;
        LONG       biYPelsPerMeter;
        DWORD      biClrUsed;
        DWORD      biClrImportant;
} BITMAPINFOHEADER;
// bmp信息
typedef struct tagBITMAPINFO {
    BITMAPINFOHEADER    bmiHeader;
    RGBQUAD             bmiColors[1];
} BITMAPINFO;

#include <pshpack2.h>
//bmp文件头
typedef struct tagBITMAPFILEHEADER {
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
} BITMAPFILEHEADER;

void compress(BYTE *RGB, int width, int height, char *JPG, int *outlen,int quality = 60)
{
	struct jpeg_compress_struct jcs;
	struct jpeg_error_mgr jem;
	jcs.err = jpeg_std_error(&jem);
	jpeg_create_compress(&jcs);
    jpeg_stdio_dest(&jcs, JPG, outlen);

	jcs.image_width = width; 			// 为图的宽和高，单位为像素 
	jcs.image_height = height;
	jcs.input_components = 3;			// 1,表示灰度图， 如果是彩色位图，则为3 
	jcs.in_color_space = JCS_RGB;

	jpeg_set_defaults(&jcs);
	jpeg_set_quality (&jcs, quality, true);

	jpeg_start_compress(&jcs, TRUE);

	JSAMPROW row_pointer[1];			// 一行位图
	int row_stride=jcs.image_width*3;	// 每一行的字节数

	// 对每一行进行压缩
	while (jcs.next_scanline < jcs.image_height) {
	    row_pointer[0] = & RGB[(jcs.image_height-jcs.next_scanline-1) * (row_stride)];
	    jpeg_write_scanlines(&jcs, row_pointer, 1);
	}

	jpeg_finish_compress(&jcs);
	jpeg_destroy_compress(&jcs);
	delete [] RGB;
}

void decompress(char *JPG,int JPGlen,BYTE *RGB,int width,int height)
{
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo,JPG,JPGlen);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	JSAMPROW row_pointer[1];
	while (cinfo.output_scanline < cinfo.output_height)
	{
		row_pointer[0] = &RGB[(cinfo.output_height - cinfo.output_scanline-1)*(cinfo.image_width*cinfo.num_components)];
		jpeg_read_scanlines(&cinfo,row_pointer ,1);
	}
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
}

/***********************************************************************
压缩图像到jpeg格式
如果压缩前图像为灰度图或24位图，压缩后图像色彩模式不变
如果压缩前图像为256色索引位图，压缩后变为灰度图
************************************************************************/
void bmptojpg()
{
	// 打开图像文件
	BYTE RGB[400*400*3]={0};
	FILE *f = fopen("d:\\121212.bmp","rb");
	
	// 读取文件头
	fread(&RGB,400*400*3,1,f);

	char JpgData[400*400*3]={0};
	int pSize=0;
	compress(RGB,400,400,JpgData,&pSize,30);

	FILE *fff = fopen("d:\\result.jpg","wb");
	fwrite(&JpgData,pSize,1,fff);
	fclose(fff);

	BYTE RGB2[400*400*3]={0};
	//解压
	decompress(JpgData,pSize,RGB2,400,400);

	f=fopen("d:\\121212.dat","wb");
	fwrite(RGB2,400*400*3,1,f);
	fclose(f);
}



int _tmain(int argc, _TCHAR* argv[])
{
	bmptojpg();
	return 0;
}

