//
// Created by droc101 on 10/27/24.
//

#include "Options.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void DefaultOptions(Options *options)
{
	options->gameDirectory[0] = '\0';
}

ushort GetOptionsChecksum(Options *options)
{
	const byte *data = (byte *)options;
	ushort checksum = 0;
	for (int i = sizeof(ushort); i < sizeof(Options) - sizeof(ushort); i++)
	{
		checksum += data[i];
	}
	return checksum;
}

char *GetOptionsPath()
{
	return "./editor_options.bin";
}

void LoadOptions(Options *options)
{
	const char *filePath = GetOptionsPath();

	FILE *file = fopen(filePath, "rb");
	if (file == NULL)
	{
		printf("Options file not found, using default options\n");
		DefaultOptions(options);
	} else
	{
		fseek(file, 0, SEEK_END);
		const int fileLen = ftell(file);

		// if the file is the wrong size, just use the default options
		if (fileLen != sizeof(Options))
		{
			printf("Options file is invalid, using defaults\n");
			DefaultOptions(options);
			fclose(file);
			return;
		}

		printf("Valid options file found, loading options\n");

		fseek(file, 0, SEEK_SET);
		fread(options, sizeof(Options), 1, file);

		if (options->checksum != GetOptionsChecksum(options))
		{
			printf("Options file checksum invalid, using defaults\n");
			DefaultOptions(options);
		}

		fclose(file);
	}
}

void SaveOptions(Options *options)
{
	options->checksum = GetOptionsChecksum(options);

	const char *filePath = GetOptionsPath();

	FILE *file = fopen(filePath, "wb");
	fwrite(options, sizeof(Options), 1, file);
	fclose(file);
}

bool IsValidGameDirectory(Options *options)
{
	/*
	 *Directory must contain:
	 * - game or game.exe
	 * - assets folder
	 */

	char *gameDirectory = options->gameDirectory;
	char *gamePath = malloc(strlen(gameDirectory) + 7);
	strcpy(gamePath, gameDirectory);
	strcat(gamePath, "/game");
	char *gameExePath = malloc(strlen(gameDirectory) + 11);
	strcpy(gameExePath, gameDirectory);
	strcat(gameExePath, "/game.exe");

	if (access(gamePath, F_OK) == -1 && access(gameExePath, F_OK) == -1)
	{
		printf("Invalid game directory: %s\n", gameDirectory);
		free(gamePath);
		free(gameExePath);
		return false;
	}

	char *assetsPath = malloc(strlen(gameDirectory) + 8);
	strcpy(assetsPath, gameDirectory);
	strcat(assetsPath, "/assets");

	if (access(assetsPath, F_OK) == -1)
	{
		printf("Invalid game directory: %s\n", gameDirectory);
		free(gamePath);
		free(gameExePath);
		free(assetsPath);
		return false;
	}

	free(gamePath);
	free(gameExePath);
	free(assetsPath);

	return true;
}
