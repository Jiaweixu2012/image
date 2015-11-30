#include <cstring>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <iostream>
#include "bmpfile.h"

#define DEBUG_MODE

#define DEFAULT_DPI_X 3780
#define DEFAULT_DPI_Y 3780
#define DPI_FACTOR 39.37007874015748

using namespace std;

Bitmap::Bitmap(const char* filename){
  open_file(filename);
}

Bitmap::Bitmap(const Bitmap* source){
  height = source->height;
  width = source->width;
  bytes_per_pixel = source->bytes_per_pixel;
  bytes_per_line = source->bytes_per_line;

  fileHeader = source->fileHeader;
  infoHeader = source->infoHeader;

  colors = new RGBQUAD[0x01<<infoHeader.biBitCount];
  memcpy(colors, source->colors, sizeof(RGBQUAD)*(0x01<<infoHeader.biBitCount));

  pixels = new BYTE*[height];
  for (int i=0; i<height; i++){
    pixels[i] = new BYTE[bytes_per_line];
    memcpy(pixels[i], source->pixels[i], sizeof(BYTE)*bytes_per_line);
  }

  // hexDump((char*)"fileHeader", &fileHeader, sizeof(BITMAPFILEHEADER));
  // hexDump((char*)"infoHeader", &infoHeader, sizeof(BITMAPINFOHEADER));
  // hexDump((char*)"colors", colors, sizeof(RGBQUAD)*(0x01<<infoHeader.biBitCount));
  // hexDump((char*)"pixels", pixels[i], bytes_per_line);

}

Bitmap* Bitmap::create_new_bmp(int height, int width, int depth){
  DWORD palette_size;

  if (depth != 1 && depth != 4 && depth != 8 && depth != 16 && depth != 24 &&
      depth != 32)
    return NULL;
  this->height = height;
  this->width = width;
  fileHeader.bfType = 0x4d42; //"BM"
  infoHeader.biSize = 40;
  infoHeader.biWidth = width;
  infoHeader.biHeight = height;
  infoHeader.biPlanes = 1;
  infoHeader.biBitCount = depth;
  infoHeader.biXPelsPerMeter = DEFAULT_DPI_X;
  infoHeader.biYPelsPerMeter = DEFAULT_DPI_Y;

  if (depth == 16)
    infoHeader.biCompression = BI_BITFIELDS;
  else
    infoHeader.biCompression = BI_RGB;

  //malloc_colors
  infoHeader.biClrUsed = 0x01<<infoHeader.biBitCount;
  if (depth == 1 || depth == 4 || depth == 8) {
    colors = new RGBQUAD[infoHeader.biClrUsed];
    bmp_create_standard_color_table(this);
  }

  /* Calculate the field value of header and DIB */
  bytes_per_pixel = (infoHeader.biBitCount * 1.0) / 8.0;
  bytes_per_line = (int)ceil(bytes_per_pixel * width);
  if (bytes_per_line % 4 != 0)
    bytes_per_line += 4 - bytes_per_line % 4;

  pixels = new BYTE*[height];
  for (int i=0; i<height; i++){
    pixels[i] = new BYTE[bytes_per_line];
    memset(pixels[i], 255, width);
  }
  infoHeader.biSizeImage = bytes_per_line * height;

  palette_size = 0;
  if (depth == 1 || depth == 4 || depth == 8)
    palette_size = 0x01<<(infoHeader.biBitCount+2);
  else if (depth == 16)
    palette_size = 3 * 4;

  fileHeader.bfOffBits = 14 + infoHeader.biSize + palette_size;
  fileHeader.bfSize = fileHeader.bfOffBits + infoHeader.biSizeImage;

  return this;
}

Bitmap* Bitmap::open_file(const char* filename){
  ifstream file(filename, ifstream::binary);
  if (file){
    file.read((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
    file.read((char*)&infoHeader, sizeof(BITMAPINFOHEADER));
    // hexDump((char*)"fileHeader", &fileHeader, sizeof(BITMAPFILEHEADER));
    // hexDump((char*)"infoHeader", &infoHeader, sizeof(BITMAPINFOHEADER));

    height = infoHeader.biHeight;
    width = infoHeader.biWidth;

    colors = new RGBQUAD[0x01<<infoHeader.biBitCount];
    file.read((char*)colors, sizeof(RGBQUAD)*(0x01<<infoHeader.biBitCount));
    // hexDump((char*)"colors", colors, sizeof(RGBQUAD)*(0x01<<infoHeader.biBitCount);

    bytes_per_pixel = (infoHeader.biBitCount * 1.0) / 8.0;
    bytes_per_line = (int)ceil(bytes_per_pixel * infoHeader.biWidth);
    if (bytes_per_line % 4 != 0)
      bytes_per_line += 4 - bytes_per_line % 4;

    pixels = new BYTE*[height];
    for (int i=0; i<height; i++){
      pixels[i] = new BYTE[bytes_per_line];
      file.read((char*)pixels[i], sizeof(BYTE)*bytes_per_line);
    // hexDump((char*)"pixels", pixels[i], bytes_per_line);
    }
  }
  file.close();

  return this;
}

void Bitmap::write_to_file(const char* filename){
  ofstream file(filename, fstream::binary);

  file.write((char*)&fileHeader, sizeof(BITMAPFILEHEADER));
  file.write((char*)&infoHeader, sizeof(BITMAPINFOHEADER));
  file.write((char*)colors, sizeof(RGBQUAD)*(0x01<<infoHeader.biBitCount));
  for (int i=0; i<height; i++)
    file.write((char*)pixels[i], sizeof(BYTE)*bytes_per_line);

  file.close();
}

void Bitmap::close(){
  if (NULL != colors){
    delete colors;
    colors = NULL;
  }
  if (NULL != pixels){
    for (int i=0; i<height; i++)
      delete pixels[i];
    pixels = NULL;
  }
}

Bitmap::~Bitmap(){
  this->close();
}

/**
 * Function from libbmp
 */

/**
 * Create the standard color table for BMP object
 */
void
bmp_create_standard_color_table(Bitmap *bmp)
{
  int i, j, k, ell;

  switch (bmp->infoHeader.biBitCount) {
  case 1:
    for (i = 0; i < 2; ++i) {
      bmp->colors[i].rgbRed = i * 255;
      bmp->colors[i].rgbGreen = i * 255;
      bmp->colors[i].rgbBlue = i * 255;
      bmp->colors[i].rgbReserved = 0;
    }
    break;

  case 4:
    i = 0;
    for (ell = 0; ell < 2; ++ell) {
      for (k = 0; k < 2; ++k) {
	for (j = 0; j < 2; ++j) {
	  bmp->colors[i].rgbRed = j * 128;
	  bmp->colors[i].rgbGreen = k * 128;
	  bmp->colors[i].rgbBlue = ell * 128;
	  bmp->colors[i].rgbReserved = 0;
	  ++i;
	}
      }
    }

    for (ell = 0; ell < 2; ++ell) {
      for (k = 0; k < 2; ++k) {
	for (j = 0; j < 2; ++j) {
	  bmp->colors[i].rgbRed = j * 255;
	  bmp->colors[i].rgbGreen = k * 255;
	  bmp->colors[i].rgbBlue = ell * 255;
	  bmp->colors[i].rgbReserved = 0;
	  ++i;
	}
      }
    }

    i = 8;
    bmp->colors[i].rgbRed = 192;
    bmp->colors[i].rgbGreen = 192;
    bmp->colors[i].rgbBlue = 192;
    bmp->colors[i].rgbReserved = 0;

    break;

  case 8:
    i = 0;
    for (ell = 0; ell < 4; ++ell) {
      for (k = 0; k < 8; ++k) {
	for (j = 0; j < 8; ++j) {
	  bmp->colors[i].rgbRed = j * 32;
	  bmp->colors[i].rgbGreen = k * 32;
	  bmp->colors[i].rgbBlue = ell * 64;
	  bmp->colors[i].rgbReserved = 0;
	  ++i;
	}
      }
    }

    i = 0;
    for (ell = 0; ell < 2; ++ell) {
      for (k = 0; k < 2; ++k) {
	for (j = 0; j < 2; ++j) {
	  bmp->colors[i].rgbRed = j * 128;
	  bmp->colors[i].rgbGreen = k * 128;
	  bmp->colors[i].rgbBlue = ell * 128;
	  ++i;
	}
      }
    }

    // overwrite colors 7, 8, 9
    i = 7;
    bmp->colors[i].rgbRed = 192;
    bmp->colors[i].rgbGreen = 192;
    bmp->colors[i].rgbBlue = 192;
    i++; // 8
    bmp->colors[i].rgbRed = 192;
    bmp->colors[i].rgbGreen = 220;
    bmp->colors[i].rgbBlue = 192;
    i++; // 9
    bmp->colors[i].rgbRed = 166;
    bmp->colors[i].rgbGreen = 202;
    bmp->colors[i].rgbBlue = 240;

    // overwrite colors 246 to 255
    i = 246;
    bmp->colors[i].rgbRed = 255;
    bmp->colors[i].rgbGreen = 251;
    bmp->colors[i].rgbBlue = 240;
    i++; // 247
    bmp->colors[i].rgbRed = 160;
    bmp->colors[i].rgbGreen = 160;
    bmp->colors[i].rgbBlue = 164;
    i++; // 248
    bmp->colors[i].rgbRed = 128;
    bmp->colors[i].rgbGreen = 128;
    bmp->colors[i].rgbBlue = 128;
    i++; // 249
    bmp->colors[i].rgbRed = 255;
    bmp->colors[i].rgbGreen = 0;
    bmp->colors[i].rgbBlue = 0;
    i++; // 250
    bmp->colors[i].rgbRed = 0;
    bmp->colors[i].rgbGreen = 255;
    bmp->colors[i].rgbBlue = 0;
    i++; // 251
    bmp->colors[i].rgbRed = 255;
    bmp->colors[i].rgbGreen = 255;
    bmp->colors[i].rgbBlue = 0;
    i++; // 252
    bmp->colors[i].rgbRed = 0;
    bmp->colors[i].rgbGreen = 0;
    bmp->colors[i].rgbBlue = 255;
    i++; // 253
    bmp->colors[i].rgbRed = 255;
    bmp->colors[i].rgbGreen = 0;
    bmp->colors[i].rgbBlue = 255;
    i++; // 254
    bmp->colors[i].rgbRed = 0;
    bmp->colors[i].rgbGreen = 255;
    bmp->colors[i].rgbBlue = 255;
    i++; // 255
    bmp->colors[i].rgbRed = 255;
    bmp->colors[i].rgbGreen = 255;
    bmp->colors[i].rgbBlue = 255;
    break;
  }
}

/**
 * Create grayscale color table for BMP object
 */
void
bmp_create_grayscale_color_table(Bitmap *bmp)
{
  int i;
  BYTE step_size;

  if (!bmp->colors) return;

  if (bmp->infoHeader.biBitCount != 1)
    step_size = 255 / (bmp->infoHeader.biClrUsed - 1);
  else
    step_size = 255;

  for (i = 0; i < bmp->infoHeader.biClrUsed; ++i) {
    BYTE value = i * step_size;
    RGBQUAD color = {value, value, value, 0};
    bmp->colors[i] = color;
  }
}
/**
 * Convert standard color table to grayscale color table
 */
void bmp_std_2_gray(Bitmap *bmp){
  int i, clrCount;
  if (bmp->infoHeader.biClrUsed != 0)
    clrCount = bmp->infoHeader.biClrUsed;
  else
    clrCount = 0x01<<bmp->infoHeader.biBitCount;

  double gray_value;
  for (i = 0; i<clrCount; i++){
    // Y = R * 0.29900 + G * 0.58700 + B * 0.11400
    gray_value = bmp->colors[i].rgbRed * 0.29900 +
                  bmp->colors[i].rgbGreen * 0.58700 +
                    bmp->colors[i].rgbBlue * 0.11400;
    bmp->colors[i].rgbRed = bmp->colors[i].rgbGreen = bmp->colors[i].rgbBlue = gray_value;
  }
}
