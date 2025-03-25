//
// Created by droc101 on 1/15/25.
//

#include "Editor.h"
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "Helpers/Drawing.h"
#include "Helpers/GameInterface.h"
#include "Helpers/Input.h"
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

void RescanAssets()
{
	if (!LoadDefFiles())
	{
		printf("Failed to load game definitions!\n");
		return;
	}
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
	ListFreeWithData(l->actors);
	ListFreeWithData(l->walls);
	ListFreeWithData(l->triggers);
	//ListFreeWithData(l->models);
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
	//l->models = CreateList();
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
	// for (int i = 0; i < 2000; i++)
	// {
	// 	Wall *w = malloc(sizeof(Wall));
	// 	memset(w, 0, sizeof(Wall));
	// 	w->a = v2(rand() % 100 - 50, rand() % 100 - 50);
	// 	w->b = v2(rand() % 100 - 50, rand() % 100 - 50);
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
	//
	// for (int i = 0; i < 50; i++)
	// {
	// 	Trigger *t = malloc(sizeof(Trigger));
	// 	memset(t, 0, sizeof(Trigger));
	// 	t->position = v2(rand() % 50 - 25, rand() % 50 - 25);
	// 	t->extents = v2(rand() % 10 - 5, rand() % 10 - 5);
	// 	t->rotation = (rand() % 360) * (M_PI / 180.0);
	// 	ListAdd(l->triggers, t);
	// }

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

	for (int a = 0; a < l->actors->size; a++)
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

	for (int t = 0; t < l->triggers->size; t++)
	{
		const Trigger *trig = ListGet(l->triggers, t);
		const Vector2 scaledTriggerPos = WorldToScreen(trig->position);
		if (Vector2Distance(scaledTriggerPos, GetLocalMousePos()) < 10)
		{
			hoverType = SELTYPE_TRIGGER;
			hoverIndex = t;
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
			} else if (selectionType == SELTYPE_TRIGGER)
			{
				Trigger *t = ListGet(l->triggers, selectionIndex);
				t->position = ScreenToWorldSnapped(GetLocalMousePos());
			}
		}
	}
}

void Zoom(const double by)
{
	const Vector2 oldScrollPos = scrollPos;
	const Vector2 worldMousePos = ScreenToWorld(v2(WindowWidth()/2, WindowHeight()/2));
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
				a->actorType = 1;
				a->name[0] = '\0';
				ListAdd(l->actors, a);
				selectionType = SELTYPE_ACTOR;
				selectionIndex = l->actors->size - 1;
				SelectionTypeChanged();
			} else if (addRequest == ADDREQ_TRIGGER)
			{
				Trigger *t = malloc(sizeof(Trigger));
				memset(t, 0, sizeof(Trigger));
				t->position = ScreenToWorldSnapped(GetLocalMousePos());
				t->extents = v2(5, 5);
				t->rotation = 0.0;
				ListAdd(l->triggers, t);
				selectionType = SELTYPE_TRIGGER;
				selectionIndex = l->triggers->size - 1;
				SelectionTypeChanged();
			}
			isDragging = true;
			if (addRequest != ADDREQ_WALL)
			{
				addRequest = ADDREQ_NONE;
			}
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

void RenderActor(const Actor *actor, const int a)
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

void RenderTrigger(const Trigger *trigger, const int t)
{
	const Vector2 scaledTriggerPos = WorldToScreen(trigger->position);

	DrawArea(scaledTriggerPos, WorldToScreenSize(trigger->extents), trigger->rotation, triggerArea);

	DrawRect(v2(scaledTriggerPos.x - 6, scaledTriggerPos.y - 6), v2(12, 12), triggerNode);

	if (hoverType == SELTYPE_TRIGGER && hoverIndex == t)
	{
		DrawRect(v2(scaledTriggerPos.x - 6, scaledTriggerPos.y - 6), v2(12, 12), triggerNodeHover);
	}
	if (selectionType == SELTYPE_TRIGGER && selectionIndex == t)
	{
		DrawRectOutline(v2(scaledTriggerPos.x - 6, scaledTriggerPos.y - 6), v2(12, 12), selectionOutline, 2.0);
	}
	Vector2 textPos = scaledTriggerPos;
	textPos = Vector2Add(textPos, v2(0, 16));
	textPos = Vector2Sub(textPos, Vector2Scale(WorldToScreenSize(trigger->extents), 0.5));
	RenderText(trigger->command, textPos, 16, triggerCommand);
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

	for (int t = 0; t < l->triggers->size; t++)
	{
		const Trigger *trigger = ListGet(l->triggers, t);
		RenderTrigger(trigger, t);
	}

	for (int w = 0; w < l->walls->size; w++)
	{
		const Wall *wall = ListGet(l->walls, w);
		RenderWall(wall, w);
	}

	for (int a = 0; a < l->actors->size; a++)
	{
		const Actor *actor = ListGet(l->actors, a);
		RenderActor(actor, a);
	}

	RenderPlayer(&l->player);
}
