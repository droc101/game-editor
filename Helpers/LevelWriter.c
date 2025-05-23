//
// Created by droc101 on 1/17/25.
//

#include "LevelWriter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void WriteLevel(const Level *level, const char *path)
{
	FILE *file = fopen(path, "wb");
	if (file == NULL)
	{
		printf("Failed to open file %s\n", path);
		return;
	}

	// Write the level name
	fwrite(level->name, sizeof(char), 32, file);
	// Write the course number
	fwrite(&level->courseNum, sizeof(short), 1, file);
	// Write the has ceiling flag
	fwrite(&level->hasCeiling, sizeof(byte), 1, file);
	// Write the floor and ceiling textures
	fwrite(level->ceilOrSkyTex, sizeof(char), 32, file);
	fwrite(level->floorTex, sizeof(char), 32, file);
	// Write the music
	fwrite(level->music, sizeof(char), 32, file);
	// Write the fog color and start/end
	fwrite(&level->fogColor, sizeof(uint), 1, file);
	fwrite(&level->fogStart, sizeof(float), 1, file);
	fwrite(&level->fogEnd, sizeof(float), 1, file);
	// Write the player position and rotation
	fwrite(&level->player.pos.x, sizeof(float), 1, file);
	fwrite(&level->player.pos.y, sizeof(float), 1, file);
	fwrite(&level->player.rotation, sizeof(float), 1, file);

	// Write the number of actors
	const uint actorCount = level->actors->size;
	fwrite(&actorCount, sizeof(uint), 1, file);

	// Write the actors
	for (int i = 0; i < actorCount; i++)
	{
		const Actor *actor = ListGet(level->actors, i);
		fwrite(&actor->position.x, sizeof(float), 1, file);
		fwrite(&actor->position.y, sizeof(float), 1, file);
		fwrite(&actor->rotation, sizeof(float), 1, file);
		fwrite(&actor->actorType, sizeof(int), 1, file);
		fwrite(&actor->paramA, sizeof(byte), 1, file);
		fwrite(&actor->paramB, sizeof(byte), 1, file);
		fwrite(&actor->paramC, sizeof(byte), 1, file);
		fwrite(&actor->paramD, sizeof(byte), 1, file);
		fwrite(&actor->name, sizeof(char), 64, file);
		uint connectionCount = actor->ioConnections->size;
		fwrite(&connectionCount, sizeof(uint), 1, file);
		for (int j = 0; j < connectionCount; j++)
		{
			const ActorConnection *connection = ListGet(actor->ioConnections, j);
			fwrite(&connection->myOutput, sizeof(byte), 1, file);
			fwrite(&connection->outActorName, sizeof(char) * 64, 1, file);
			fwrite(&connection->targetInput, sizeof(byte), 1, file);
			fwrite(&connection->outParamOverride, sizeof(Param), 1, file);
		}
	}

	// Write the number of walls
	const uint wallCount = level->walls->size;
	fwrite(&wallCount, sizeof(uint), 1, file);

	// Write the walls
	for (int i = 0; i < wallCount; i++)
	{
		const Wall *wall = ListGet(level->walls, i);
		fwrite(&wall->a.x, sizeof(float), 1, file);
		fwrite(&wall->a.y, sizeof(float), 1, file);
		fwrite(&wall->b.x, sizeof(float), 1, file);
		fwrite(&wall->b.y, sizeof(float), 1, file);
		fwrite(wall->tex, sizeof(char), 32, file);
		fwrite(&wall->uvScale, sizeof(float), 1, file);
		fwrite(&wall->uvOffset, sizeof(float), 1, file);
	}

	fclose(file);
}

Level *ReadLevel(const char *path)
{
	FILE *file = fopen(path, "rb");
	if (file == NULL)
	{
		printf("Failed to open file %s\n", path);
		return NULL;
	}

	Level *level = malloc(sizeof(Level));
	level->walls = CreateList();
	level->actors = CreateList();

	fread(&level->name, sizeof(char), 32, file);
	fread(&level->courseNum, sizeof(short), 1, file);
	fread(&level->hasCeiling, sizeof(byte), 1, file);
	fread(&level->ceilOrSkyTex, sizeof(char), 32, file);
	fread(&level->floorTex, sizeof(char), 32, file);
	fread(&level->music, sizeof(char), 32, file);
	fread(&level->fogColor, sizeof(uint), 1, file);
	fread(&level->fogStart, sizeof(float), 1, file);
	fread(&level->fogEnd, sizeof(float), 1, file);
	fread(&level->player.pos.x, sizeof(float), 1, file);
	fread(&level->player.pos.y, sizeof(float), 1, file);
	fread(&level->player.rotation, sizeof(float), 1, file);

	uint actorCount = 0;
	fread(&actorCount, sizeof(uint), 1, file);

	for (int i = 0; i < actorCount; i++)
	{
		Actor *actor = malloc(sizeof(Actor));
		actor->ioConnections = CreateList();
		fread(&actor->position.x, sizeof(float), 1, file);
		fread(&actor->position.y, sizeof(float), 1, file);
		fread(&actor->rotation, sizeof(float), 1, file);
		fread(&actor->actorType, sizeof(int), 1, file);
		fread(&actor->paramA, sizeof(byte), 1, file);
		fread(&actor->paramB, sizeof(byte), 1, file);
		fread(&actor->paramC, sizeof(byte), 1, file);
		fread(&actor->paramD, sizeof(byte), 1, file);
		fread(&actor->name, sizeof(char) * 64, 1, file);
		uint connectionCount = 0;
		fread(&connectionCount, sizeof(uint), 1, file);
		for (int j = 0; j < connectionCount; j++)
		{
			ActorConnection *connection = malloc(sizeof(ActorConnection));
			fread(&connection->myOutput, sizeof(byte), 1, file);
			fread(&connection->outActorName, sizeof(char) * 64, 1, file);
			fread(&connection->targetInput, sizeof(byte), 1, file);
			fread(&connection->outParamOverride, sizeof(Param), 1, file);
			ListAdd(actor->ioConnections, connection);
		}

		ListAdd(level->actors, actor);
	}

	uint wallCount = 0;
	fread(&wallCount, sizeof(uint), 1, file);

	for (int i = 0; i < wallCount; i++)
	{
		Wall *wall = malloc(sizeof(Wall));
		fread(&wall->a.x, sizeof(float), 1, file);
		fread(&wall->a.y, sizeof(float), 1, file);
		fread(&wall->b.x, sizeof(float), 1, file);
		fread(&wall->b.y, sizeof(float), 1, file);
		fread(&wall->tex, sizeof(char), 32, file);
		fread(&wall->uvScale, sizeof(float), 1, file);
		fread(&wall->uvOffset, sizeof(float), 1, file);

		ListAdd(level->walls, wall);
	}

	return level;
}
