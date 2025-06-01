//
// Created by droc101 on 1/16/25.
//

#include "OptionsWindow.h"

#include "../Editor.h"
#include "../Helpers/Options.h"
#include "Message.h"

GtkWindow *optionsWindow;
GtkFileDialog *gameFolderDialog;
GtkWidget *gamePathEntry;
bool optionsWindowOpen = false;
bool optionsRequired = false;

GtkApplication *optionsApplication;

/**
 * The callback for when a game folder was selected from the folder dialog
 */
void game_folder_selected(GObject *, GAsyncResult *res, gpointer)
{
	GError *error = NULL;
	GFile *f = gtk_file_dialog_select_folder_finish(gameFolderDialog, res, &error);
	if (error != NULL)
	{
		printf("File dialog error: %s\n", error->message);
		g_error_free(error);
		return;
	}
	const char *path = g_file_get_path(f);
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(gamePathEntry));
	gtk_entry_buffer_set_text(buffer, path, -1);
}

/**
 * Callback for when the pick folder ("...") button is clicked
 */
void pick_folder_clicked(GtkButton *, gpointer)
{
	gameFolderDialog = gtk_file_dialog_new();
	gtk_file_dialog_select_folder(gameFolderDialog, optionsWindow, NULL, game_folder_selected, NULL);
}

/**
 * Callback for when the cancel button is pressed
 */
void opt_cancel_clicked(GtkButton *, gpointer)
{
	gtk_window_close(optionsWindow);
}

/**
 * Callback for when the OK button is pressed
 */
void opt_ok_clicked(GtkButton *, gpointer)
{
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(GTK_ENTRY(gamePathEntry));
	const char *path = gtk_entry_buffer_get_text(buffer);
	char oldPath[260];
	strncpy(oldPath, options.gameDirectory, 260);
	strncpy(options.gameDirectory, path, 260);

	if (!IsValidGameDirectory(&options))
	{
		MessageWindowShow(optionsWindow, "Invalid Game Directory",
						  "The selected directory does not contain a valid game executable and assets folder.",
						  NULL);
		strncpy(options.gameDirectory, oldPath, 260);
		return;
	}

	optionsRequired = false;
	gtk_window_close(optionsWindow);
}

/**
 * Callback for when the options window is closed
 */
void options_closed(GtkWindow *, gpointer)
{
	if (optionsRequired)
	{
		g_application_quit(G_APPLICATION(optionsApplication));
		EditorDestroy();
		exit(1);
	}
	optionsWindowOpen = false;
}

void OptionsWindowShow(GtkWindow *parent, GtkApplication *app, const bool required)
{
	optionsWindowOpen = true;
	GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "Setup");
	gtk_window_set_transient_for(GTK_WINDOW(window), parent);
	gtk_window_set_modal(GTK_WINDOW(window), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(window), 400, -1);

	GtkWidget *mainStack = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_margin_start(mainStack, 4);
	gtk_widget_set_margin_end(mainStack, 4);
	gtk_widget_set_margin_top(mainStack, 4);
	gtk_widget_set_margin_bottom(mainStack, 4);

	GtkWidget *headerLabel = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(headerLabel), "<big><b>Setup</b></big>");
	gtk_label_set_xalign(GTK_LABEL(headerLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), headerLabel);

	GtkWidget *gamePathLabel = gtk_label_new("Game Path");
	gtk_label_set_xalign(GTK_LABEL(gamePathLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), gamePathLabel);

	GtkWidget *gamePathHBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_append(GTK_BOX(mainStack), gamePathHBox);

	gamePathEntry = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(gamePathEntry), "Path to game folder");
	gtk_entry_buffer_set_max_length(GTK_ENTRY_BUFFER(gtk_entry_get_buffer(GTK_ENTRY(gamePathEntry))), 259);
	gtk_entry_buffer_set_text(GTK_ENTRY_BUFFER(gtk_entry_get_buffer(GTK_ENTRY(gamePathEntry))),
							  options.gameDirectory,
							  -1);
	gtk_widget_set_hexpand(GTK_WIDGET(gamePathEntry), TRUE);
	gtk_box_append(GTK_BOX(gamePathHBox), gamePathEntry);

	GtkWidget *gamePathButton = gtk_button_new_with_label("...");
	gtk_widget_set_size_request(gamePathButton, 40, -1);
	g_signal_connect(gamePathButton, "clicked", G_CALLBACK(pick_folder_clicked), NULL);
	gtk_box_append(GTK_BOX(gamePathHBox), gamePathButton);

	if (!required)
	{
		GtkWidget *requiresRestartLabel = gtk_label_new("Changes require a restart");
		gtk_widget_add_css_class(requiresRestartLabel, "grayText");
		gtk_label_set_xalign(GTK_LABEL(requiresRestartLabel), 0);
		gtk_widget_set_hexpand(requiresRestartLabel, TRUE);
		gtk_box_append(GTK_BOX(mainStack), requiresRestartLabel);
	}

	GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep);

	GtkWidget *okCancelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_margin_top(okCancelBox, 0);
	gtk_box_append(GTK_BOX(mainStack), okCancelBox);
	gtk_widget_set_halign(okCancelBox, GTK_ALIGN_END);

	GtkWidget *okButton = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okButton, 80, -1);
	g_signal_connect(okButton, "clicked", G_CALLBACK(opt_ok_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), okButton);

	GtkWidget *cancelButton = gtk_button_new_with_label(required ? "Quit" : "Cancel");
	gtk_widget_set_size_request(cancelButton, 80, -1);
	g_signal_connect(cancelButton, "clicked", G_CALLBACK(opt_cancel_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), cancelButton);

	gtk_window_set_child(GTK_WINDOW(window), mainStack);

	optionsWindow = GTK_WINDOW(window);
	optionsRequired = required;
	optionsApplication = app;

	g_signal_connect(window, "destroy", G_CALLBACK(options_closed), NULL);

	gtk_window_present(GTK_WINDOW(window));
}
