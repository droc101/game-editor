//
// Created by droc101 on 1/14/25.
//

#ifndef INPUT_H
#define INPUT_H

#include <gtk/gtk.h>

#include "Vector2.h"

typedef enum InputState
{
	JUST_PRESSED,
	JUST_RELEASED,
	PRESSED,
	RELEASED
} InputState;

typedef enum MouseButton
{
	LMB,
	RMB
} MouseButton;

void mouse_enter(GtkEventControllerMotion *self, gdouble x, gdouble y, gpointer user_data);

void mouse_leave(GtkEventControllerMotion *self, gpointer user_data);

void motion(GtkEventControllerMotion *self, gdouble x, gdouble y, gpointer user_data);

gboolean scroll(GtkEventControllerScroll *self, gdouble dx, gdouble dy, gpointer user_data);

void lmb_pressed(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

void lmb_released(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

void rmb_pressed(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

void rmb_released(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

void TickInput();

bool IsMouseButtonPressed(MouseButton button);

bool IsMouseButtonReleased(MouseButton button);

bool IsMouseButtonJustPressed(MouseButton button);

bool IsMouseButtonJustReleased(MouseButton button);

Vector2 GetLocalMousePos();

void rmb_stopped(GtkGestureClick *self, gpointer user_data);

void lmb_stopped(GtkGestureClick *self, gpointer user_data);

void lmb_unpaired_release(GtkGestureClick *self,
						  gdouble x,
						  gdouble y,
						  guint button,
						  GdkEventSequence *sequence,
						  gpointer user_data);

void rmb_unpaired_release(GtkGestureClick *self,
						  gdouble x,
						  gdouble y,
						  guint button,
						  GdkEventSequence *sequence,
						  gpointer user_data);

Vector2 GetRelativeMouseMotion();

void InputFrameEnd();

Vector2 GetScroll();

#endif //INPUT_H
