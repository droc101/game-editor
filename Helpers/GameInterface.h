//
// Created by droc101 on 1/24/25.
//

#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "../defines.h"

#define DEF_FILE_VERSION 2

typedef enum ActorDefSignalParamType ActorDefSignalParamType;
typedef enum ActorDefRenderType ActorDefRenderType;

typedef struct ActorDefinition ActorDefinition;
typedef struct ActorDefParam ActorDefParam;
typedef struct ActorDefSignal ActorDefSignal;

enum ActorDefSignalParamType
{
	NONE,
	INT,
	FLOAT,
	STRING,
	ACTOR
};

enum ActorDefRenderType
{
	NORMAL,
	TRIGGER
};

struct ActorDefinition
{
	uint actorType;
	char actorName[64];
	ActorDefRenderType renderType;
	uint numParams;
	ActorDefParam *params;
	uint numInputs;
	ActorDefSignal *inputs;
	uint numOutputs;
	ActorDefSignal *outputs;
};

struct ActorDefParam
{
	char name[64];
	byte min;
	byte max;
};

struct ActorDefSignal
{
	char name[64];
	ActorDefSignalParamType paramType;
};

bool LoadDefFiles();

void UnloadDefFiles();

bool LoadDefFile(char *path);

size_t GetActorTypeCount();

ActorDefinition *GetActorDef(int actor);

ActorDefinition *GetActorDefByLoadIndex(int actor);

ActorDefSignal *GetActorDefOutput(int actor, byte output);

ActorDefSignal *GetActorDefInput(int actor, byte input);

#endif //GAMEINTERFACE_H
