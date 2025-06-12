//
// Created by droc101 on 6/1/25.
//

#include "Message.h"
#include <adwaita.h>

void mb_exit_callback(GObject *, GAsyncResult *, gpointer)
{
	exit(1);
}

void MessageWindowShow(GtkWindow *parent, const char *title, const char *message, const GAsyncReadyCallback callback)
{
	GtkWidget *dialog = adw_message_dialog_new(parent, title, message);
	adw_message_dialog_add_response(ADW_MESSAGE_DIALOG(dialog), "ok", "OK");
	adw_message_dialog_choose(ADW_MESSAGE_DIALOG(dialog), NULL, callback, NULL);
}
