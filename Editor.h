//
// Created by droc101 on 1/15/25.
//

#ifndef EDITOR_H
#define EDITOR_H

#include "defines.h"

typedef enum SelectionType
{
	SELTYPE_NONE,
	SELTYPE_WALL_A,
	SELTYPE_WALL_B,
	SELTYPE_WALL_LINE,
	SELTYPE_ACTOR,
	SELTYPE_PLAYER,
} SelectionType;

typedef enum AddRequestType
{
	ADDREQ_NONE,
	ADDREQ_WALL,
	ADDREQ_ACTOR,
} AddRequestType;

extern AddRequestType addRequest;

extern Level *l;
extern Options options;
extern SelectionType selectionType;
extern int selectionIndex;

extern Vector2 scrollPos;
extern Vector2 scrollPosCentered;
extern double zoom;

extern List *textureList;
extern List *musicList;

void EditorDestroyLevel();

void EditorRenderLevel();

void EditorNewLevel();

void EditorUpdate();

void EditorInit();

void EditorDestroy();

void RescanAssets();

List *ScanAssetFolder(const char *folderName, const char *extension);

#endif //EDITOR_H
