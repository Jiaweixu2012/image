#include <stdio.h>
#include "bmpfile.h"

int main(){
	int height, width;
	BYTE* image;

	image = readBmpFile("source.bmp");

	height = infoHeader.biHeight;
	width = infoHeader.biWidth;
	printf("Height : %d\n", height);
	printf("Width : %d\n", width);

	//int i, j;
		for(int i = 0 ; i<width; i++)
			for(int j = 0 ; j< height; j++)
				if( i == j )
					image[i*width+j] = 0;

	writeBmpFile("result.bmp", image);
	free(image);

	printf("Press enter to continue...");
	getchar();
	return 0;
}
