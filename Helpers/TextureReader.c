//
// Created by droc101 on 6/2/25.
//

#include "TextureReader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Editor.h"

byte *DecompressAsset(const char *absolutePath)
{
	FILE *file = fopen(absolutePath, "rb");
	if (file == NULL)
	{
		printf("Failed to open asset file: %s\n", absolutePath);
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	const size_t fileSize = ftell(file);

	byte *asset = malloc(fileSize);
	fseek(file, 0, SEEK_SET);
	const size_t bytesRead = fread(asset, 1, fileSize, file);
	if (bytesRead != fileSize)
	{
		free(asset);
		fclose(file);
		printf("Failed to read asset file: %s\n", absolutePath);
		return NULL;
	}

	fseek(file, 0, SEEK_SET);

	uint compressedSize;
	fread(&compressedSize, sizeof(uint), 1, file);
	uint decompressedSize;
	fread(&decompressedSize, sizeof(uint), 1, file);
	fseek(file, sizeof(uint), SEEK_CUR);
	uint assetType;
	fread(&assetType, sizeof(uint), 1, file);

	fclose(file);

	// Allocate memory for the decompressed data
	byte *decompressedData = malloc(decompressedSize);

	z_stream stream = {0};

	// Initialize the zlib stream
	stream.next_in = asset + (sizeof(uint) * 4); // skip header
	stream.avail_in = compressedSize;
	stream.next_out = decompressedData;
	stream.avail_out = decompressedSize;

	// Initialize the zlib stream
	if (inflateInit2(&stream, MAX_WBITS | 16) != Z_OK)
	{
		free(decompressedData);
		free(asset);
		printf("Failed to initialize zlib stream: %s\n", stream.msg);
		return NULL;
	}

	// Decompress the data
	int inflateReturnValue = inflate(&stream, Z_NO_FLUSH);
	while (inflateReturnValue != Z_STREAM_END)
	{
		if (inflateReturnValue != Z_OK)
		{
			free(decompressedData);
			free(asset);
			printf("Failed to decompress zlib stream: %s\n", stream.msg);
			return NULL;
		}
		inflateReturnValue = inflate(&stream, Z_NO_FLUSH);
	}

	// Clean up the zlib stream
	if (inflateEnd(&stream) != Z_OK)
	{
		free(decompressedData);
		free(asset);
		printf("Failed to end zlib stream: %s\n", stream.msg);
		return NULL;
	}

	free(asset);

	return decompressedData;
}

void GenFallbackImage(Image *src)
{
	src->width = 64;
	src->height = 64;
	src->pixelDataSize = 64 * 64 * 4;
	src->pixelData = malloc(src->pixelDataSize);

	for (int x = 0; x < 64; x++)
	{
		for (int y = 0; y < 64; y++)
		{
			if ((x < 32) ^ (y < 32))
			{
				src->pixelData[(x + y * 64) * 4] = 0;
				src->pixelData[(x + y * 64) * 4 + 1] = 0;
				src->pixelData[(x + y * 64) * 4 + 2] = 0;
				src->pixelData[(x + y * 64) * 4 + 3] = 255;
			} else
			{
				src->pixelData[(x + y * 64) * 4] = 255;
				src->pixelData[(x + y * 64) * 4 + 1] = 0;
				src->pixelData[(x + y * 64) * 4 + 2] = 255;
				src->pixelData[(x + y * 64) * 4 + 3] = 255;
			}
		}
	}
}

uint ReadUintA(const byte *data, const size_t offset)
{
	uint i;
	memcpy(&i, data + offset, sizeof(uint));
	return i;
}

Image *LoadImage(const char *name)
{
	Image *img = malloc(sizeof(Image));

	const size_t length = strlen(name) + strlen(options.gameDirectory) + strlen("/assets/texture/.gtex") + 1;
	char path[length];
	snprintf(path, length, "%s/assets/texture/%s.gtex", options.gameDirectory, name);

	byte *textureAsset = DecompressAsset(path);
	if (textureAsset == NULL)
	{
		GenFallbackImage(img);
	} else
	{
		img->pixelDataSize = ReadUintA(textureAsset, IMAGE_SIZE_OFFSET);
		img->width = ReadUintA(textureAsset, IMAGE_WIDTH_OFFSET);
		img->height = ReadUintA(textureAsset, IMAGE_HEIGHT_OFFSET);
		img->pixelData = malloc(img->pixelDataSize);
		memcpy(img->pixelData, textureAsset + sizeof(uint) * 4, img->pixelDataSize);
		strncpy(img->path, name, 64);
	}
	free(textureAsset);
	return img;
}

void FreeImage(Image *img)
{
	if (img != NULL)
	{
		free(img->pixelData);
		free(img);
	}
}
