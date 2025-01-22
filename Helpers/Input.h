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

/**
 * Event handler for mouse enter
 */
void mouse_enter(GtkEventControllerMotion *self, gdouble x, gdouble y, gpointer user_data);

/**
 * Event handler for mouse leave
 */
void mouse_leave(GtkEventControllerMotion *self, gpointer user_data);

/**
 * Event handler for mouse move
 */
void motion(GtkEventControllerMotion *self, gdouble x, gdouble y, gpointer user_data);

/**
 * Event handler for scroll wheel
 */
gboolean scroll(GtkEventControllerScroll *self, gdouble dx, gdouble dy, gpointer user_data);

/**
 * Event handler for left mouse button press
 */
void lmb_pressed(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

/**
 * Event handler for left mouse button release
 */
void lmb_released(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

/**
 * Event handler for right mouse button press
 */
void rmb_pressed(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

/**
 * Event handler for right mouse button release
 */
void rmb_released(GtkGestureClick *self, gint n_press, gdouble x, gdouble y, gpointer user_data);

/**
 * Tick input state (just pressed -> pressed, etc.)
 */
void TickInput();

/**
 * Check if a mouse button is pressed
 */
bool IsMouseButtonPressed(MouseButton button);

/**
 * Check if a mouse button is released (not pressed)
 */
bool IsMouseButtonReleased(MouseButton button);

/**
 * Check if a mouse button was just pressed
 */
bool IsMouseButtonJustPressed(MouseButton button);

/**
 * Check if a mouse button was just released
 */
bool IsMouseButtonJustReleased(MouseButton button);

/**
 * Get the local mouse position within the drawing area
 */
Vector2 GetLocalMousePos();

/**
 * Event handler for right mouse button stopped
 */
void rmb_stopped(GtkGestureClick *self, gpointer user_data);

/**
 * Event handler for left mouse button stopped
 */
void lmb_stopped(GtkGestureClick *self, gpointer user_data);

/**
 * Event handler for left mouse button unpaired release
 */
void lmb_unpaired_release(GtkGestureClick *self,
						  gdouble x,
						  gdouble y,
						  guint button,
						  GdkEventSequence *sequence,
						  gpointer user_data);

/**
 * Event handler for right mouse button unpaired release
 */
void rmb_unpaired_release(GtkGestureClick *self,
						  gdouble x,
						  gdouble y,
						  guint button,
						  GdkEventSequence *sequence,
						  gpointer user_data);

/**
 * Get relative mouse motion
 */
Vector2 GetRelativeMouseMotion();

/**
 * Finish the input frame
 */
void InputFrameEnd();

/**
 * Get scroll delta since last input tick
 */
Vector2 GetScroll();

#endif //INPUT_H
