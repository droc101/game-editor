//
// Created by droc101 on 1/13/25.
//

#ifndef DRAWING_H
#define DRAWING_H

#include <gtk/gtk.h>
#include "../defines.h"

/**
 * Begin the frame
 * @param ar The drawing area
 * @param c The cairo instance
 */
void BeginRender(GtkDrawingArea *ar, cairo_t *c);

/**
 * Convert an ARGB uint into a GdkRGBA
 * @param clr The uint color
 */
#define Color(clr) {((int)((clr) >> 24 & 0xFF) / 255.0), ((int)((clr) >> 16 & 0xFF) / 255.0), ((int)((clr) >> 8 & 0xFF) / 255.0), ((int)((clr) & 0xFF) / 255.0)}

/**
 * Draw a rectangle
 * @param start The start position in pixels
 * @param size The size in pixels (down-right)
 * @param color The color
 */
void DrawRect(Vector2 start, Vector2 size, GdkRGBA color);

/**
 * Draw a rotated rectangle "area"
 * @param center The center point of the area
 * @param size The size of the area
 * @param rotation The rotation of the area
 * @param color The color of the area
 */
void DrawArea(Vector2 center, Vector2 size, double rotation, GdkRGBA color);

/**
 * Draw the outline of a rectangle
 * @param start The start of the rectangle
 * @param size The size of the rectangle
 * @param color The color of the rectangle
 * @param thickness The thickness of the outline
 */
void DrawRectOutline(Vector2 start, Vector2 size, GdkRGBA color, double thickness);

/**
 * Draw a line
 * @param start The start of the line
 * @param end The end of the line
 * @param color The color of the line
 * @param thickness The thickness of the line
 */
void DrawLine(Vector2 start, Vector2 end, GdkRGBA color, float thickness);

/**
 * Clear the screen with a given color
 * @param color The color to clear with
 */
void Clear(GdkRGBA color);

/**
 * Get the size of the drawing area
 */
Vector2 GetWindowSize();

/**
 * Draw monospace text
 * @param str The string to draw
 * @param pos The position to draw at
 * @param size The font size
 * @param color The font color
 */
void RenderText(const char *str, Vector2 pos, double size, GdkRGBA color);

#define WindowWidth() GetWindowSize().x
#define WindowHeight() GetWindowSize().y

#endif //DRAWING_H
