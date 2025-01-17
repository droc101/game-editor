//
// Created by droc101 on 1/15/25.
//

#include "Editor.h"

#include <stdlib.h>
#include <string.h>
#include "Helpers/Options.h"
#include "defines.h"
#include "Helpers/Drawing.h"
#include "Helpers/Input.h"
#include "UI/MainWindow.h"

Level *l;
Options options;

AddRequestType addRequest;

Vector2 scrollPos = {0};
Vector2 scrollPosCentered = {0};
double zoom = 20.0;

SelectionType selectionType = SELTYPE_NONE;
int selectionIndex = -1;

SelectionType hoverType = SELTYPE_NONE;
int hoverIndex = -1;

bool isDragging = false;
Vector2 wallDragAOffset = {0};
Vector2 wallDragBOffset = {0};

List *textureList = NULL;
List *musicList = NULL;

GdkRGBA bg = Color(0x123456FF);
GdkRGBA grid = Color(0x808080FF);
GdkRGBA xAxis = Color(0xFF0000FF);
GdkRGBA zAxis = Color(0x0000FFFF);
GdkRGBA selectionOutline = Color(0xFF0000FF);
GdkRGBA wallLine = Color(0xFFFFFF80);
GdkRGBA wallLineHover = Color(-1);
GdkRGBA wallNode = Color(0x0000FFFF);
GdkRGBA wallNodeHover = Color(0xFFFFFF40);
GdkRGBA actorRotationLine = Color(0x808000FF);
GdkRGBA actorNode = Color(0xFFFF00FF);
GdkRGBA actorNodeHover = Color(0x00000040);
GdkRGBA playerRotationLine = Color(0x008000FF);
GdkRGBA playerNode = Color(0x00FF00FF);
GdkRGBA playerNodeHover = Color(0x00000040);

void EditorInit()
{
	LoadOptions(&options);
	EditorNewLevel();
}

void EditorDestroy()
{
	SaveOptions(&options);
	EditorDestroyLevel();
}

void RescanAssets()
{
	if (textureList != NULL)
	{
		ListFreeWithData(textureList);
		ListFreeWithData(musicList);
	}

	textureList = ScanAssetFolder("texture", ".gtex");
	musicList = ScanAssetFolder("audio", ".gmus");
}

List *ScanAssetFolder(const char *folderName, const char *extension)
{
	List *fileList = CreateList();
	char *levelDataPath = malloc(strlen(options.gameDirectory) + strlen(folderName) + 2);
	strcpy(levelDataPath, options.gameDirectory);
	strcat(levelDataPath, "/assets/");
	strcat(levelDataPath, folderName);

	// Get the name of all gmap files in the level directory
	DIR *dir = opendir(levelDataPath);
	if (dir == NULL)
	{
		printf("Failed to open level directory: %s\n", levelDataPath);
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
	closedir(dir);

	return fileList;
}

void EditorDestroyLevel()
{
	if (l == NULL)
	{
		return;
	}
	ListFreeWithData(l->actors);
	ListFreeWithData(l->walls);
	ListFreeWithData(l->triggers);
	ListFreeWithData(l->models);
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

	l->walls = CreateList();
	l->actors = CreateList();
	l->triggers = CreateList();
	l->models = CreateList();
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

	// srand(time(NULL));
	// for (int i = 0; i < 20; i++)
	// {
	// 	Wall *w = malloc(sizeof(Wall));
	// 	memset(w, 0, sizeof(Wall));
	// 	w->a = v2(rand() % 50 - 25, rand() % 50 - 25);
	// 	w->b = v2(rand() % 50 - 25, rand() % 50 - 25);
	// 	strcpy(w->tex, "level_wall_test");
	// 	w->uvScale = 1.0;
	//
	// 	ListAdd(l->walls, w);
	// }
	//
	// for (int i = 0; i < 50; i++)
	// {
	// 	Actor *a = malloc(sizeof(Actor));
	// 	memset(a, 0, sizeof(Actor));
	// 	a->position = v2(rand() % 50 - 25, rand() % 50 - 25);
	// 	a->rotation = (rand() % 360) * (M_PI / 180.0);
	// 	ListAdd(l->actors, a);
	// }
}

Vector2 WorldToScreen(Vector2 wp)
{
	return v2(round((wp.x * zoom) + scrollPosCentered.x), round((wp.y * zoom) + scrollPosCentered.y));
}

Vector2 ScreenToWorld(Vector2 sp)
{
    return v2((sp.x - scrollPosCentered.x) / zoom, (sp.y - scrollPosCentered.y) / zoom);
}

Vector2 ScreenToWorldSnapped(Vector2 sp)
{
	Vector2 realPos = ScreenToWorld(sp);
	realPos.x = round(realPos.x);
	realPos.y = round(realPos.y);
	return realPos;
}

void RenderGrid()
{
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

	for (int w = 0; w < l->walls->size; w++)
	{
		Wall *wall = ListGet(l->walls, w);

		Vector2 scaledWallA = WorldToScreen(wall->a);
		Vector2 scaledWallB = WorldToScreen(wall->b);

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

	for (int a = 0; a < l->actors->size; a++)
	{
		Actor *act = ListGet(l->actors, a);
		Vector2 scaledActorPos = WorldToScreen(act->position);
		if (Vector2Distance(scaledActorPos, GetLocalMousePos()) < 10)
		{
			hoverType = SELTYPE_ACTOR;
			hoverIndex = a;
			break;
		}
	}

	Vector2 scaledPlayerPos = WorldToScreen(l->player.pos);
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
				Vector2 wp = ScreenToWorldSnapped(GetLocalMousePos());
				w->a = v2(round(wp.x - wallDragAOffset.x), round(wp.y - wallDragAOffset.y));
				w->b = v2(round(wp.x - wallDragBOffset.x), round(wp.y - wallDragBOffset.y));
			}
		}
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

	zoom -= GetScroll().y;
	if (zoom < 4)
	{
		zoom = 4;
	}
	if (zoom > 30.0)
	{
		zoom = 30.0;
	}

	if (addRequest == ADDREQ_NONE)
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
				ListAdd(l->walls, w);
				selectionType = SELTYPE_WALL_B;
				selectionIndex = l->walls->size - 1;
				SelectionTypeChanged();
			} else if (addRequest == ADDREQ_ACTOR)
			{
				Actor *a = malloc(sizeof(Actor));
				memset(a, 0, sizeof(Actor));
				a->position = ScreenToWorldSnapped(GetLocalMousePos());
				a->rotation = 0.0;
				ListAdd(l->actors, a);
				selectionType = SELTYPE_ACTOR;
				selectionIndex = l->actors->size - 1;
				SelectionTypeChanged();
			}
			isDragging = true;
			addRequest = ADDREQ_NONE;
		}
	}


}

void RenderWall(const Wall *wall, const int w)
{
	Vector2 scaledWallA = WorldToScreen(wall->a);
	Vector2 scaledWallB = WorldToScreen(wall->b);

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

void RenderActor(const Actor *actor, const int a)
{
	Vector2 scaledActorPos = WorldToScreen(actor->position);

	Vector2 dir = v2(cos(actor->rotation), sin(actor->rotation));
	Vector2 end = v2(scaledActorPos.x + (dir.x * 20), scaledActorPos.y + (dir.y * 20));
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

void RenderPlayer(const Player *plr)
{
	Vector2 scaledActorPos = WorldToScreen(plr->pos);

	Vector2 dir = v2(cos(plr->rotation), sin(plr->rotation));
	Vector2 end = v2(scaledActorPos.x + (dir.x * 20), scaledActorPos.y + (dir.y * 20));
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

	for (int w = 0; w < l->walls->size; w++)
	{
		Wall *wall = ListGet(l->walls, w);
		RenderWall(wall, w);
	}

	for (int a = 0; a < l->actors->size; a++)
	{
		Actor *actor = ListGet(l->actors, a);
		RenderActor(actor, a);
	}

	RenderPlayer(&l->player);
}
