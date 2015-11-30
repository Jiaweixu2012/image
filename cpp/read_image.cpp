#include <cstdlib>
#include <iostream>
#include "bmpfile.h"
// #include "hexdump.h"

using namespace std;

int main(){
	Bitmap *image = new Bitmap("source.bmp");

	printf("Height : %d\n", image->height);
	printf("Width : %d\n", image->width);
	// hexDump((char*)"fileHeader", &(image->fileHeader), sizeof(image->fileHeader));
	// hexDump((char*)"infoHeader", &(image->infoHeader), sizeof(image->infoHeader));

	image->write_to_file("result.bmp");
	delete image;

	cout << "Press enter to continue..." << endl;
	getchar();
	return 0;
}
