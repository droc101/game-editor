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
		LoadDefFile(path);
		free(path);
	}
	ListFreeWithData(defs);
	return true;
}

void UnloadDefFiles() {
	for (int i = 0; i < actorDefs->size; i++)
	{
		ActorDefinition *def = ListGet(actorDefs, i);
		free(def->params);
		free(def);
	}
	ListFree(actorDefs);
	actorDefs = NULL;
}

void LoadDefFile(char *path)
{
	FILE *f = fopen(path, "r");
	if (f == NULL)
	{
		printf("Failed to open def file: %s\n", path);
		return;
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
		return;
	}

	if (json_object_get_type(json_object_object_get(root, "version")) != json_type_int)
	{
		printf("Invalid def file version: %s\n", path);
		json_object_put(root);
		return;
	}

	const int version = json_object_get_int(json_object_object_get(root, "version"));
	if (version != DEF_FILE_VERSION)
	{
		printf("Invalid def file version: %d (expected %d)\n", version, DEF_FILE_VERSION);
		json_object_put(root);
		return;
	}

	// loop through "actors" array
	if (json_object_get_type(json_object_object_get(root, "actors")) != json_type_array)
	{
		printf("Invalid def file format: %s\n", path);
		json_object_put(root);
		return;
	}
	const json_object *actors = json_object_object_get(root, "actors");
	const size_t numActors = json_object_array_length(actors);
	for (int i = 0; i < numActors; i++)
	{
		if (json_object_get_type(json_object_array_get_idx(actors, i)) != json_type_object)
		{
			printf("Invalid def file format: %s\n", path);
			json_object_put(root);
			return;
		}
		const json_object *actor = json_object_array_get_idx(actors, i);
		ActorDefinition *def = calloc(1, sizeof(ActorDefinition));
		if (json_object_get_type(json_object_object_get(actor, "id")) != json_type_int ||
			json_object_get_type(json_object_object_get(actor, "name")) != json_type_string ||
			json_object_get_type(json_object_object_get(actor, "params")) != json_type_array)
		{
			printf("Invalid def file format: %s\n", path);
			json_object_put(root);
			return;
		}
		def->actorType = json_object_get_int(json_object_object_get(actor, "id"));
		strncpy(def->actorName, json_object_get_string(json_object_object_get(actor, "name")), 63);
		// if the name already exists, don't add it
		for (int j = 0; j < actorDefs->size; j++)
		{
			ActorDefinition *existing = ListGet(actorDefs, j);
			if (strcmp(existing->actorName, def->actorName) == 0)
			{
				free(def);
				continue;
			}
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
				return;
			}
			const json_object *param = json_object_array_get_idx(json_object_object_get(actor, "params"), p);
			if (json_object_get_type(json_object_object_get(param, "name")) != json_type_string ||
				json_object_get_type(json_object_object_get(param, "min")) != json_type_int ||
				json_object_get_type(json_object_object_get(param, "max")) != json_type_int)
			{
				printf("Invalid def file format: %s\n", path);
				json_object_put(root);
				return;
			}
			strncpy(def->params[p].name, json_object_get_string(json_object_object_get(param, "name")), 63);
			def->params[p].min = json_object_get_int(json_object_object_get(param, "min"));
			def->params[p].max = json_object_get_int(json_object_object_get(param, "max"));
		}
		ListAdd(actorDefs, def);
	}
	json_object_put(root);
	free(fileData);
	fclose(f);
}

size_t GetActorTypeCount()
{
	return actorDefs->size;
}

ActorDefinition *GetActorDef(int actor)
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

ActorDefinition *GetActorDefByLoadIndex(int actor)
{
	return ListGet(actorDefs, actor);
}
