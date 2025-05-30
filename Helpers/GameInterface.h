//
// Created by droc101 on 1/24/25.
//

#ifndef GAMEINTERFACE_H
#define GAMEINTERFACE_H

#include "../defines.h"

#define DEF_FILE_VERSION 2

typedef enum ActorDefSignalParamType ActorDefSignalParamType;
typedef enum ActorDefRenderType ActorDefRenderType;
typedef enum StringParamHint StringParamHint;

typedef struct ActorDefinition ActorDefinition;
typedef struct ActorDefSignal ActorDefSignal;
typedef struct ActorDefParam ActorDefParam;

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

enum StringParamHint
{
	HINT_NONE,
	HINT_MODEL,
	HINT_TEXTURE,
	HINT_SOUND,
	HINT_LEVEL,
	HINT_ACTOR,
	HINT_MUSIC
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

struct ActorDefSignal
{
	char name[64];
	ActorDefSignalParamType paramType;
};

struct ActorDefParam
{
	ParamType type;
	char name[64];
	union
	{
		struct
		{
			byte defaultValue;
			byte minValue;
			byte maxValue;
		} byteDef;
		struct
		{
			int defaultValue;
			int minValue;
			int maxValue;
		} intDef;
		struct
		{
			float defaultValue;
			float minValue;
			float maxValue;
			float step;
		} floatDef;
		struct
		{
			bool defaultValue;
		} boolDef;
		struct
		{
			char defaultValue[64];
			StringParamHint hint;
		} stringDef;
		struct
		{
			Color defaultValue;
			bool hasAlpha;
		} colorDef;
	};
};

bool LoadDefFiles();

void UnloadDefFiles();

bool LoadDefFile(char *path);

size_t GetActorTypeCount();

ActorDefinition *GetActorDef(int actor);

ActorDefinition *GetActorDefByLoadIndex(int actor);

ActorDefSignal *GetActorDefOutput(int actor, byte output);

ActorDefSignal *GetActorDefInput(int actor, byte input);

ActorDefParam *GetActorDefParam(int actor, const char *paramName);

#endif //GAMEINTERFACE_H
