//
// Created by droc101 on 1/13/25.
//

#include "Drawing.h"

GtkDrawingArea *area;
cairo_t *cr;
Vector2 frameSize;

void BeginRender(GtkDrawingArea *ar, cairo_t *c)
{
    area = ar;
    cr = c;
	frameSize = v2(gtk_widget_get_width(GTK_WIDGET(ar)), gtk_widget_get_height(GTK_WIDGET(ar)));
}

void DrawRect(Vector2 start, Vector2 size, GdkRGBA color) /* RGBA 0-255 */
{
    gdk_cairo_set_source_rgba(cr, &color);
    cairo_rectangle(cr, start.x, start.y, size.x, size.y);
    cairo_fill(cr);
}

void DrawArea(Vector2 center, Vector2 size, double rotation, GdkRGBA color)
{
	gdk_cairo_set_source_rgba(cr, &color);
	cairo_save(cr);
	cairo_translate(cr, center.x, center.y);
	cairo_rotate(cr, rotation);
	cairo_rectangle(cr, -size.x / 2, -size.y / 2, size.x, size.y);
	cairo_fill(cr);
	cairo_restore(cr);
}

void DrawRectOutline(Vector2 start, Vector2 size, GdkRGBA color, double thickness)
{
	gdk_cairo_set_source_rgba (cr, &color);
	cairo_rectangle(cr, start.x, start.y, size.x, size.y);
	cairo_set_line_width(cr, thickness);
	cairo_stroke(cr);
}

void DrawLine(Vector2 start, Vector2 end, GdkRGBA color, float thickness)
{
    gdk_cairo_set_source_rgba (cr, &color);
    cairo_set_line_width(cr, thickness);
    cairo_move_to(cr, start.x, start.y);
    cairo_line_to(cr, end.x, end.y);
    cairo_stroke(cr);
}

void Clear(GdkRGBA color)
{
    gdk_cairo_set_source_rgba (cr, &color);
    cairo_paint(cr);
}

Vector2 GetWindowSize()
{
	return frameSize;
}

void RenderText(const char *str, const Vector2 pos, const double size, const GdkRGBA color)
{
	gdk_cairo_set_source_rgba (cr, &color);
	cairo_set_font_size(cr, size);
	cairo_set_font_face(cr, cairo_toy_font_face_create("monospace", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL));
	cairo_move_to(cr, pos.x, pos.y);
	cairo_show_text(cr, str);
}
