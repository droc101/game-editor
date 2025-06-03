//
// Created by droc101 on 6/2/25.
//

#ifndef TEXTUREREADER_H
#define TEXTUREREADER_H

#include <zlib.h>
#include "../defines.h"

typedef struct Image Image;

struct Image
{
	/// The size of the pixel data (width * height * 4)
	uint pixelDataSize;
	/// The width of the image
	uint width;
	/// The height of the image
	uint height;
	/// The pixel data of the image
	byte *pixelData;
	/// The relative path of the image
	char path[64];
};

enum ImageDataOffsets
{
	IMAGE_SIZE_OFFSET = 0,
	IMAGE_WIDTH_OFFSET = 4,
	IMAGE_HEIGHT_OFFSET = 8,
	IMAGE_ID_OFFSET = 12
};

Image *LoadImage(const char *name);

void FreeImage(Image *img);

#endif //TEXTUREREADER_H
