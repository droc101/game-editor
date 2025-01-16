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
Vector2 _LastFrameMousePos = {0};

void mouse_enter(GtkEventControllerMotion *self, gdouble x, gdouble y, gpointer user_data)
{
	mouseInBounds = true;
}

void mouse_leave(GtkEventControllerMotion *self, gpointer user_data)
{
	mouseInBounds = false;
}

void motion(GtkEventControllerMotion *self, gdouble x, gdouble y, gpointer user_data)
{
	const graphene_point_t inPoint = {x, y};
	graphene_point_t outPoint;
	gboolean cmp = gtk_widget_compute_point(GTK_WIDGET(user_data), GTK_WIDGET(user_data), &inPoint, &outPoint);
	if (!cmp)
	{
		// printf("gtk_widget_compute_point failed\n");
		return;
	}
	localMousePos = v2(outPoint.x, outPoint.y);
}

gboolean scroll(GtkEventControllerScroll *self, gdouble dx, gdouble dy, gpointer user_data)
{
	frameScroll.x += dx;
	frameScroll.y += dy;
	return true;
}

void lmb_pressed(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	// printf("lm pressed\n");
	mouseButtons[LMB] = JUST_PRESSED;
}

void lmb_released(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	// printf("lm released\n");
	mouseButtons[LMB] = JUST_RELEASED;
}

void lmb_stopped(GtkGestureClick *self, gpointer user_data)
{
	// printf("lm stopped\n");
	mouseButtons[LMB] = JUST_RELEASED;
}


void rmb_pressed(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	// printf("rm pressed\n");
	mouseButtons[RMB] = JUST_PRESSED;
}

void rmb_released(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data)
{
	// printf("rm released\n");
	mouseButtons[RMB] = JUST_RELEASED;
}

void rmb_stopped(GtkGestureClick *self, gpointer user_data)
{
	// printf("rm stopped\n");
	mouseButtons[RMB] = JUST_RELEASED;
}

void lmb_unpaired_release(GtkGestureClick *self,
						  gdouble x,
						  gdouble y,
						  guint button,
						  GdkEventSequence *sequence,
						  gpointer user_data)
{
	if (button != GDK_BUTTON_PRIMARY) return;
	// printf("lm unpaired release\n");
	mouseButtons[LMB] = JUST_RELEASED;
}

void rmb_unpaired_release(GtkGestureClick *self,
						  gdouble x,
						  gdouble y,
						  guint button,
						  GdkEventSequence *sequence,
						  gpointer user_data)
{
	if (button != GDK_BUTTON_SECONDARY) return;
	// printf("rm unpaired release\n");
	mouseButtons[RMB] = JUST_RELEASED;
}

InputState TickInputState(InputState st)
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

	_LastFrameMousePos = localMousePos;
}

bool IsMouseButtonPressed(MouseButton button)
{
	return mouseButtons[button] == PRESSED || mouseButtons[button] == JUST_PRESSED;
}

bool IsMouseButtonReleased(MouseButton button)
{
	return mouseButtons[button] == RELEASED || mouseButtons[button] == JUST_RELEASED;
}

bool IsMouseButtonJustPressed(MouseButton button)
{
	return mouseButtons[button] == JUST_PRESSED;
}

bool IsMouseButtonJustReleased(MouseButton button)
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
	return v2(localMousePos.x - _LastFrameMousePos.x, localMousePos.y - _LastFrameMousePos.y);
}

Vector2 GetScroll()
{
	return frameScroll;
}

void InputFrameEnd()
{
	frameScroll = v2(0, 0);
}
