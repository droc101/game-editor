//
// Created by droc101 on 1/13/25.
//

#ifndef DRAWING_H
#define DRAWING_H
#include <gtk/gtk.h>
#include "Vector2.h"
#include "../defines.h"

void BeginRender(GtkDrawingArea *ar, cairo_t *c);

#define Color(clr) {((int)((clr) >> 24 & 0xFF) / 255.0), ((int)((clr) >> 16 & 0xFF) / 255.0), ((int)((clr) >> 8 & 0xFF) / 255.0), ((int)((clr) & 0xFF) / 255.0)}

void DrawRect(Vector2 start, Vector2 size, GdkRGBA color);

void DrawRectOutline(Vector2 start, Vector2 size, GdkRGBA color, double thickness);

void DrawLine(Vector2 start, Vector2 end, GdkRGBA color, float thickness);

void Clear(GdkRGBA color);

void DrawImage(Vector2 start, Vector2 size, const char *path);

void TransformForLevelRender(Vector2 translation);

void ResetTransform();

Vector2 GetWindowSize();

#define WindowWidth() GetWindowSize().x
#define WindowHeight() GetWindowSize().y

#endif //DRAWING_H
