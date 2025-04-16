//
// Created by droc101 on 1/13/25.
//

#ifndef DEFINES_H
#define DEFINES_H

#include <stdbool.h>
#include <stdint.h>
#include "Helpers/List.h"

#pragma region Forward Declarations/Typedefs

// Basic types
typedef uint8_t byte;
typedef uint16_t ushort;
typedef uint32_t uint;
typedef uint64_t ulong;

// Struct forward declarations
typedef struct Vector2 Vector2;
typedef struct Player Player;
typedef struct Wall Wall;
typedef struct Level Level;
typedef struct Actor Actor;
typedef struct Options Options;
typedef struct ActorConnection ActorConnection;

#pragma endregion

#pragma region Utility defines

#define STR(x) #x
#define TO_STR(x) STR(x)
#define PI 3.14159265358979323846

/**
 * Convert degrees to radians
 * @param d Degrees
 */
#define degToRad(d) ((d) * PI / 180)

/**
 * Convert radians to degrees
 * @param r Radians
 */
#define radToDeg(r) ((r) * 180 / PI)

#pragma endregion


#pragma region Struct definitions

// Utility functions are in Structs/Vector2.h
struct Vector2
{
	float x;
	float y;
};

struct Player
{
	Vector2 pos;
	float rotation;
};

// Utility functions are in Structs/wall.h
struct Wall
{
	Vector2 a; // The first point of the wall
	Vector2 b; // The second point of the wall
	char tex[32]; // The texture name
	float uvScale; // The X scale of the texture
	float uvOffset; // The X offset of the texture
};

// Utility functions are in Structs/level.h
struct Level
{
	char name[32];
	short courseNum;

	List *actors; // The list of actors in the level. You must bake this into staticActors before it is used.
	List *walls; // The list of walls in the level. You must bake this into staticWalls before it is used.

	bool hasCeiling;
	char ceilOrSkyTex[32];
	char floorTex[32];

	char music[32];

	uint fogColor;
	double fogStart;
	double fogEnd;

	Player player;
};

// Actor (interactable/moving wall) struct
struct Actor
{
	Vector2 position; // The position of the actor
	float rotation; // The rotation of the actor
	int actorType; // type of actor. do not change this after creation.
	byte paramA; // extra parameters for the actor. saved in level data, so can be used during Init
	byte paramB;
	byte paramC;
	byte paramD;
	char name[64]; // Non-unique name of the actor
	List *ioConnections; // List of I/O connections
};

struct Options
{
	ushort checksum;
	char gameDirectory[260];
} __attribute__((packed));

struct ActorConnection
{
	byte targetInput;
	byte myOutput;
	char outActorName[64];
	char outParamOverride[64];
};

#pragma endregion

#endif //DEFINES_H
