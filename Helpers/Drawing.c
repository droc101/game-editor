//
// Created by droc101 on 1/13/25.
//

#include "Drawing.h"
#include "Vector2.h"

GtkDrawingArea *area;
cairo_t *cr;
Vector2 frameSize;

void BeginRender(GtkDrawingArea *ar, cairo_t *c)
{
	area = ar;
	cr = c;
	frameSize = v2(gtk_widget_get_width(GTK_WIDGET(ar)), gtk_widget_get_height(GTK_WIDGET(ar)));
	cairo_set_antialias(cr, CAIRO_ANTIALIAS_NONE);
}

void DrawRect(const Vector2 start, const Vector2 size, const GdkRGBA color) /* RGBA 0-255 */
{
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_rectangle(cr, start.x, start.y, size.x, size.y);
	cairo_fill(cr);
}

void DrawArea(const Vector2 center, const Vector2 size, const double rotation, const GdkRGBA color)
{
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_save(cr);
	cairo_translate(cr, center.x, center.y);
	cairo_rotate(cr, rotation);
	cairo_rectangle(cr, -size.x / 2, -size.y / 2, size.x, size.y);
	cairo_fill(cr);
	cairo_restore(cr);
}

void DrawRectOutline(const Vector2 start, const Vector2 size, const GdkRGBA color, const double thickness)
{
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_rectangle(cr, start.x, start.y, size.x, size.y);
	cairo_set_line_width(cr, thickness);
	cairo_stroke(cr);
}

void DrawLine(const Vector2 start, const Vector2 end, const GdkRGBA color, const float thickness)
{
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_set_line_width(cr, thickness);
	cairo_move_to(cr, start.x, start.y);
	cairo_line_to(cr, end.x, end.y);
	cairo_stroke(cr);
}

void Clear(const GdkRGBA color)
{
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_paint(cr);
}

Vector2 GetWindowSize()
{
	return frameSize;
}

void RenderText(const char *str, const Vector2 pos, const double size, const GdkRGBA color)
{
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_set_font_size(cr, size);
	cairo_set_font_face(cr, cairo_toy_font_face_create("monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
	cairo_move_to(cr, pos.x, pos.y);
	cairo_show_text(cr, str);
}
