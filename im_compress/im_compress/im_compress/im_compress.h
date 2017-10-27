#ifndef IM_COMPRESS_H_
#define IM_COMPRESS_H_

#ifdef IM_COMPRESSDLL
#define IM_COMPRESSDLL extern "C" _declspec(dllimport)
#else
#define IM_COMPRESSDLL extern "C" _declspec(dllexport)
#endif

typedef unsigned char       BYTE;

IM_COMPRESSDLL void compress(BYTE *RGB, int width, int height, char *JPG, int *outlen,int quality=60);
IM_COMPRESSDLL void decompress(char *JPG,int JPGlen,BYTE *RGB,int width,int height);

#endif

