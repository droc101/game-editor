//
// Created by droc101 on 1/8/25.
//

#include <GL/glew.h>
#include <gtk-4.0/gtk/gtk.h>

#include "Editor.h"
#include "UI/MainWindow.h"

int main(const int argc, char *argv[])
{
	srand(time(NULL));
	putenv("GDK_BACKEND=wayland"); // Allow wayland if available (fixes some theme issues)

	EditorNewLevel();

	GtkApplication *app = gtk_application_new("dev.droc101.game.editor", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(MainWindowActivate), NULL);
	const int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	EditorDestroyLevel();

	return status;
}
