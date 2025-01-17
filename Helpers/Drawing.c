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
    gdk_cairo_set_source_rgba (cr, &color);
    cairo_rectangle(cr, start.x, start.y, size.x, size.y);
    cairo_fill(cr);
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

void DrawImage(Vector2 start, Vector2 size, const char *path) {
    // Load the image
    cairo_surface_t *image = cairo_image_surface_create_from_png(path);

    // Get image dimensions
    int img_width = cairo_image_surface_get_width(image);
    int img_height = cairo_image_surface_get_height(image);

    // Save the current state of the context
    cairo_save(cr);

    // Translate and scale the context to stretch the image
    cairo_translate(cr, start.x, start.y);
    cairo_scale(cr, size.x / img_width, size.y / img_height);

    // Draw the image
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_paint(cr);

    // Restore the context to remove the scaling
    cairo_restore(cr);

    // Destroy the image surface
    cairo_surface_destroy(image);
}

void TransformForLevelRender(Vector2 translation)
{
	cairo_save(cr);
	cairo_translate(cr, translation.x, translation.y);
}

void ResetTransform()
{
	cairo_restore(cr);
}

Vector2 GetWindowSize()
{
	return frameSize;
}
