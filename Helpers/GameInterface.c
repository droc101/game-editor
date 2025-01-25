//
// Created by droc101 on 1/24/25.
//

#include "GameInterface.h"
#include <stdio.h>
#include <dlfcn.h>
#include "../Editor.h"

GetActorNameFunc GetActorName = NULL;
GetActorParamNameFunc GetActorParamName = NULL;
GetActorTypeCountFunc GetActorTypeCount = NULL;

void *game = NULL;

bool LoadExecutable()
{
	if (game != NULL)
	{
		UnloadExecutable();
	}
	char path[261];
	snprintf(path, 261, "%s%s", options.gameDirectory, "/libactor.so");
	game = dlopen(path, RTLD_LAZY);
	if (game == NULL)
	{
		printf("Failed to load game executable: %s\n", dlerror());
		UnloadExecutable();
		return false;
	}
	GetActorName = (GetActorNameFunc) dlsym(game, "GetActorName");
	GetActorParamName = (GetActorParamNameFunc) dlsym(game, "GetActorParamName");
	GetActorTypeCount = (GetActorTypeCountFunc) dlsym(game, "GetActorTypeCount");
	if (GetActorName == NULL || GetActorParamName == NULL || GetActorTypeCount == NULL)
	{
		UnloadExecutable();
		printf("Failed to load game functions: %s\n", dlerror());
		return false;
	}
	return true;
}

void UnloadExecutable()
{
	if (game != NULL)
	{
		dlclose(game);
	}
	game = NULL;
	GetActorName = NULL;
	GetActorParamName = NULL;
	GetActorTypeCount = NULL;
}
