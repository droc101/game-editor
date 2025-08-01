//
// Created by droc101 on 1/8/25.
//

#include <GL/glew.h>
#include <gtk-4.0/gtk/gtk.h>
#include "Editor.h"
#include "UI/MainWindow.h"
#include "UI/UiHelpers.h"

int main(const int argc, char *argv[])
{
	putenv("GDK_BACKEND=wayland, x11"); // Allow wayland if available (fixes some theme issues)

	EditorInit();

	AdwApplication *app = adw_application_new("dev.droc101.game.editor",
											  G_APPLICATION_DEFAULT_FLAGS | G_APPLICATION_NON_UNIQUE);
	application = ADW_APPLICATION(app);
	g_signal_connect(app, "activate", G_CALLBACK(MainWindowActivate), NULL);
	const int status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);

	EditorDestroy();

	return status;
}
