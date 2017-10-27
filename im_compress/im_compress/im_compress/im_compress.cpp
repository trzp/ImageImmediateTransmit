#include "im_compress.h"
#include "source/jpeglib.h"
#include "memory.h"

void compress(BYTE *RGB, int width, int height, char *JPG, int *outlen,int quality)
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