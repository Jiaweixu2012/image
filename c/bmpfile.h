#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "hexdump.h"

#ifndef __bmpfile_h__
#define __bmpfile_h__

#ifdef __cplusplus
#define BMP_BEGIN_DECLS extern "C" {
#define BMP_END_DECLS }
#else
#define BMP_BEGIN_DECLS
#define BMP_END_DECLS
#endif

BMP_BEGIN_DECLS

#ifndef BOOL
typedef int BOOL;
#define FALSE (0)
#define TRUE !FALSE
#endif

#ifndef BYTE
typedef uint8_t BYTE;
#endif

#ifndef WORD
typedef uint16_t WORD;
#endif

#ifndef DWORD
typedef uint32_t DWORD;
#endif

typedef struct __attribute__((__packed__)) tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  DWORD  biWidth;
  DWORD  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  DWORD  biXPelsPerMeter;
  DWORD  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD;

BITMAPFILEHEADER  fileHeader;
BITMAPINFOHEADER  infoHeader;
RGBQUAD rgb[256];
BYTE *readFileData;

void writeBmpFile(char *fileName, BYTE *writeFileData)
{
	FILE *file = fopen(fileName, "wb");

	fwrite((BYTE*)&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fwrite((BYTE*)&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	fwrite((BYTE*)&rgb, sizeof(rgb), 1, file);
	fwrite((BYTE*)writeFileData, sizeof(BYTE)*(infoHeader.biHeight)*(infoHeader.biWidth), 1, file);

  fclose(file);
}

BYTE *readBmpFile(char *fileName)
{
  FILE *file = fopen(fileName, "rb");

	fread((BYTE*)&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread((BYTE*)&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	fread((BYTE*)&rgb, sizeof(rgb), 1, file);

	readFileData = (BYTE*)malloc(sizeof(BYTE)*infoHeader.biHeight*infoHeader.biWidth);
	fread(readFileData,sizeof(BYTE)*(infoHeader.biHeight)*(infoHeader.biWidth),1,file);

	hexDump("fileHeader", &fileHeader, sizeof(BITMAPFILEHEADER));
  hexDump("infoHeader", &infoHeader, sizeof(BITMAPINFOHEADER));
  hexDump("colors", &rgb, sizeof(rgb));
//  hexDump("pixels", readFileData, infoHeader.biHeight*infoHeader.biWidth);

	fclose(file);

	return readFileData;
}

BMP_END_DECLS

#endif /* __bmpfile_h__ */
