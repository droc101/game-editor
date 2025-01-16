//
// Created by droc101 on 1/13/25.
//

#include "MainWindow.h"

#include "../Editor.h"
#include "../Helpers/Drawing.h"
#include "../Helpers/Input.h"

int frame = 0;

void drawMainArea (GtkDrawingArea* drawing_area, cairo_t* cr, int width, int height, gpointer user_data)
{
	frame++;
	EditorUpdate();
	BeginRender(GTK_DRAWING_AREA(drawing_area), cr);
	TickInput();
	InputFrameEnd();

	EditorRenderLevel();
}

// Timer callback to continuously update the drawing area
gboolean on_timeout(gpointer user_data) {

	GtkWidget *drawing_area = GTK_WIDGET(user_data);
	gtk_widget_queue_draw(drawing_area);
	return TRUE;
}

static void quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
	g_application_quit(G_APPLICATION(app));
}

static void about_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window(GTK_APPLICATION(app));

	GFile *logo_file = g_file_new_for_path ("./Assets/editor_icon.png");
	GdkTexture *logo = gdk_texture_new_from_file (logo_file, NULL);
	g_object_unref (logo_file);

	GtkWidget *about_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "Game Editor");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "0.0.1");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "© 2025 Droc101 Development");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "Level editor for Game");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), "https://droc101.dev");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), (const gchar *[]){"droc101", "NBT22", NULL});
	gtk_about_dialog_add_credit_section(GTK_ABOUT_DIALOG(about_dialog), "Third-Party Libraries", (const gchar *[]){"GTK4", NULL});
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about_dialog), GDK_PAINTABLE(logo));

	gtk_window_set_transient_for(GTK_WINDOW(about_dialog), GTK_WINDOW(window));
	gtk_window_set_modal(GTK_WINDOW(about_dialog), TRUE);

	gtk_widget_set_visible(about_dialog, true);
}

static GActionEntry menu_entries[] = {
	{"new", NULL, NULL, NULL, NULL},
	{"open", NULL, NULL, NULL, NULL},
	{"save", NULL, NULL, NULL, NULL},
	{"save_as", NULL, NULL, NULL, NULL},
	{"quit", quit_activated, NULL, NULL, NULL},
	{"zoom_in", NULL, NULL, NULL, NULL},
	{"zoom_out", NULL, NULL, NULL, NULL},
	{"reset_zoom", NULL, NULL, NULL, NULL},
	{"show_walls", NULL, NULL, NULL, NULL},
	{"show_actors", NULL, NULL, NULL, NULL},
	{"show_triggers", NULL, NULL, NULL, NULL},
	{"show_models", NULL, NULL, NULL, NULL},
	{"setup", NULL, NULL, NULL, NULL},
	{"texture_manager", NULL, NULL, NULL, NULL},
	{"run_game", NULL, NULL, NULL, NULL},
	{"about", about_activated, NULL, NULL, NULL},
};

GtkWidget *SetupMenuBar(GtkApplication *app)
{
	GMenu *menu = g_menu_new();

	GMenu *file_menu = g_menu_new();
	g_menu_append(file_menu, "New", "app.new");
	g_menu_append(file_menu, "Open", "app.open");
	g_menu_append(file_menu, "Save", "app.save");
	g_menu_append(file_menu, "Save As", "app.save_as");
	g_menu_append(file_menu, "Quit", "app.quit");
	g_menu_append_submenu(menu, "File", G_MENU_MODEL(file_menu));
	g_object_unref(file_menu);

	GMenu *view_menu = g_menu_new();
	g_menu_append(view_menu, "Zoom In", "app.zoom_in");
	g_menu_append(view_menu, "Zoom Out", "app.zoom_out");
	g_menu_append(view_menu, "Reset Zoom", "app.reset_zoom");
	GMenuItem *showWallsItem = g_menu_item_new("Show Walls", "app.show_walls");
	g_menu_item_set_attribute (showWallsItem, "toggle-state", "b", TRUE);
	g_menu_append_item(view_menu, showWallsItem);
	GMenuItem *showActorsItem = g_menu_item_new("Show Actors", "app.show_actors");
	g_menu_item_set_attribute (showActorsItem, "toggle-state", "b", TRUE);
	g_menu_append_item(view_menu, showActorsItem);
	GMenuItem *showTriggersItem = g_menu_item_new("Show Triggers", "app.show_triggers");
	g_menu_item_set_attribute (showTriggersItem, "toggle-state", "b", TRUE);
	//g_menu_append_item(view_menu, showTriggersItem);
	GMenuItem *showModelsItem = g_menu_item_new("Show Models", "app.show_models");
	g_menu_item_set_attribute (showModelsItem, "toggle-state", "b", TRUE);
	//g_menu_append_item(view_menu, showModelsItem);
	g_menu_append_submenu(menu, "View", G_MENU_MODEL(view_menu));

	GMenu *tools_menu = g_menu_new();
	g_menu_append(tools_menu, "Setup", "app.setup");
	g_menu_append(tools_menu, "Texture Manager", "app.texture_manager");
	g_menu_append(tools_menu, "Launch Game", "app.run_game");
	g_menu_append_submenu(menu, "Tools", G_MENU_MODEL(tools_menu));
	g_object_unref(tools_menu);

	GMenu *help_menu = g_menu_new();
	g_menu_append(help_menu, "About", "app.about");
	g_menu_append_submenu(menu, "Help", G_MENU_MODEL(help_menu));
	g_object_unref(help_menu);

	GtkWidget *menuBar = gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu));

	g_action_map_add_action_entries(G_ACTION_MAP(app), menu_entries, G_N_ELEMENTS(menu_entries), app);

	return menuBar;
}

GtkWidget *SetupToolbar()
{
	GtkWidget *modeLabel = gtk_label_new("Mode:");

	GtkWidget *add_mode_radio = gtk_check_button_new_with_label("Add");
	GtkWidget *edit_mode_radio = gtk_check_button_new_with_label("Edit");
	GtkWidget *delete_mode_radio = gtk_check_button_new_with_label("Delete");

	gtk_check_button_set_group(GTK_CHECK_BUTTON(delete_mode_radio), GTK_CHECK_BUTTON(edit_mode_radio));
	gtk_check_button_set_group(GTK_CHECK_BUTTON(add_mode_radio), GTK_CHECK_BUTTON(edit_mode_radio));

	gtk_check_button_set_active(GTK_CHECK_BUTTON(edit_mode_radio), TRUE);

	GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

	GtkWidget *addWallButton = gtk_button_new_with_label("Add Wall");
	gtk_button_set_has_frame(GTK_BUTTON(addWallButton), FALSE);
	GtkWidget *addActorButton = gtk_button_new_with_label("Add Actor");
	gtk_button_set_has_frame(GTK_BUTTON(addActorButton), FALSE);
	GtkWidget *addTriggerButton = gtk_button_new_with_label("Add Trigger");
	gtk_button_set_has_frame(GTK_BUTTON(addTriggerButton), FALSE);
	GtkWidget *addModelButton = gtk_button_new_with_label("Add Model");
	gtk_button_set_has_frame(GTK_BUTTON(addModelButton), FALSE);


	GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_size_request(toolbar, -1, 30);
	gtk_box_append(GTK_BOX(toolbar), modeLabel);
	gtk_box_append(GTK_BOX(toolbar), add_mode_radio);
	gtk_box_append(GTK_BOX(toolbar), edit_mode_radio);
	gtk_box_append(GTK_BOX(toolbar), delete_mode_radio);
	gtk_box_append(GTK_BOX(toolbar), sep);
	gtk_box_append(GTK_BOX(toolbar), addWallButton);
	gtk_box_append(GTK_BOX(toolbar), addActorButton);
	//gtk_box_append(GTK_BOX(toolbar), addTriggerButton);
	//gtk_box_append(GTK_BOX(toolbar), addModelButton);

	return toolbar;
}

GtkWidget *SetupStatusBar()
{
	GtkWidget *statusLabel = gtk_label_new("Status");

	GtkWidget *statusBar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_size_request(statusBar, -1, 20);
	gtk_box_append(GTK_BOX(statusBar), statusLabel);
    return statusBar;
}

GtkWidget *SetupDrawingArea()
{
	GtkWidget *drawingArea = gtk_drawing_area_new();
	gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(drawingArea), drawMainArea, NULL, NULL);
	gtk_widget_set_size_request(drawingArea, 320, 240);
	gtk_widget_set_hexpand(drawingArea, TRUE);
	gtk_widget_set_vexpand(drawingArea, TRUE);

	gtk_widget_set_can_focus(drawingArea, TRUE);
	gtk_widget_set_focus_on_click(drawingArea, TRUE);
	gtk_widget_set_focusable(drawingArea, TRUE);

	GtkEventController *mouseEvents = gtk_event_controller_motion_new();
	g_signal_connect_object(mouseEvents, "motion", G_CALLBACK(motion), drawingArea, 0);
	g_signal_connect_object(mouseEvents, "enter", G_CALLBACK(mouse_enter), drawingArea, 0);
	g_signal_connect_object(mouseEvents, "leave", G_CALLBACK(mouse_leave), drawingArea, 0);
	gtk_widget_add_controller(drawingArea, mouseEvents);

	GtkEventController *scrollEvents = gtk_event_controller_scroll_new(GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES);
	g_signal_connect_object(scrollEvents, "scroll", G_CALLBACK(scroll), drawingArea, 0);
	gtk_widget_add_controller(drawingArea, scrollEvents);

	GtkGesture *leftClick = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(leftClick), GDK_BUTTON_PRIMARY);
	gtk_gesture_single_set_exclusive(GTK_GESTURE_SINGLE(leftClick), TRUE);
	g_signal_connect_object(leftClick, "pressed", G_CALLBACK(lmb_pressed), drawingArea, 0);
	g_signal_connect_object(leftClick, "released", G_CALLBACK(lmb_released), drawingArea, 0);
	g_signal_connect_object(leftClick, "unpaired-release", G_CALLBACK(lmb_unpaired_release), drawingArea, 0);
	//g_signal_connect_object(leftClick, "stopped", G_CALLBACK(lmb_stopped), drawingArea, 0);
	gtk_widget_add_controller(drawingArea, GTK_EVENT_CONTROLLER(leftClick));

	GtkGesture *rightClick = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(rightClick), GDK_BUTTON_SECONDARY);
	gtk_gesture_single_set_exclusive(GTK_GESTURE_SINGLE(rightClick), TRUE);
	g_signal_connect_object(rightClick, "pressed", G_CALLBACK(rmb_pressed), drawingArea, 0);
	g_signal_connect_object(rightClick, "released", G_CALLBACK(rmb_released), drawingArea, 0);
	g_signal_connect_object(rightClick, "unpaired-release", G_CALLBACK(rmb_unpaired_release), drawingArea, 0);
	//g_signal_connect_object(rightClick, "stopped", G_CALLBACK(rmb_stopped), drawingArea, 0);
	gtk_widget_add_controller(drawingArea, GTK_EVENT_CONTROLLER(rightClick));

	g_timeout_add(1000 / 60, on_timeout, drawingArea);

	return drawingArea;
}

void MainWindowActivate(GtkApplication *app, gpointer *)
{
	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Game Level Editor");
	gtk_window_set_default_size(GTK_WINDOW(window), 1440, 900);

	GtkWidget *menuBar = SetupMenuBar(app);

	GtkWidget *drawingArea = SetupDrawingArea();

	GtkWidget *leftSidebar = gtk_scrolled_window_new();
	gtk_widget_set_size_request(leftSidebar, 250, -1);
	GtkWidget *leftSidebarVLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(leftSidebar), leftSidebarVLayout);
	GtkWidget *leftSidebarHeader = gtk_label_new("");
	gtk_label_set_xalign(GTK_LABEL(leftSidebarHeader), 0);
	gtk_label_set_markup(GTK_LABEL(leftSidebarHeader), "<b>Selection Properties</b>");
	gtk_box_append(GTK_BOX(leftSidebarVLayout), leftSidebarHeader);

	GtkWidget *rightSidebar = gtk_scrolled_window_new();
	gtk_widget_set_size_request(rightSidebar, 250, -1);
	GtkWidget *rightSidebarVLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(rightSidebar), rightSidebarVLayout);
	GtkWidget *rightSidebarHeader = gtk_label_new("");
	gtk_label_set_xalign(GTK_LABEL(rightSidebarHeader), 0);
	gtk_label_set_markup(GTK_LABEL(rightSidebarHeader), "<b>Level Properties</b>");
	gtk_box_append(GTK_BOX(rightSidebarVLayout), rightSidebarHeader);

	GtkWidget *mainHLayout = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
	gtk_box_append(GTK_BOX(mainHLayout), leftSidebar);
	gtk_box_append(GTK_BOX(mainHLayout), drawingArea);
	gtk_box_append(GTK_BOX(mainHLayout), rightSidebar);

	GtkWidget *toolbar = SetupToolbar();
	GtkWidget *statusBar = SetupStatusBar();

	GtkWidget *mainVLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_box_append(GTK_BOX(mainVLayout), menuBar);
	gtk_box_append(GTK_BOX(mainVLayout), toolbar);
	gtk_box_append(GTK_BOX(mainVLayout), mainHLayout);
	gtk_box_append(GTK_BOX(mainVLayout), statusBar);

	gtk_window_set_child(GTK_WINDOW(window), mainVLayout);

	gtk_window_present(GTK_WINDOW(window));
}
