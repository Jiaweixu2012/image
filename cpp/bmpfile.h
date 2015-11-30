#include <cstdint>
#include <cstring>

using namespace std;

#ifndef __bmpfile_h__
#define __bmpfile_h__

#ifndef BYTE
typedef uint8_t BYTE;
#endif

#ifndef WORD
typedef uint16_t WORD;
#endif

#ifndef DWORD
typedef uint32_t DWORD;
#endif

typedef enum {
  BI_RGB = 0,
  BI_RLE8,
  BI_RLE4,
  BI_BITFIELDS,
  BI_JPEG,
  BI_PNG,
} bmp_compression_method_t;

typedef struct __attribute__((__packed__)) tagBITMAPFILEHEADER {
  WORD  bfType;     /* the magic number used to identify the BMP file:
       0x42 0x4D (Hex code points for B and M).
       The following entries are possible:
       BM - Windows 3.1x, 95, NT, ... etc
       BA - OS/2 Bitmap Array
       CI - OS/2 Color Icon
       CP - OS/2 Color Pointer
       IC - OS/2 Icon
       PT - OS/2 Pointer. */
  DWORD bfSize;     /* the size of the BMP file in bytes */
  WORD  bfReserved1;    /* reserved. */
  WORD  bfReserved2;    /* reserved. */
  DWORD bfOffBits;     /* the offset, i.e. starting address,
       of the byte where the bitmap data can be found. */
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;         /* the size of this header (40 bytes) */
  DWORD  biWidth;       /* the bitmap width in pixels */
  DWORD  biHeight;      /* the bitmap height in pixels */
  WORD  biPlanes;       /* the number of color planes being used.
                  Must be set to 1. */
  WORD  biBitCount;     /* the number of bits per pixel,
                  which is the color depth of the image.
                  Typical values are 1, 4, 8, 16, 24 and 32. */
  DWORD biCompression;  /* the compression method being used.
                  See also bmp_compression_method_t. */
  DWORD biSizeImage;    /* the image size. This is the size of the raw bitmap
                  data (see below), and should not be confused
                  with the file size. */
  DWORD  biXPelsPerMeter;     /* the horizontal resolution of the image.
                  (pixel per meter) */
  DWORD  biYPelsPerMeter;     /* the vertical resolution of the image.
                  (pixel per meter) */
  DWORD biClrUsed;      /* the number of colors in the color palette,
                  or 0 to default to 2<sup><i>n</i></sup>. */
  DWORD biClrImportant;       /* the number of important colors used,
                  or 0 when every color is important;
                  generally ignored. */
} BITMAPINFOHEADER;

typedef struct tagRGBQUAD {
  BYTE rgbBlue;
  BYTE rgbGreen;
  BYTE rgbRed;
  BYTE rgbReserved;
} RGBQUAD;

class Bitmap {
public:
  BITMAPFILEHEADER fileHeader;
  BITMAPINFOHEADER infoHeader;
  RGBQUAD *colors;
  BYTE **pixels;

  int height, width;
  double bytes_per_pixel;
  int bytes_per_line;

  Bitmap(){};
  Bitmap(const char* filename);
  Bitmap(const Bitmap* source);
  Bitmap* create_new_bmp(int height, int width, int depth);
  Bitmap* open_file(const char* filename);
  void write_to_file(const char* filename);
  void close();
  ~Bitmap();
};

void bmp_create_standard_color_table(Bitmap *);
void bmp_create_grayscale_color_table(Bitmap *);
void bmp_std_2_gray(Bitmap *);

#endif /* __bmpfile_h__ */
