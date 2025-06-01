//
// Created by droc101 on 6/1/25.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <gtk/gtk.h>

void mb_exit_callback(GObject *, GAsyncResult *, gpointer);

void MessageWindowShow(GtkWindow *parent, const char *title, const char *message, GAsyncReadyCallback callback);

#endif //MESSAGE_H
