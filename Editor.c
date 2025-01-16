//
// Created by droc101 on 1/15/25.
//

#include "Editor.h"

#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "Helpers/Drawing.h"
#include "Helpers/Input.h"

enum SelectionType
{
	NONE,
	WALL_A,
	WALL_B,
	WALL_LINE,
	ACTOR,
};

Level *l;
Vector2 scrollPos = {0};
Vector2 scrollPosCentered = {0};
double zoom = 20.0;

enum SelectionType selectionType = NONE;
int selectionIndex = -1;

enum SelectionType hoverType = NONE;
int hoverIndex = -1;

bool isDragging = false;

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
	strcpy(l->ceilOrSkyTex, "level_sky");
	strcpy(l->floorTex, "level_uvtest");
	l->fogColor = 0x000000FF;
	l->fogStart = 50.0f;
	l->fogEnd = 100.0f;

	for (int i = 0; i < 20; i++)
	{
		Wall *w = malloc(sizeof(Wall));
		memset(w, 0, sizeof(Wall));
		w->a = v2(rand() % 50, rand() % 50);
		w->b = v2(rand() % 50, rand() % 50);
		strcpy(w->tex, "level_uvtest");
		w->uvScale = 1.0;

		ListAdd(l->walls, w);
	}

	for (int i = 0; i < 50; i++)
	{
		Actor *a = malloc(sizeof(Actor));
		memset(a, 0, sizeof(Actor));
		a->position = v2(rand() % 50, rand() % 50);
		a->rotation = (rand() % 360) * (M_PI / 180.0);
		ListAdd(l->actors, a);
	}
}

Vector2 WorldToScreen(Vector2 wp)
{
	return v2((wp.x * zoom) + scrollPosCentered.x, (wp.y * zoom) + scrollPosCentered.y);
}

Vector2 ScreenToWorld(Vector2 sp)
{
    return v2((sp.x - scrollPosCentered.x) / zoom, (sp.y - scrollPosCentered.y) / zoom);
}

Vector2 ScreenToWorldClamped(Vector2 sp)
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
		DrawLine(v2(x, 0), v2(x, WindowHeight()), 0x808080ff, 1.0f);
	}
	for (int y = gridOffsetY; y < WindowHeight(); y += gridSpacing)
	{
		DrawLine(v2(0, y), v2(WindowWidth(), y), 0x808080ff, 1.0f);
	}

	DrawLine(v2(scrollPosCentered.x, 0), v2(scrollPosCentered.x, WindowHeight()), 0x0000FFFF, 2.0f);
	DrawLine(v2(0, scrollPosCentered.y), v2(WindowWidth(), scrollPosCentered.y), 0xFF0000FF, 2.0f);
}

void ProcessHover()
{
	hoverType = NONE;

	for (int w = 0; w < l->walls->size; w++)
	{
		Wall *wall = ListGet(l->walls, w);

		Vector2 scaledWallA = WorldToScreen(wall->a);
		Vector2 scaledWallB = WorldToScreen(wall->b);

		if (Vector2Distance(scaledWallA, GetLocalMousePos()) < 10)
		{
			hoverType = WALL_A;
			hoverIndex = w;
			break;
		}
		if (Vector2Distance(scaledWallB, GetLocalMousePos()) < 10)
		{
			hoverType = WALL_B;
			hoverIndex = w;
			break;
		}
		if (Vector2DistanceToLine(scaledWallA, scaledWallB, GetLocalMousePos()) < 10)
		{
			hoverType = WALL_LINE;
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
			hoverType = ACTOR;
			hoverIndex = a;
			break;
		}
	}
}

void ProcessDrag()
{
	if (selectionType == WALL_A || selectionType == WALL_B || selectionType == ACTOR)
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
			if (selectionType == WALL_A)
			{
				Wall *w = ListGet(l->walls, selectionIndex);
				w->a = ScreenToWorldClamped(GetLocalMousePos());
			} else if (selectionType == WALL_B)
			{
				Wall *w = ListGet(l->walls, selectionIndex);
				w->b = ScreenToWorldClamped(GetLocalMousePos());
			} else if (selectionType == ACTOR)
			{
				Actor *a = ListGet(l->actors, selectionIndex);
				a->position = ScreenToWorldClamped(GetLocalMousePos());
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

	ProcessHover();

	if (IsMouseButtonJustPressed(LMB))
	{
		selectionType = hoverType;
		selectionIndex = hoverIndex;
		if (selectionType == WALL_LINE)
		{
			selectionType = WALL_A;
		}
	}

	ProcessDrag();
}

void RenderWall(const Wall *wall, const int w)
{
	Vector2 scaledWallA = WorldToScreen(wall->a);
	Vector2 scaledWallB = WorldToScreen(wall->b);

	uint wallLineColor = 0xAAAAAAFF;
	if (hoverType == WALL_LINE && hoverIndex == w)
	{
		wallLineColor = -1;
	}

	DrawLine(scaledWallA, scaledWallB, wallLineColor, 4.0);
	DrawRect(v2(scaledWallA.x - 6, scaledWallA.y - 6), v2(12, 12), 0x0000FFFF);
	DrawRect(v2(scaledWallB.x - 6, scaledWallB.y - 6), v2(12, 12), 0x0000FFFF);

	if (hoverType == WALL_A && hoverIndex == w)
	{
		DrawRect(v2(scaledWallA.x - 6, scaledWallA.y - 6), v2(12, 12), 0xFFFFFF40);
	} else if (hoverType == WALL_B && hoverIndex == w)
	{
		DrawRect(v2(scaledWallB.x - 6, scaledWallB.y - 6), v2(12, 12), 0xFFFFFF40);
	}

	if (selectionType == WALL_A && selectionIndex == w)
	{
		DrawRectOutline(v2(scaledWallA.x - 6, scaledWallA.y - 6), v2(12, 12), 0xFF0000FF, 2.0);
	} else if (selectionType == WALL_B && selectionIndex == w)
	{
		DrawRectOutline(v2(scaledWallB.x - 6, scaledWallB.y - 6), v2(12, 12), 0xFF0000FF, 2.0);
	}
}

void RenderActor(const Actor *actor, const int a)
{
	Vector2 scaledActorPos = WorldToScreen(actor->position);

	DrawRect(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), 0xffFF00FF);

	Vector2 dir = v2(cos(actor->rotation), sin(actor->rotation));
	Vector2 end = v2(scaledActorPos.x + (dir.x * 20), scaledActorPos.y + (dir.y * 20));
	DrawLine(scaledActorPos, end, 0xffFF00FF, 4.0);

	if (hoverType == ACTOR && hoverIndex == a)
	{
		DrawRect(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), 0x00000040);
	}
	if (selectionType == ACTOR && selectionIndex == a)
	{
		DrawRectOutline(v2(scaledActorPos.x - 6, scaledActorPos.y - 6), v2(12, 12), 0xFF0000FF, 2.0);
	}
}

void EditorRenderLevel()
{
	if (l == NULL)
	{
		printf("null level!");
		return;
	}

	Clear(0x123456FF);

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
}
