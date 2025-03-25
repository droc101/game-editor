//
// Created by droc101 on 1/24/25.
//

#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "../defines.h"

#define DEF_FILE_VERSION 1

typedef struct ActorDefinition ActorDefinition;
typedef struct ActorDefParam ActorDefParam;

struct ActorDefinition
{
	uint actorType;
	char actorName[64];
	uint numParams;
	ActorDefParam *params;
};

struct ActorDefParam
{
	char name[64];
	byte min;
	byte max;
};

bool LoadDefFiles();

void UnloadDefFiles();

void LoadDefFile(char *path);

size_t GetActorTypeCount();

ActorDefinition *GetActorDef(int actor);

ActorDefinition *GetActorDefByLoadIndex(int actor);

#endif //GAMEINTERFACE_H
