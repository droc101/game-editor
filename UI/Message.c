//
// Created by droc101 on 6/1/25.
//

#include "Message.h"

void mb_exit_callback(GObject *, GAsyncResult *, gpointer)
{
	exit(1);
}

void MessageWindowShow(GtkWindow *parent, const char *title, const char *message, const GAsyncReadyCallback callback)
{
	GtkAlertDialog *dialog = gtk_alert_dialog_new("");
	gtk_alert_dialog_set_message(dialog, title);
	gtk_alert_dialog_set_detail(dialog, message);
	gtk_alert_dialog_set_modal(dialog, TRUE);
	gtk_alert_dialog_choose(dialog, parent, NULL, callback, NULL);
}
