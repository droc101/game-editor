//
// Created by droc101 on 1/13/25.
//

#ifndef DRAWING_H
#define DRAWING_H
#include <gtk/gtk.h>
#include "Vector2.h"
#include "../defines.h"

void BeginRender(GtkDrawingArea *ar, cairo_t *c);

void DrawRect(Vector2 start, Vector2 size, uint color);

void DrawRectOutline(Vector2 start, Vector2 size, uint color, double thickness);

void DrawLine(Vector2 start, Vector2 end, uint color, float thickness);

void Clear(uint color);

void DrawImage(Vector2 start, Vector2 size, const char *path);

void TransformForLevelRender(Vector2 translation);

void ResetTransform();

Vector2 GetWindowSize();

#define WindowWidth() GetWindowSize().x
#define WindowHeight() GetWindowSize().y

#endif //DRAWING_H
