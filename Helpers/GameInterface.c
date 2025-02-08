//
// Created by droc101 on 1/24/25.
//

#include "GameInterface.h"
#include <dlfcn.h>
#include <stdio.h>
#include "../Editor.h"

GetActorNameFunc GetActorName = NULL;
GetActorParamNameFunc GetActorParamName = NULL;
GetActorTypeCountFunc GetActorTypeCount = NULL;

void *libactor = NULL;

bool LoadExecutable()
{
	if (libactor != NULL)
	{
		UnloadExecutable();
	}
	char path[261];
	snprintf(path, 261, "%s%s", options.gameDirectory, "/libactor.so");
	libactor = dlopen(path, RTLD_LAZY);
	if (libactor == NULL)
	{
		printf("Failed to load game executable: %s\n", dlerror());
		UnloadExecutable();
		return false;
	}
	GetActorName = (GetActorNameFunc) dlsym(libactor, "GetActorName");
	GetActorParamName = (GetActorParamNameFunc) dlsym(libactor, "GetActorParamName");
	GetActorTypeCount = (GetActorTypeCountFunc) dlsym(libactor, "GetActorTypeCount");
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
	if (libactor != NULL)
	{
		dlclose(libactor);
	}
	libactor = NULL;
	GetActorName = NULL;
	GetActorParamName = NULL;
	GetActorTypeCount = NULL;
}
