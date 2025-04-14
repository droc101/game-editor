//
// Created by droc101 on 1/24/25.
//

#include "GameInterface.h"

#include <json-c/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../Editor.h"

List *actorDefs = NULL;

bool LoadDefFiles()
{
	if (actorDefs != NULL)
	{
		UnloadDefFiles();
	}

	actorDefs = CreateList();
	List *defs = ScanAssetFolder("defs", ".def");
	for (int i = 0; i < defs->size; i++)
	{
		char *path = malloc(strlen(options.gameDirectory) + strlen("/assets/defs/") + strlen(ListGet(defs, i)) + strlen(".def") + 1);
		strcpy(path, options.gameDirectory);
		strcat(path, "/assets/defs/");
		strcat(path, ListGet(defs, i));
		strcat(path, ".def");
		const bool success = LoadDefFile(path);
		free(path);
		if (!success)
		{
			return false;
		}
	}
	ListFreeWithData(defs);

	printf("Loaded %d actor definitions\n", actorDefs->size);

	return true;
}

void UnloadDefFiles()
{
	for (int i = 0; i < actorDefs->size; i++)
	{
		ActorDefinition *def = ListGet(actorDefs, i);
		free(def->params);
		free(def->inputs);
		free(def->outputs);
		free(def);
	}
	ListFree(actorDefs);
	actorDefs = NULL;
}

bool LoadDefFile(char *path)
{
	FILE *f = fopen(path, "r");
	if (f == NULL)
	{
		printf("Failed to open def file: %s\n", path);
		return false;
	}
	fseek(f, 0, SEEK_END);
	const size_t fileSize = ftell(f);
	fseek(f, 0, SEEK_SET);
	char *fileData = malloc(fileSize + 1);
	fread(fileData, 1, fileSize, f);
	json_object *root = json_tokener_parse(fileData);

	if (root == NULL)
	{
		printf("Failed to parse def file: %s\n", path);
		free(fileData);
		fclose(f);
		return false;
	}

	if (json_object_get_type(json_object_object_get(root, "version")) != json_type_int)
	{
		printf("Invalid def file version: %s (expected %d)\n", path, DEF_FILE_VERSION);
		json_object_put(root);
		return false;
	}

	const int version = json_object_get_int(json_object_object_get(root, "version"));
	if (version != DEF_FILE_VERSION)
	{
		printf("Invalid def file version: %d (expected %d)\n", version, DEF_FILE_VERSION);
		json_object_put(root);
		return false;
	}

	// loop through "actors" array
	if (json_object_get_type(json_object_object_get(root, "actors")) != json_type_array)
	{
		printf("Invalid def file format: %s (invalid actors key)\n", path);
		json_object_put(root);
		return false;
	}
	const json_object *actors = json_object_object_get(root, "actors");
	const size_t numActors = json_object_array_length(actors);
	for (int i = 0; i < numActors; i++)
	{
		if (json_object_get_type(json_object_array_get_idx(actors, i)) != json_type_object)
		{
			printf("Invalid def file format: %s (invalid actor definition - not object)\n", path);
			json_object_put(root);
			return false;
		}
		const json_object *actor = json_object_array_get_idx(actors, i);
		ActorDefinition *def = calloc(1, sizeof(ActorDefinition));
		if (json_object_get_type(json_object_object_get(actor, "id")) != json_type_int ||
			json_object_get_type(json_object_object_get(actor, "name")) != json_type_string ||
			json_object_get_type(json_object_object_get(actor, "params")) != json_type_array ||
			json_object_get_type(json_object_object_get(actor, "inputs")) != json_type_array ||
			json_object_get_type(json_object_object_get(actor, "outputs")) != json_type_array ||
			json_object_get_type(json_object_object_get(actor, "render_type")) != json_type_string)
		{
			printf("Invalid def file format: %s (invalid actor definition - missing or incorrect keys)\n", path);
			json_object_put(root);
			return false;
		}
		def->actorType = json_object_get_int(json_object_object_get(actor, "id"));
		strncpy(def->actorName, json_object_get_string(json_object_object_get(actor, "name")), 63);
		// if the name already exists, don't add it
		for (int j = 0; j < actorDefs->size; j++)
		{
			const ActorDefinition *existing = ListGet(actorDefs, j);
			if (strcmp(existing->actorName, def->actorName) == 0)
			{
				free(def);
				def = NULL;
				break;
			}
		}
		if (!def) return false;

		const char *renderTypeString = json_object_get_string(json_object_object_get(actor, "render_type"));
		if (strcmp("trigger", renderTypeString) == 0)
		{
			def->renderType = TRIGGER;
		} else
		{
			def->renderType = NORMAL;
		}

		const size_t numParams = json_object_array_length(json_object_object_get(actor, "params"));
		def->numParams = numParams;
		def->params = calloc(numParams, sizeof(ActorDefParam));
		for (int p = 0; p < numParams; p++)
		{
			if (json_object_get_type(json_object_array_get_idx(json_object_object_get(actor, "params"), p)) != json_type_object)
			{
				printf("Invalid def file format: %s\n", path);
				json_object_put(root);
				return false;
			}
			const json_object *param = json_object_array_get_idx(json_object_object_get(actor, "params"), p);
			if (json_object_get_type(json_object_object_get(param, "name")) != json_type_string ||
				json_object_get_type(json_object_object_get(param, "min")) != json_type_int ||
				json_object_get_type(json_object_object_get(param, "max")) != json_type_int)
			{
				printf("Invalid def file format: %s (invalid actor parameter - missing or incorrect keys)\n", path);
				json_object_put(root);
				return false;
			}
			strncpy(def->params[p].name, json_object_get_string(json_object_object_get(param, "name")), 63);
			def->params[p].min = json_object_get_int(json_object_object_get(param, "min"));
			def->params[p].max = json_object_get_int(json_object_object_get(param, "max"));
		}

		const size_t numInputs = json_object_array_length(json_object_object_get(actor, "inputs"));
		def->numInputs = numInputs;
		def->inputs = calloc(numInputs, sizeof(ActorDefSignal));
		for (int s = 0; s < numInputs; s++)
		{
			if (json_object_get_type(json_object_array_get_idx(json_object_object_get(actor, "inputs"), s)) != json_type_object)
			{
				printf("Invalid def file format: %s\n", path);
				json_object_put(root);
				return false;
			}
			const json_object *signal = json_object_array_get_idx(json_object_object_get(actor, "inputs"), s);
			if (json_object_get_type(json_object_object_get(signal, "name")) != json_type_string ||
				json_object_get_type(json_object_object_get(signal, "paramType")) != json_type_string)
			{
				printf("Invalid def file format: %s (invalid actor input - missing or incorrect keys)\n", path);
				json_object_put(root);
				return false;
			}
			strncpy(def->inputs[s].name, json_object_get_string(json_object_object_get(signal, "name")), 63);
			const char *paramType = json_object_get_string(json_object_object_get(signal, "paramType"));
			if (strcmp(paramType, "int") == 0)
			{
				def->inputs[s].paramType = INT;
			}
			else if (strcmp(paramType, "float") == 0)
			{
				def->inputs[s].paramType = FLOAT;
			}
			else if (strcmp(paramType, "string") == 0)
			{
				def->inputs[s].paramType = STRING;
			}
			else if (strcmp(paramType, "actor") == 0)
			{
				def->inputs[s].paramType = ACTOR;
			}
			else
			{
				def->inputs[s].paramType = NONE;
			}
		}

		const size_t numOutputs = json_object_array_length(json_object_object_get(actor, "outputs"));
		def->numOutputs = numOutputs;
		def->outputs = calloc(numOutputs, sizeof(ActorDefSignal));
		for (int s = 0; s < numOutputs; s++)
		{
			if (json_object_get_type(json_object_array_get_idx(json_object_object_get(actor, "outputs"), s)) != json_type_object)
			{
				printf("Invalid def file format: %s\n", path);
				json_object_put(root);
				return false;
			}
			const json_object *signal = json_object_array_get_idx(json_object_object_get(actor, "outputs"), s);
			if (json_object_get_type(json_object_object_get(signal, "name")) != json_type_string ||
				json_object_get_type(json_object_object_get(signal, "paramType")) != json_type_string)
			{
				printf("Invalid def file format: %s (invalid actor output - missing or incorrect keys)\n", path);
				json_object_put(root);
				return false;
			}
			strncpy(def->outputs[s].name, json_object_get_string(json_object_object_get(signal, "name")), 63);
			const char *paramType = json_object_get_string(json_object_object_get(signal, "paramType"));
			if (strcmp(paramType, "int") == 0)
			{
				def->outputs[s].paramType = INT;
			}
			else if (strcmp(paramType, "float") == 0)
			{
				def->outputs[s].paramType = FLOAT;
			}
			else if (strcmp(paramType, "string") == 0)
			{
				def->outputs[s].paramType = STRING;
			}
			else if (strcmp(paramType, "actor") == 0)
			{
				def->outputs[s].paramType = ACTOR;
			}
			else
			{
				def->outputs[s].paramType = NONE;
			}
		}

		ListAdd(actorDefs, def);
	}
	json_object_put(root);
	free(fileData);
	fclose(f);

	return true;
}

size_t GetActorTypeCount()
{
	return actorDefs->size;
}

ActorDefinition *GetActorDef(const int actor)
{
	for (int i = 0; i < actorDefs->size; i++)
	{
		ActorDefinition *def = ListGet(actorDefs, i);
		if (def->actorType == actor)
		{
			return def;
		}
	}
	return NULL;
}

ActorDefinition *GetActorDefByLoadIndex(const int actor)
{
	return ListGet(actorDefs, actor);
}

ActorDefSignal *GetActorDefOutput(const int actor, const byte output)
{
	if (output >= GetActorDef(actor)->numOutputs) return NULL;
	return GetActorDef(actor)->outputs + output;
}

ActorDefSignal *GetActorDefInput(const int actor, const byte input)
{
    if (input >= GetActorDef(actor)->numInputs) return NULL;
    return GetActorDef(actor)->inputs + input;
}
