//
// Created by droc101 on 1/14/25.
//

#include "Input.h"
#include "../defines.h"
#include "Vector2.h"

Vector2 localMousePos;
InputState mouseButtons[2] = {RELEASED, RELEASED};
bool mouseInBounds = false;

Vector2 frameScroll = {0};
Vector2 lastFrameMousePos = {0};

void mouse_enter(GtkEventControllerMotion *, gdouble, gdouble, gpointer)
{
	mouseInBounds = true;
}

void mouse_leave(GtkEventControllerMotion *, gpointer)
{
	mouseInBounds = false;
}

void motion(GtkEventControllerMotion *, const gdouble x, const gdouble y, const gpointer user_data)
{
	const graphene_point_t inPoint = {x, y};
	graphene_point_t outPoint;
	const gboolean cmp = gtk_widget_compute_point(GTK_WIDGET(user_data), GTK_WIDGET(user_data), &inPoint, &outPoint);
	if (!cmp)
	{
		// printf("gtk_widget_compute_point failed\n");
		return;
	}
	localMousePos = v2(outPoint.x, outPoint.y);
}

gboolean scroll(GtkEventControllerScroll *, const gdouble dx, const gdouble dy, gpointer)
{
	frameScroll.x += dx;
	frameScroll.y += dy;
	return true;
}

void lmb_pressed(GtkGestureClick *, gint, gdouble, gdouble, gpointer)
{
	// printf("lm pressed\n");
	mouseButtons[LMB] = JUST_PRESSED;
}

void lmb_released(GtkGestureClick *, gint, gdouble, gdouble, gpointer)
{
	// printf("lm released\n");
	mouseButtons[LMB] = JUST_RELEASED;
}

void lmb_stopped(GtkGestureClick *, gpointer)
{
	// printf("lm stopped\n");
	mouseButtons[LMB] = JUST_RELEASED;
}


void rmb_pressed(GtkGestureClick *, gint, gdouble, gdouble, gpointer)
{
	// printf("rm pressed\n");
	mouseButtons[RMB] = JUST_PRESSED;
}

void rmb_released(GtkGestureClick *, gint, gdouble, gdouble, gpointer)
{
	// printf("rm released\n");
	mouseButtons[RMB] = JUST_RELEASED;
}

void rmb_stopped(GtkGestureClick *, gpointer)
{
	// printf("rm stopped\n");
	mouseButtons[RMB] = JUST_RELEASED;
}

void lmb_unpaired_release(GtkGestureClick *,
						  gdouble,
						  gdouble,
						  const guint button,
						  GdkEventSequence *,
						  gpointer)
{
	if (button != GDK_BUTTON_PRIMARY) return;
	// printf("lm unpaired release\n");
	mouseButtons[LMB] = JUST_RELEASED;
}

void rmb_unpaired_release(GtkGestureClick *,
						  gdouble,
						  gdouble,
						  const guint button,
						  GdkEventSequence *,
						  gpointer)
{
	if (button != GDK_BUTTON_SECONDARY) return;
	// printf("rm unpaired release\n");
	mouseButtons[RMB] = JUST_RELEASED;
}

InputState TickInputState(const InputState st)
{
	switch (st)
	{
		case JUST_PRESSED:
			return PRESSED;
		case JUST_RELEASED:
			return RELEASED;
		default:
			return st;
	}
}

void TickInput()
{
	for (int i = 0; i < 2; i++)
	{
		mouseButtons[i] = TickInputState(mouseButtons[i]);
	}

	lastFrameMousePos = localMousePos;
}

bool IsMouseButtonPressed(const MouseButton button)
{
	return mouseButtons[button] == PRESSED || mouseButtons[button] == JUST_PRESSED;
}

bool IsMouseButtonReleased(const MouseButton button)
{
	return mouseButtons[button] == RELEASED || mouseButtons[button] == JUST_RELEASED;
}

bool IsMouseButtonJustPressed(const MouseButton button)
{
	return mouseButtons[button] == JUST_PRESSED;
}

bool IsMouseButtonJustReleased(const MouseButton button)
{
	return mouseButtons[button] == JUST_RELEASED;
}

Vector2 GetLocalMousePos()
{
	if (!mouseInBounds)
	{
		return v2(-1000, -1000);
	}
	return localMousePos;
}

Vector2 GetRelativeMouseMotion()
{
	return v2(localMousePos.x - lastFrameMousePos.x, localMousePos.y - lastFrameMousePos.y);
}

Vector2 GetScroll()
{
	return frameScroll;
}

void InputFrameEnd()
{
	frameScroll = v2(0, 0);
}
