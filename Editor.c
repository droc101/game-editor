//
// Created by droc101 on 1/15/25.
//

#include "Editor.h"
#include <stdlib.h>
#include <string.h>
#include <tgmath.h>

#include "defines.h"
#include "Helpers/Drawing.h"
#include "Helpers/GameInterface.h"
#include "Helpers/Input.h"
#include "Helpers/KVList.h"
#include "Helpers/Options.h"
#include "UI/MainWindow.h"

Level *l;
Options options;

AddRequestType addRequest;

Vector2 scrollPos = {0};
Vector2 scrollPosCentered = {0};
double zoom = 30.0;

SelectionType selectionType = SELTYPE_NONE;
int selectionIndex = -1;

SelectionType hoverType = SELTYPE_NONE;
int hoverIndex = -1;

bool isDragging = false;
Vector2 wallDragAOffset = {0};
Vector2 wallDragBOffset = {0};

List *textureList = NULL;
List *musicList = NULL;
List *modelList = NULL;
List *soundList = NULL;
List *levelList = NULL;

const GdkRGBA bg = Color(0x123456FF);
const GdkRGBA grid = Color(0x808080FF);
const GdkRGBA xAxis = Color(0xFF0000FF);
const GdkRGBA zAxis = Color(0x0000FFFF);
const GdkRGBA selectionOutline = Color(0xFF0000FF);
const GdkRGBA wallLine = Color(0xFFFFFF80);
const GdkRGBA wallLineHover = Color(-1);
const GdkRGBA wallNode = Color(0x0000FFFF);
const GdkRGBA wallNodeHover = Color(0xFFFFFF40);
const GdkRGBA actorRotationLine = Color(0x808000FF);
const GdkRGBA actorNode = Color(0xFFFF00FF);
const GdkRGBA actorNodeHover = Color(0x00000040);
const GdkRGBA playerRotationLine = Color(0x008000FF);
const GdkRGBA playerNode = Color(0x00FF00FF);
const GdkRGBA playerNodeHover = Color(0x00000040);
const GdkRGBA triggerNode = Color(0xFF00FFFF);
const GdkRGBA triggerNodeHover = Color(0x00000040);
const GdkRGBA triggerArea = Color(0xFF00FF40);
const GdkRGBA triggerCommand = Color(0xFFFFFF80);

const float snaps[] = {0.0625, 0.125, 0.25, 0.5, 1.0, 2.0, 4.0, 8.0};
const float snapsInv[] = {16.0, 8.0, 4.0, 2.0, 1.0, 0.5, 0.25, 0.125}; // 1.0 / snaps[i]
int snapIndex = 4; // 4 = 1.0
const int snapCount = sizeof(snaps) / sizeof(snaps[0]);

void EditorInit()
{
	LoadOptions(&options);
	EditorNewLevel();
}

void EditorDestroy()
{
	SaveOptions(&options);
	EditorDestroyLevel();
	UnloadDefFiles();
}

bool RescanAssets()
{
	if (!LoadDefFiles())
	{
		printf("Failed to load game definitions!\n");
		return false;
	}
	if (textureList != NULL)
	{
		ListAndContentsFree(textureList, true);
		ListAndContentsFree(musicList, true);
	}

	textureList = ScanAssetFolder("texture", ".gtex");
	musicList = ScanAssetFolder("audio", ".gmus");
	modelList = ScanAssetFolder("model", ".gmdl");
	soundList = ScanAssetFolder("audio", ".gsnd");
	levelList = ScanAssetFolder("level", ".gmap");

	return true;
}

List *ScanAssetFolder(const char *folderName, const char *extension)
{
	List *fileList = malloc(sizeof(List));
	ListCreate(fileList);
	char *levelDataPath = malloc(strlen(options.gameDirectory) + strlen(folderName) + 10);
	strcpy(levelDataPath, options.gameDirectory);
	strcat(levelDataPath, "/assets/");
	strcat(levelDataPath, folderName);

	// Get the name of all gmap files in the level directory
	DIR *dir = opendir(levelDataPath);
	if (dir == NULL)
	{
		printf("Failed to open level directory: %s\n", levelDataPath);
		free(levelDataPath);
		return fileList;
	}
	struct dirent *ent;
	while ((ent = readdir(dir)) != NULL)
	{
		if (strstr(ent->d_name, extension) != NULL)
		{
			char *levelName = malloc(strlen(ent->d_name) + 1);
			strcpy(levelName, ent->d_name);
			// Remove the .gmap extension
			levelName[strlen(levelName) - strlen(extension)] = '\0';
			ListAdd(fileList, levelName);
		}
	}
	free(levelDataPath);
	closedir(dir);

	return fileList;
}

void EditorDestroyLevel()
{
	if (l == NULL)
	{
		return;
	}
	for (int i = 0; i < l->actors.length; i++)
	{
		const Actor *actor = ListGet(l->actors, i);
		ListFree(&actor->ioConnections, false);
		KvListDestroy(&actor->params);
	}
	ListAndContentsFree(&l->actors, false);
	ListAndContentsFree(&l->walls, false);
	free(l);
	l = NULL;
}

void EditorNewLevel()
{
	if (l != NULL)
	{
		EditorDestroyLevel();
	}

	l = malloc(sizeof(Level));
	memset(l, 0, sizeof(Level));

	ListCreate(&l->walls);
	ListCreate(&l->actors);
	strcpy(l->name, "Unnamed Level");
	l->courseNum = -1;
	l->hasCeiling = false;
	strcpy(l->ceilOrSkyTex, "level_sky_test");
	strcpy(l->floorTex, "level_floor_test");
	strcpy(l->music, "none");
	l->fogColor = 0x99c0f1FF;
	l->fogStart = 50.0f;
	l->fogEnd = 100.0f;
	l->player.rotation = degToRad(-90.0);
}

Vector2 WorldToScreen(const Vector2 wp)
{
	return v2(round((wp.x * zoom) + scrollPosCentered.x), round((wp.y * zoom) + scrollPosCentered.y));
}

Vector2 WorldToScreenSize(const Vector2 wp)
{
	return v2(wp.x * zoom, wp.y * zoom);
}

Vector2 ScreenToWorld(const Vector2 sp)
{
	return v2((sp.x - scrollPosCentered.x) / zoom, (sp.y - scrollPosCentered.y) / zoom);
}

Vector2 ScreenToWorldSnapped(const Vector2 sp)
{
	const float snapInvValue = snapsInv[snapIndex];
	Vector2 realPos = ScreenToWorld(sp);
	realPos.x = round(realPos.x * snapInvValue) / snapInvValue;
	realPos.y = round(realPos.y * snapInvValue) / snapInvValue;
	return realPos;
}

float roundToGrid(const float x)
{
	const float snapInvValue = snapsInv[snapIndex];
	return round(x * snapInvValue) / snapInvValue;
}

float floorToGrid(const float x)
{
	const float snapInvValue = snapsInv[snapIndex];
	return floorf(x * snapInvValue) / snapInvValue;
}

float ceilToGrid(const float x)
{
	const float snapInvValue = snapsInv[snapIndex];
	return ceilf(x * snapInvValue) / snapInvValue;
}

void RenderGrid()
{
	// TODO: Draw grid using snap size instead of only on integers
	const int gridSpacing = zoom;
	const int gridOffsetX = (int)scrollPosCentered.x % gridSpacing;
	const int gridOffsetY = (int)scrollPosCentered.y % gridSpacing;

	for (int x = gridOffsetX; x < WindowWidth(); x += gridSpacing)
	{
		DrawLine(v2(x, 0), v2(x, WindowHeight()), grid, .5f);
	}
	for (int y = gridOffsetY; y < WindowHeight(); y += gridSpacing)
	{
		DrawLine(v2(0, y), v2(WindowWidth(), y), grid, .5f);
	}

	DrawLine(v2(scrollPosCentered.x, 0), v2(scrollPosCentered.x, WindowHeight()), zAxis, 2.0f);
	DrawLine(v2(0, scrollPosCentered.y), v2(WindowWidth(), scrollPosCentered.y), xAxis, 2.0f);
}

void ProcessHover()
{
	hoverType = SELTYPE_NONE;

	for (int w = 0; w < l->walls.length; w++)
	{
		const Wall *wall = ListGet(l->walls, w);

		const Vector2 scaledWallA = WorldToScreen(wall->a);
		const Vector2 scaledWallB = WorldToScreen(wall->b);

		if (Vector2Distance(scaledWallA, GetLocalMousePos()) < 10)
		{
			hoverType = SELTYPE_WALL_A;
			hoverIndex = w;
			break;
		}
		if (Vector2Distance(scaledWallB, GetLocalMousePos()) < 10)
		{
			hoverType = SELTYPE_WALL_B;
			hoverIndex = w;
			break;
		}
		if (Vector2DistanceToLine(scaledWallA, scaledWallB, GetLocalMousePos()) < 10)
		{
			hoverType = SELTYPE_WALL_LINE;
			hoverIndex = w;
			break;
		}
	}

	for (int a = 0; a < l->actors.length; a++)
	{
		const Actor *act = ListGet(l->actors, a);
		const Vector2 scaledActorPos = WorldToScreen(act->position);
		if (Vector2Distance(scaledActorPos, GetLocalMousePos()) < 10)
		{
			hoverType = SELTYPE_ACTOR;
			hoverIndex = a;
			break;
		}
	}

	const Vector2 scaledPlayerPos = WorldToScreen(l->player.pos);
	if (Vector2Distance(scaledPlayerPos, GetLocalMousePos()) < 10)
	{
		hoverType = SELTYPE_PLAYER;
		hoverIndex = 0;
	}
}

void ProcessDrag()
{
	if (selectionType != SELTYPE_NONE)
	{
		if (IsMouseButtonJustPressed(LMB))
		{
			isDragging = true;
		} else if (IsMouseButtonJustReleased(LMB))
		{
			isDragging = false;
		}

		if (isDragging)
		{
			if (selectionType == SELTYPE_WALL_A)
			{
				Wall *w = ListGet(l->walls, selectionIndex);
				w->a = ScreenToWorldSnapped(GetLocalMousePos());
			} else if (selectionType == SELTYPE_WALL_B)
			{
				Wall *w = ListGet(l->walls, selectionIndex);
				w->b = ScreenToWorldSnapped(GetLocalMousePos());
			} else if (selectionType == SELTYPE_ACTOR)
			{
				Actor *a = ListGet(l->actors, selectionIndex);
				a->position = ScreenToWorldSnapped(GetLocalMousePos());
			} else if (selectionType == SELTYPE_PLAYER)
			{
				l->player.pos = ScreenToWorldSnapped(GetLocalMousePos());
			} else if (selectionType == SELTYPE_WALL_LINE)
			{
				Wall *w = ListGet(l->walls, selectionIndex);
				const Vector2 wp = ScreenToWorldSnapped(GetLocalMousePos());
				w->a = v2(round(wp.x - wallDragAOffset.x), round(wp.y - wallDragAOffset.y));
				w->b = v2(round(wp.x - wallDragBOffset.x), round(wp.y - wallDragBOffset.y));
			}
		}
	}
}

void Zoom(const double by)
{
	const Vector2 oldScrollPos = scrollPos;
	const Vector2 worldMousePos = ScreenToWorld(v2(WindowWidth() / 2, WindowHeight() / 2));
	const double oldZoom = zoom;

	zoom += by;
	const double zoomDiff = zoom - oldZoom;
	const Vector2 mousePosDiff = v2(worldMousePos.x * zoomDiff, worldMousePos.y * zoomDiff);
	scrollPos = Vector2Sub(scrollPos, mousePosDiff);

	if (zoom < 4)
	{
		zoom = 4;
		scrollPos = oldScrollPos;
	}
	if (zoom > 40.0)
	{
		zoom = 40.0;
		scrollPos = oldScrollPos;
	}
}

void EditorUpdate()
{
	if (l == NULL)
	{
		printf("null level!");
		return;
	}

	if (IsMouseButtonPressed(RMB))
	{
		scrollPos.x += GetRelativeMouseMotion().x;
		scrollPos.y += GetRelativeMouseMotion().y;
	}

	if (GetScroll().y != 0)
	{
		const Vector2 oldScrollPos = scrollPos;
		const Vector2 worldMousePos = ScreenToWorld(GetLocalMousePos());
		const double oldZoom = zoom;

		zoom -= GetScroll().y;
		const double zoomDiff = zoom - oldZoom;
		const Vector2 mousePosDiff = v2(worldMousePos.x * zoomDiff, worldMousePos.y * zoomDiff);
		scrollPos = Vector2Sub(scrollPos, mousePosDiff);

		if (zoom < 4)
		{
			zoom = 4;
			scrollPos = oldScrollPos;
		}
		if (zoom > 40.0)
		{
			zoom = 40.0;
			scrollPos = oldScrollPos;
		}
	}


	if (addRequest == ADDREQ_NONE || isDragging)
	{
		ProcessHover();

		if (IsMouseButtonJustPressed(LMB))
		{
			selectionType = hoverType;
			selectionIndex = hoverIndex;
			if (selectionType == SELTYPE_WALL_LINE)
			{
				const Vector2 wp = ScreenToWorld(GetLocalMousePos());
				const Wall *w = ListGet(l->walls, selectionIndex);
				wallDragAOffset = v2(wp.x - w->a.x, wp.y - w->a.y);
				wallDragBOffset = v2(wp.x - w->b.x, wp.y - w->b.y);
			}
			SelectionTypeChanged();
		}

		ProcessDrag();
	} else
	{
		if (IsMouseButtonJustPressed(LMB))
		{
			if (addRequest == ADDREQ_WALL)
			{
				Wall *w = malloc(sizeof(Wall));
				memset(w, 0, sizeof(Wall));
				w->a = ScreenToWorldSnapped(GetLocalMousePos());
				w->b = ScreenToWorldSnapped(GetLocalMousePos());
				strcpy(w->tex, "level_wall_test");
				w->uvScale = 1.0;
				ListAdd(&l->walls, w);
				selectionType = SELTYPE_WALL_B;
				selectionIndex = l->walls.length - 1;
				SelectionTypeChanged();
			} else if (addRequest == ADDREQ_ACTOR)
			{
				Actor *a = malloc(sizeof(Actor));
				memset(a, 0, sizeof(Actor));
				a->position = ScreenToWorldSnapped(GetLocalMousePos());
				a->rotation = 0.0;
				a->actorType = 1;
				a->name[0] = '\0';
				ListCreate(&a->ioConnections);
				ListAdd(&l->actors, a);
				KvListCreate(&a->params);
				UpdateActorKvs(a);
				selectionType = SELTYPE_ACTOR;
				selectionIndex = l->actors.length - 1;
				SelectionTypeChanged();
			}
			isDragging = true;
		}
	}
}

void RenderWall(const Wall *wall, const int w)
{
	const Vector2 scaledWallA = WorldToScreen(wall->a);
	const Vector2 scaledWallB = WorldToScreen(wall->b);

	GdkRGBA wallLineColor = wallLine;
	if (hoverType == SELTYPE_WALL_LINE && hoverIndex == w)
	{
		wallLineColor = wallLineHover;
	}

	DrawLine(scaledWallA, scaledWallB, wallLineColor, 4.0);
	DrawRect(v2(scaledWallA.x - 6, scaledWallA.y - 6), v2(12, 12), wallNode);
	DrawRect(v2(scaledWallB.x - 6, scaledWallB.y - 6), v2(12, 12), wallNode);

	if (hoverType == SELTYPE_WALL_A && hoverIndex == w)
	{
		DrawRect(v2(scaledWallA.x - 6, scaledWallA.y - 6), v2(12, 12), wallNodeHover);
	} else if (hoverType == SELTYPE_WALL_B && hoverIndex == w)
	{
		DrawRect(v2(scaledWallB.x - 6, scaledWallB.y - 6), v2(12, 12), wallNodeHover);
	}

	if (selectionType == SELTYPE_WALL_A && selectionIndex == w)
	{
		DrawRectOutline(v2(scaledWallA.x - 6, scaledWallA.y - 6), v2(12, 12), selectionOutline, 2.0);
	} else if (selectionType == SELTYPE_WALL_B && selectionIndex == w)
	{
		DrawRectOutline(v2(scaledWallB.x - 6, scaledWallB.y - 6), v2(12, 12), selectionOutline, 2.0);
	}
}

void RenderActorNormal(const Actor *actor, const int a)
{
	const Vector2 scaledActorPos = WorldToScreen(actor->position);

	const Vector2 dir = v2(cos(actor->rotation), sin(actor->rotation));
	const Vector2 end = v2(scaledActorPos.x + (dir.x * 20), scaledActorPos.y + (dir.y * 20));
	DrawLine(scaledActorPos, end, actorRotationLine, 2.0);

	DrawRect(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), actorNode);

	if (hoverType == SELTYPE_ACTOR && hoverIndex == a)
	{
		DrawRect(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), actorNodeHover);
	}
	if (selectionType == SELTYPE_ACTOR && selectionIndex == a)
	{
		DrawRectOutline(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), selectionOutline, 2.0);
	}
}

void RenderActorTrigger(const Actor *actor, const int a)
{
	const Vector2 scaledTriggerPos = WorldToScreen(actor->position);

	const float triggerWidth = KvGetFloat(&actor->params, "width", 1.0f);
	const float triggerHeight = KvGetFloat(&actor->params, "depth", 1.0f);
	DrawArea(scaledTriggerPos, WorldToScreenSize(v2(triggerWidth, triggerHeight)), actor->rotation, triggerArea);

	DrawRect(v2(scaledTriggerPos.x - 6, scaledTriggerPos.y - 6), v2(12, 12), triggerNode);

	if (hoverType == SELTYPE_ACTOR && hoverIndex == a)
	{
		DrawRect(v2(scaledTriggerPos.x - 6, scaledTriggerPos.y - 6), v2(12, 12), triggerNodeHover);
	}
	if (selectionType == SELTYPE_ACTOR && selectionIndex == a)
	{
		DrawRectOutline(v2(scaledTriggerPos.x - 6, scaledTriggerPos.y - 6), v2(12, 12), selectionOutline, 2.0);
	}
}

void RenderActor(const Actor *actor, const int a)
{
	const ActorDefinition *def = GetActorDef(actor->actorType);
	if (def->renderType == NORMAL)
	{
		RenderActorNormal(actor, a);
	} else if (def->renderType == TRIGGER)
	{
		RenderActorTrigger(actor, a);
	}
}

void RenderPlayer(const Player *plr)
{
	const Vector2 scaledActorPos = WorldToScreen(plr->pos);

	const Vector2 dir = v2(cos(plr->rotation), sin(plr->rotation));
	const Vector2 end = v2(scaledActorPos.x + (dir.x * 20), scaledActorPos.y + (dir.y * 20));
	DrawLine(scaledActorPos, end, playerRotationLine, 2.0);

	DrawRect(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), playerNode);

	if (hoverType == SELTYPE_PLAYER)
	{
		DrawRect(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), playerNodeHover);
	}
	if (selectionType == SELTYPE_PLAYER)
	{
		DrawRectOutline(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), playerRotationLine, 2.0);
	}
}

void EditorRenderLevel()
{
	if (l == NULL)
	{
		printf("null level!");
		return;
	}

	Clear(bg);

	scrollPosCentered = v2(scrollPos.x + (GetWindowSize().x / 2), scrollPos.y + (GetWindowSize().y / 2));

	RenderGrid();

	for (int w = 0; w < l->walls.length; w++)
	{
		const Wall *wall = ListGet(l->walls, w);
		RenderWall(wall, w);
	}

	for (int a = 0; a < l->actors.length; a++)
	{
		const Actor *actor = ListGet(l->actors, a);
		RenderActor(actor, a);
	}

	RenderPlayer(&l->player);
}

void UpdateActorKvs(Actor *actor)
{
	const ActorDefinition *def = GetActorDef(actor->actorType);
	List toDelete;
	ListCreate(&toDelete);
	for (int i = 0; i < KvListLength(&actor->params); i++)
	{
		if (def->numParams == 0)
		{
			// No parameters defined, so we can delete all keys
			ListAdd(&toDelete, KvListGetKeyName(&actor->params, i));
			continue;
		}
		// Check if the key is in the definition
		const char *key = KvListGetKeyName(&actor->params, i);
		for (int j = 0; j < def->numParams; j++)
		{
			const ActorDefParam paramDef = def->params[j];
			if (strcmp(paramDef.name, key) == 0 && paramDef.type == KvGetType(&actor->params, key))
			{
				// Key is in the definition, so we don't need to delete it
				break;
			}
			if (j == def->numParams - 1)
			{
				// Key is not in the definition, so we need to delete it
				ListAdd(&toDelete, (void *)key);
			}
		}
	}
	for (int i = 0; i < toDelete.length; i++)
	{
		const char *key = ListGet(toDelete, i);
		KvDelete(&actor->params, key);
	}

	for (int i = 0; i < def->numParams; i++)
	{
		const ActorDefParam paramDef = def->params[i];
		if (!KvListHas(&actor->params, paramDef.name))
		{
			// Key is not in the definition, so we need to add it
			switch (paramDef.type)
			{
				case PARAM_TYPE_BYTE:
					KvSetByte(&actor->params, paramDef.name, paramDef.byteDef.defaultValue);
					break;
				case PARAM_TYPE_INTEGER:
					KvSetInt(&actor->params, paramDef.name, paramDef.intDef.defaultValue);
					break;
				case PARAM_TYPE_FLOAT:
					KvSetFloat(&actor->params, paramDef.name, paramDef.floatDef.defaultValue);
					break;
				case PARAM_TYPE_BOOL:
					KvSetBool(&actor->params, paramDef.name, paramDef.boolDef.defaultValue);
					break;
				case PARAM_TYPE_STRING:
					KvSetString(&actor->params, paramDef.name, paramDef.stringDef.defaultValue);
					break;
				case PARAM_TYPE_COLOR:
					KvSetColor(&actor->params, paramDef.name, paramDef.colorDef.defaultValue);
					break;
				default:
					printf("Unknown parameter type %d for actor %s\n", paramDef.type, actor->name);
			}
		}
	}
}

float randomf(float min, float max) {
	return min + ((float)rand() / RAND_MAX) * (max - min);
}

void GenerateBenchmarkLevel(const size_t numWalls, const size_t numActors, const int actorType, const float halfSize)
{
	srand(time(NULL));
	for (size_t i = 0; i < numWalls; i++)
	{
		Wall *w = malloc(sizeof(Wall));
		memset(w, 0, sizeof(Wall));
		w->a = v2(randomf(-halfSize, halfSize), randomf(-halfSize, halfSize));
		w->b = v2(randomf(-halfSize, halfSize), randomf(-halfSize, halfSize));
		strcpy(w->tex, "level_wall_test");
		w->uvScale = 1.0;
		ListAdd(&l->walls, w);
	}
	for (size_t i = 0; i < numActors; i++)
	{
		Actor *a = malloc(sizeof(Actor));
		memset(a, 0, sizeof(Actor));
		a->position = v2(randomf(-halfSize, halfSize), randomf(-halfSize, halfSize));
		a->rotation = degToRad(rand() % 360);
		a->actorType = actorType;
		a->name[0] = '\0';
		ListCreate(&a->ioConnections);
		ListAdd(&l->actors, a);
		KvListCreate(&a->params);
		UpdateActorKvs(a);
	}
}
