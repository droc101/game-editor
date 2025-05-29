//
// Created by droc101 on 1/13/25.
//

#include "MainWindow.h"

#include "../Editor.h"
#include "../Helpers/Drawing.h"
#include "../Helpers/GameInterface.h"
#include "../Helpers/Input.h"
#include "../Helpers/LevelWriter.h"
#include "../Helpers/Options.h"
#include "IOWindow.h"
#include "KvWindow.h"
#include "OptionsWindow.h"
#include "UiHelpers.h"

int frame = 0;
GtkWidget *statusLabel;

GtkWidget *leftSidebarVLayout;
GtkWidget *leftSidebarCurrent;

GtkWindow *mainWindow = NULL;
GtkFileDialog *fileDialog;
GtkApplication *mainWindowApplication;

GtkWidget *addWallsButton;
GtkWidget *addActorsButton;

GtkWidget *actorTypeLabel;
GtkWidget *actorNameBox;
GtkWidget *actorTypeBox;

#pragma region Signal Handlers

/**
 * Draw callback from the drawing area
 * @param drawing_area The drawing area to draw to
 * @param cr The cairo instance to use
 */
void drawMainArea(GtkDrawingArea *drawing_area, cairo_t *cr, int, int, gpointer)
{
	frame++;
	EditorUpdate();
	BeginRender(GTK_DRAWING_AREA(drawing_area), cr);
	TickInput();
	InputFrameEnd();

	EditorRenderLevel();
}

/**
 * Timer callback to continuously update the drawing area
 */
gboolean on_timeout(const gpointer user_data)
{
	GtkWidget *drawing_area = GTK_WIDGET(user_data);
	gtk_widget_queue_draw(drawing_area);
	return TRUE;
}

/**
 * Callback for when the add wall button is clicked
 */
void add_wall_clicked(GtkToggleButton *self, gpointer)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(addActorsButton), FALSE);
	const bool toggled = gtk_toggle_button_get_active(self);
	addRequest = toggled ? ADDREQ_WALL : ADDREQ_NONE;
}

/**
 * Callback for when the add actor button is clicked
 */
void add_actor_clicked(GtkToggleButton *self, gpointer)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(addWallsButton), FALSE);
	const bool toggled = gtk_toggle_button_get_active(self);
	addRequest = toggled ? ADDREQ_ACTOR : ADDREQ_NONE;
}

/**
 * Callback for when the delete selected button is clicked
 */
void delete_selected_clicked(GtkButton *, gpointer)
{
	if (selectionType == SELTYPE_WALL_A || selectionType == SELTYPE_WALL_B || selectionType == SELTYPE_WALL_LINE)
	{
		ListRemoveAt(&l->walls, selectionIndex);
		selectionType = SELTYPE_NONE;
		selectionIndex = -1;
	} else if (selectionType == SELTYPE_ACTOR)
	{
		ListRemoveAt(&l->actors, selectionIndex);
		selectionType = SELTYPE_NONE;
		selectionIndex = -1;
	}
	SelectionTypeChanged();
}

#pragma region File Menu

/**
 * Callback for when the quit menu item is clicked
 */
static void quit_activated(GSimpleAction *, GVariant *, const gpointer app)
{
	g_application_quit(G_APPLICATION(app));
}

/**
 * Callback for when a file is selected from the save file dialog
 */
static void save_file_selected(GObject *, GAsyncResult *res, gpointer)
{
	GError *error = NULL;
	GFile *f = gtk_file_dialog_save_finish(fileDialog, res, &error);
	if (error != NULL)
	{
		printf("Save file dialog error: %s\n", error->message);
		g_error_free(error);
		return;
	}
	const char *path = g_file_get_path(f);
	WriteLevel(l, path);
}

/**
 * Callback for when a file is selected from the open file dialog
 */
static void open_file_selected(GObject *, GAsyncResult *res, gpointer)
{
	GError *error = NULL;
	GFile *f = gtk_file_dialog_open_finish(fileDialog, res, &error);
	if (error != NULL)
	{
		printf("Open file dialog error: %s\n", error->message);
		g_error_free(error);
		return;
	}
	const char *path = g_file_get_path(f);
	EditorDestroyLevel();
	l = ReadLevel(path);
}

/**
 * Callback for when the save level menu item is pressed
 */
static void save_activated(GSimpleAction *, GVariant *, gpointer)
{
	gtk_file_dialog_save(fileDialog, mainWindow, NULL, save_file_selected, NULL);
}

/**
 * Callback for when the open level menu item is pressed
 */
static void open_activated(GSimpleAction *, GVariant *, gpointer)
{
	gtk_file_dialog_open(fileDialog, mainWindow, NULL, open_file_selected, NULL);
}

/**
 * Callback for when the new level menu item is pressed
 */
static void new_activated(GSimpleAction *, GVariant *, gpointer)
{
	EditorNewLevel();
}

#pragma endregion

#pragma region Edit Menu

/**
 * Callback for when the delete selected menu item is pressed
 */
void delete_selected_menu_item_activated(GSimpleAction *, GVariant *, gpointer)
{
	delete_selected_clicked(NULL, NULL);
}

#pragma endregion

#pragma region View Menu

/**
 * Callback for when the zoom out menu item is pressed
 */
static void zoom_out_activated(GSimpleAction *, GVariant *, gpointer)
{
	Zoom(-1.0);
}

/**
 * Callback for when the zoom in menu item is pressed
 */
static void zoom_in_activated(GSimpleAction *, GVariant *, gpointer)
{
	Zoom(1.0);
}

/**
 * Callback for when the reset zoom menu item is pressed
 */
static void reset_zoom_activated(GSimpleAction *, GVariant *, gpointer)
{
	zoom = 20.0;
}

/**
 * Callback for when the center origin menu item is pressed
 */
static void center_origin_activated(GSimpleAction *, GVariant *, gpointer)
{
	scrollPos = v2s(0);
}

static void snap_lower_activated(GSimpleAction *, GVariant *, gpointer)
{
	snapIndex--;
	if (snapIndex < 0)
	{
		snapIndex = 0;
	}
}

static void snap_higher_activated(GSimpleAction *, GVariant *, gpointer)
{
	snapIndex++;
	if (snapIndex >= snapCount)
	{
		snapIndex = snapCount - 1;
	}
}

static void snap_reset_activated(GSimpleAction *, GVariant *, gpointer)
{
	snapIndex = 4;
}

#pragma endregion

#pragma region Tools Menu

/**
 * Callback for when the setup menu item is pressed
 */
void setup_activated(GSimpleAction *, GVariant *, const gpointer app)
{
	OptionsWindowShow(mainWindow, GTK_APPLICATION(app), false);
}

#pragma endregion

#pragma region Help Menu

/**
 * Callback for when the about menu item is pressed
 */
static void about_activated(GSimpleAction *, GVariant *, const gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window(GTK_APPLICATION(app));

	GFile *logo_file = g_file_new_for_path("./Assets/editor_icon.png");
	GdkTexture *logo = gdk_texture_new_from_file(logo_file, NULL);
	g_object_unref(logo_file);

	GtkWidget *about_dialog = gtk_about_dialog_new();
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(about_dialog), "Game Editor");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about_dialog), "0.0.1");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about_dialog), "© 2025 Droc101 Development");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about_dialog), "Level editor for Game");
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about_dialog), "https://droc101.dev");
	gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about_dialog), (const gchar *[]){"droc101", "NBT22", NULL});
	gtk_about_dialog_add_credit_section(GTK_ABOUT_DIALOG(about_dialog),
										"Third-Party Libraries",
										(const gchar *[]){"GTK4", "ZLIB", "JSON-C", NULL});
	gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about_dialog), GDK_PAINTABLE(logo));

	gtk_window_set_transient_for(GTK_WINDOW(about_dialog), GTK_WINDOW(window));
	gtk_window_set_modal(GTK_WINDOW(about_dialog), TRUE);

	gtk_widget_set_visible(about_dialog, true);
}

#pragma endregion

#pragma region Player Sidebar

/**
 * Callback for when the player rotation value is changed
 */
void plr_rot_value_changed(GtkSpinButton *self, gpointer)
{
	l->player.rotation = degToRad(gtk_spin_button_get_value(self));
}

#pragma endregion

#pragma region Wall Sidebar

/**
 * Callback for when the wall texture is changed
 */
void wall_texture_changed(GtkEditable *self, gpointer)
{
	Wall *w = ListGet(l->walls, selectionIndex);
	const char *text = gtk_editable_get_text(self);
	strcpy(w->tex, text);
}

/**
 * Callback for when the wall uv scale is changed
 */
void wall_uv_scale_value_changed(GtkSpinButton *self, gpointer)
{
	Wall *w = ListGet(l->walls, selectionIndex);
	w->uvScale = gtk_spin_button_get_value(self);
}

/**
 * Callback for when the wall uv offset is changed
 */
void wall_uv_offset_value_changed(GtkSpinButton *self, gpointer)
{
	Wall *w = ListGet(l->walls, selectionIndex);
	w->uvOffset = gtk_spin_button_get_value(self);
}

#pragma endregion

#pragma region Actor Sidebar

/**
 * Callback for when the actor type is changed
 */
void actor_type_changed(GtkComboBox *self, gpointer)
{
	Actor *a = ListGet(l->actors, selectionIndex);
	int type = 0;
	for (int i = 0; i < GetActorTypeCount(); i++)
	{
		const ActorDefinition *def = GetActorDefByLoadIndex(i);
		if (strcmp(def->actorName, gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(self))) == 0)
		{
			type = def->actorType;
			break;
		}
	}
	a->actorType = type;
	UpdateActorKvs(a);
	UpdateActorSidebar();
}

/**
 * Callback for when actor rotation is changed
 */
void actor_rot_value_changed(GtkSpinButton *self, gpointer)
{
	Actor *a = ListGet(l->actors, selectionIndex);
	a->rotation = degToRad(gtk_spin_button_get_value(self));
}

/**
 * Callback for when the trigger command is changed
 */
static void actor_name_changed(GtkEditable *self, gpointer)
{
	Actor *actor = ListGet(l->actors, selectionIndex);
	const char *text = gtk_editable_get_text(self);
	strcpy(actor->name, text);
}

void edit_io_clicked(GtkButton *, gpointer)
{
	IOWindowShow(mainWindow, GTK_APPLICATION(mainWindowApplication), ListGet(l->actors, selectionIndex));
}

void edit_kv_clicked(GtkButton *, gpointer)
{
	KvWindowShow(mainWindow, GTK_APPLICATION(mainWindowApplication), ListGet(l->actors, selectionIndex));
}

#pragma endregion

#pragma region Level Sidebar

/**
 * Callback for when the level name is changed
 */
void level_name_changed(GtkEditable *self, gpointer)
{
	const char *text = gtk_editable_get_text(self);
	strcpy(l->name, text);
}

/**
 * Callback for when the level course number is changed
 */
void level_course_num_value_changed(GtkSpinButton *self, gpointer)
{
	l->courseNum = gtk_spin_button_get_value(self);
}

/**
 * Callback for when the level ceil/sky switch is toggled
 */
gboolean level_ceil_or_sky_state_set(GtkSwitch *, const gboolean state, gpointer)
{
	if (state)
	{
		l->hasCeiling = TRUE;
	} else
	{
		l->hasCeiling = FALSE;
	}
	return TRUE;
}

/**
 * Callback for when the level ceil/sky texture is changed
 */
void level_ceil_or_sky_tex_changed(GtkEditable *self, gpointer)
{
	const char *text = gtk_editable_get_text(self);
	strcpy(l->ceilOrSkyTex, text);
}

/**
 * Callback for when the level floor texture is changed
 */
void level_floor_tex_changed(GtkEditable *self, gpointer)
{
	const char *text = gtk_editable_get_text(self);
	strcpy(l->floorTex, text);
}

/**
 * Callback for when the level music is changed
 */
void level_music_changed(GtkEditable *self, gpointer)
{
	const char *text = gtk_editable_get_text(self);
	strcpy(l->music, text);
}

/**
 * Callback for when the level fog color is changed
 */
void fog_color_set(GtkColorButton *self, gpointer)
{
	GdkRGBA gtkColor;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(self), &gtkColor);
	const uint color = (uint)(gtkColor.red * 255) << 24 |
					   (uint)(gtkColor.green * 255) << 16 |
					   (uint)(gtkColor.blue * 255) << 8 |
					   (uint)(gtkColor.alpha * 255);
	l->fogColor = color;
}

/**
 * Callback for when the fog start value is changed
 */
void fog_start_value_changed(GtkRange *self, gpointer)
{
	l->fogStart = gtk_range_get_value(self);
}

/**
 * Callback for when the level fog end value is changed
 */
void fog_end_value_changed(GtkRange *self, gpointer)
{
	l->fogEnd = gtk_range_get_value(self);
}

#pragma endregion

#pragma endregion

/**
 * Menu item actions and callbacks
 */
static GActionEntry menu_entries[] = {
	{"new", new_activated, NULL, NULL, NULL},
	{"open", open_activated, NULL, NULL, NULL},
	{"save", save_activated, NULL, NULL, NULL},
	{"quit", quit_activated, NULL, NULL, NULL},
	{"delete_selected", delete_selected_menu_item_activated, NULL, NULL, NULL},
	{"zoom_in", zoom_in_activated, NULL, NULL, NULL},
	{"zoom_out", zoom_out_activated, NULL, NULL, NULL},
	{"reset_zoom", reset_zoom_activated, NULL, NULL, NULL},
	{"center_origin", center_origin_activated, NULL, NULL, NULL},
	{"snap_lower", snap_lower_activated, NULL, NULL, NULL},
	{"snap_reset", snap_reset_activated, NULL, NULL, NULL},
	{"snap_higher", snap_higher_activated, NULL, NULL, NULL},
	{"setup", setup_activated, NULL, NULL, NULL},
	{"about", about_activated, NULL, NULL, NULL},
};

#pragma region UI Setup

void UpdateActorSidebar()
{
	const Actor *a = ListGet(l->actors, selectionIndex);
	const ActorDefinition *def = GetActorDef(a->actorType);
	// Find the actor type in the combo box
	GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(actorTypeBox);
	for (int i = 0; i < GetActorTypeCount(); i++)
	{
		const ActorDefinition *cdef = GetActorDefByLoadIndex(i);
		if (cdef->actorType == a->actorType)
		{
			gtk_combo_box_set_active(GTK_COMBO_BOX(combo), i);
			break;
		}
	}
}

/**
 * Create and populate the menu bar
 */
GtkWidget *SetupMenuBar(GtkApplication *app)
{
	GMenu *menu = g_menu_new();

	GMenu *file_menu = g_menu_new();
	g_menu_append(file_menu, "New", "app.new");
	g_menu_append(file_menu, "Open", "app.open");
	g_menu_append(file_menu, "Save", "app.save");
	g_menu_append(file_menu, "Quit", "app.quit");
	g_menu_append_submenu(menu, "File", G_MENU_MODEL(file_menu));
	g_object_unref(file_menu);

	GMenu *edit_menu = g_menu_new();
	g_menu_append(edit_menu, "Delete Selected", "app.delete_selected");
	g_menu_append_submenu(menu, "Edit", G_MENU_MODEL(edit_menu));

	GMenu *view_menu = g_menu_new();
	g_menu_append(view_menu, "Zoom In", "app.zoom_in");
	g_menu_append(view_menu, "Zoom Out", "app.zoom_out");
	g_menu_append(view_menu, "Reset Zoom", "app.reset_zoom");
	g_menu_append(view_menu, "Center Origin", "app.center_origin");
	g_menu_append(view_menu, "Snap On Grid: Lower", "app.snap_lower");
	g_menu_append(view_menu, "Snap On Grid: Higher", "app.snap_higher");
	g_menu_append(view_menu, "Snap On Grid: Reset", "app.snap_reset");
	g_menu_append_submenu(menu, "View", G_MENU_MODEL(view_menu));

	GMenu *tools_menu = g_menu_new();
	g_menu_append(tools_menu, "Setup", "app.setup");
	g_menu_append_submenu(menu, "Tools", G_MENU_MODEL(tools_menu));
	g_object_unref(tools_menu);

	GMenu *help_menu = g_menu_new();
	g_menu_append(help_menu, "About", "app.about");
	g_menu_append_submenu(menu, "Help", G_MENU_MODEL(help_menu));
	g_object_unref(help_menu);

	GtkWidget *menuBar = gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu));

	g_action_map_add_action_entries(G_ACTION_MAP(app), menu_entries, G_N_ELEMENTS(menu_entries), app);

	const gchar *new_accels[] = {"<Ctrl>N", NULL};
	gtk_application_set_accels_for_action(app, "app.new", new_accels);

	const gchar *open_accels[] = {"<Ctrl>O", NULL};
	gtk_application_set_accels_for_action(app, "app.open", open_accels);

	const gchar *save_accels[] = {"<Ctrl>S", NULL};
	gtk_application_set_accels_for_action(app, "app.save", save_accels);

	const gchar *quit_accels[] = {"<Ctrl>Q", NULL};
	gtk_application_set_accels_for_action(app, "app.quit", quit_accels);

	const gchar *zoom_in_accels[] = {"<Ctrl>equal", NULL};
	gtk_application_set_accels_for_action(app, "app.zoom_in", zoom_in_accels);

	const gchar *zoom_out_accels[] = {"<Ctrl>minus", NULL};
	gtk_application_set_accels_for_action(app, "app.zoom_out", zoom_out_accels);

	const gchar *reset_zoom_accels[] = {"<Ctrl>0", NULL};
	gtk_application_set_accels_for_action(app, "app.reset_zoom", reset_zoom_accels);

	const gchar *center_origin_accels[] = {"<Ctrl>Home", NULL};
	gtk_application_set_accels_for_action(app, "app.center_origin", center_origin_accels);

	const gchar *add_actor_accels[] = {"<Shift>A", NULL};
	gtk_application_set_accels_for_action(app, "app.add_actor", add_actor_accels);

	const gchar *delete_selected_accels[] = {"Delete", NULL};
	gtk_application_set_accels_for_action(app, "app.delete_selected", delete_selected_accels);

	const gchar *snap_lower_accels[] = {"bracketleft", NULL};
	gtk_application_set_accels_for_action(app, "app.snap_lower", snap_lower_accels);
	const gchar *snap_higher_accels[] = {"bracketright", NULL};
	gtk_application_set_accels_for_action(app, "app.snap_higher", snap_higher_accels);

	return menuBar;
}

/**
 * Create and populate the toolbar
 */
GtkWidget *SetupToolbar()
{
	GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

	addWallsButton = gtk_toggle_button_new_with_label("Add Walls");
	g_signal_connect(addWallsButton, "clicked", G_CALLBACK(add_wall_clicked), NULL);
	addActorsButton = gtk_toggle_button_new_with_label("Add Actors");
	g_signal_connect(addActorsButton, "clicked", G_CALLBACK(add_actor_clicked), NULL);

	GtkWidget *deleteSelectedButton = gtk_button_new_with_label("Delete Selected");
	g_signal_connect(deleteSelectedButton, "clicked", G_CALLBACK(delete_selected_clicked), NULL);

	GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_size_request(toolbar, -1, 30);

	gtk_box_append(GTK_BOX(toolbar), addWallsButton);
	gtk_box_append(GTK_BOX(toolbar), addActorsButton);
	gtk_box_append(GTK_BOX(toolbar), sep);
	gtk_box_append(GTK_BOX(toolbar), deleteSelectedButton);

	return toolbar;
}

/**
 * Create the drawing area
 */
GtkWidget *SetupDrawingArea()
{
	GtkWidget *drawingArea = gtk_drawing_area_new();
	gtk_widget_add_css_class(drawingArea, "editor");
	gtk_widget_set_name(drawingArea, "editor");

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

/**
 * Create and populate the level sidebar
 */
GtkWidget *SetupRSidebar()
{
	GtkWidget *rightSidebar = gtk_scrolled_window_new();
	gtk_widget_set_margin_end(rightSidebar, 8);
	gtk_widget_set_size_request(rightSidebar, 250, -1);
	gtk_widget_set_hexpand(rightSidebar, FALSE);

	GtkWidget *rightSidebarVLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(rightSidebar), rightSidebarVLayout);
	GtkWidget *rightSidebarHeader = gtk_label_new("Level Properties");
	gtk_label_set_xalign(GTK_LABEL(rightSidebarHeader), 0);
	gtk_widget_add_css_class(rightSidebarHeader, "sidebarHeader");
	gtk_widget_set_margin_bottom(rightSidebarHeader, 8);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), rightSidebarHeader);

	GtkWidget *levelNameLabel = gtk_label_new("Level Name");
	gtk_label_set_xalign(GTK_LABEL(levelNameLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), levelNameLabel);
	GtkWidget *levelNameBox = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(levelNameBox), "Level Name");
	gtk_entry_set_max_length(GTK_ENTRY(levelNameBox), 30);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(levelNameBox)), l->name, -1);
	g_signal_connect(levelNameBox, "changed", G_CALLBACK(level_name_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), levelNameBox);

	GtkWidget *courseNumLabel = gtk_label_new("Course Number");
	gtk_label_set_xalign(GTK_LABEL(courseNumLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), courseNumLabel);

	GtkWidget *courseNumBox = gtk_spin_button_new_with_range(-1, 100, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(courseNumBox), l->courseNum);
	g_signal_connect(courseNumBox, "value-changed", G_CALLBACK(level_course_num_value_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), courseNumBox);

	GtkWidget *sep1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_margin_top(sep1, 8);
	gtk_widget_set_margin_bottom(sep1, 8);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), sep1);

	GtkWidget *ceilOrSkyHBox = gtk_center_box_new();
	gtk_widget_set_hexpand(ceilOrSkyHBox, TRUE);
	gtk_widget_set_margin_bottom(ceilOrSkyHBox, 8);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), ceilOrSkyHBox);

	GtkWidget *ceilLabel = gtk_label_new("Ceiling");
	gtk_label_set_xalign(GTK_LABEL(ceilLabel), 1);
	GtkWidget *skyLabel = gtk_label_new("Sky");

	GtkWidget *ceilSkySwitch = gtk_switch_new();
	gtk_widget_add_css_class(ceilSkySwitch, "skySwitch");
	gtk_widget_set_hexpand(ceilSkySwitch, FALSE);
	gtk_switch_set_state(GTK_SWITCH(ceilSkySwitch), l->hasCeiling);
	g_signal_connect(ceilSkySwitch, "state-set", G_CALLBACK(level_ceil_or_sky_state_set), NULL);

	gtk_center_box_set_end_widget(GTK_CENTER_BOX(ceilOrSkyHBox), ceilLabel);
	gtk_center_box_set_center_widget(GTK_CENTER_BOX(ceilOrSkyHBox), ceilSkySwitch);
	gtk_center_box_set_start_widget(GTK_CENTER_BOX(ceilOrSkyHBox), skyLabel);

	GtkWidget *ceilOrSkyLabel = gtk_label_new("Ceiling/Sky Texture");
	gtk_label_set_xalign(GTK_LABEL(ceilOrSkyLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), ceilOrSkyLabel);
	GtkWidget *ceilOrSkyTex = gtk_combo_box_text_new_with_entry();
	PopulateComboBoxTextures(ceilOrSkyTex);
	GtkEntry *entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(ceilOrSkyTex)));
	gtk_entry_set_max_length(entry, 30);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), l->ceilOrSkyTex, -1);
	g_signal_connect(entry, "changed", G_CALLBACK(level_ceil_or_sky_tex_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), ceilOrSkyTex);

	GtkWidget *floorTexLabel = gtk_label_new("Floor Texture");
	gtk_label_set_xalign(GTK_LABEL(floorTexLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), floorTexLabel);
	GtkWidget *floorTex = gtk_combo_box_text_new_with_entry();
	PopulateComboBoxTextures(floorTex);
	entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(floorTex)));
	gtk_entry_set_max_length(entry, 30);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), l->floorTex, -1);
	g_signal_connect(entry, "changed", G_CALLBACK(level_floor_tex_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), floorTex);

	GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_margin_top(sep2, 8);
	gtk_widget_set_margin_bottom(sep2, 8);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), sep2);

	GtkWidget *musicLabel = gtk_label_new("Music");
	gtk_label_set_xalign(GTK_LABEL(musicLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), musicLabel);
	GtkWidget *musicBox = gtk_combo_box_text_new_with_entry();
	PopulateComboBoxMusic(musicBox);
	entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(musicBox)));
	gtk_entry_set_max_length(entry, 30);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), l->music, -1);
	g_signal_connect(entry, "changed", G_CALLBACK(level_music_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), musicBox);

	GtkWidget *sep3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_margin_top(sep3, 8);
	gtk_widget_set_margin_bottom(sep3, 8);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), sep3);

	GtkWidget *fogColorLabel = gtk_label_new("Fog Color");
	gtk_label_set_xalign(GTK_LABEL(fogColorLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), fogColorLabel);
	GdkRGBA fogColorRGBA = {0, 0, 0, 1};
	fogColorRGBA.red = (l->fogColor >> 24) / 255.0;
	fogColorRGBA.green = ((l->fogColor >> 16) & 0xFF) / 255.0;
	fogColorRGBA.blue = ((l->fogColor >> 8) & 0xFF) / 255.0;
	fogColorRGBA.alpha = (l->fogColor & 0xFF) / 255.0;
	GtkWidget *fogColor = gtk_color_button_new_with_rgba(&fogColorRGBA);
	g_signal_connect(fogColor, "color-set", G_CALLBACK(fog_color_set), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), fogColor);

	GtkWidget *fogStartLabel = gtk_label_new("Fog Start");
	gtk_label_set_xalign(GTK_LABEL(fogStartLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), fogStartLabel);

	GtkWidget *fogStartSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, -50, 200, 1);
	gtk_scale_set_value_pos(GTK_SCALE(fogStartSlider), GTK_POS_RIGHT);
	gtk_scale_set_digits(GTK_SCALE(fogStartSlider), 0);
	gtk_scale_set_draw_value(GTK_SCALE(fogStartSlider), TRUE);
	gtk_range_set_value(GTK_RANGE(fogStartSlider), l->fogStart);
	g_signal_connect(fogStartSlider, "value-changed", G_CALLBACK(fog_start_value_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), fogStartSlider);

	GtkWidget *fogEndLabel = gtk_label_new("Fog End");
	gtk_label_set_xalign(GTK_LABEL(fogEndLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), fogEndLabel);

	GtkWidget *fogEndSlider = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, 0, 300, 1);
	gtk_scale_set_value_pos(GTK_SCALE(fogEndSlider), GTK_POS_RIGHT);
	gtk_scale_set_digits(GTK_SCALE(fogEndSlider), 0);
	gtk_scale_set_draw_value(GTK_SCALE(fogEndSlider), TRUE);
	gtk_range_set_value(GTK_RANGE(fogEndSlider), l->fogEnd);
	g_signal_connect(fogEndSlider, "value-changed", G_CALLBACK(fog_end_value_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), fogEndSlider);

	return rightSidebar;
}

/**
 * Create the no selection sidebar
 */
GtkWidget *SetupLSidebar_NoSelection()
{
	GtkWidget *noSelectionSidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	GtkWidget *noSelectionLabel = gtk_label_new("No Selection");
	gtk_label_set_xalign(GTK_LABEL(noSelectionLabel), 0);
	gtk_widget_add_css_class(noSelectionLabel, "grayText");
	gtk_box_append(GTK_BOX(noSelectionSidebar), noSelectionLabel);

	return noSelectionSidebar;
}

/**
 * Create the wall sidebar
 */
GtkWidget *SetupLSidebar_WallSelection(const Wall *w)
{
	GtkWidget *wallSelectionSidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	GtkWidget *textureLabel = gtk_label_new("Texture");
	gtk_label_set_xalign(GTK_LABEL(textureLabel), 0);
	gtk_box_append(GTK_BOX(wallSelectionSidebar), textureLabel);
	GtkWidget *textureBox = gtk_combo_box_text_new_with_entry();
	PopulateComboBoxTextures(textureBox);
	GtkEntry *entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(textureBox)));
	gtk_entry_set_max_length(entry, 30);
	GtkEntryBuffer *buffer = gtk_entry_get_buffer(entry);
	gtk_entry_buffer_set_text(buffer, w->tex, -1);
	g_signal_connect(entry, "changed", G_CALLBACK(wall_texture_changed), NULL);
	gtk_box_append(GTK_BOX(wallSelectionSidebar), textureBox);

	GtkWidget *sep1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_margin_top(sep1, 8);
	gtk_widget_set_margin_bottom(sep1, 8);
	gtk_box_append(GTK_BOX(wallSelectionSidebar), sep1);

	GtkWidget *uvScaleLabel = gtk_label_new("UV Scale");
	gtk_label_set_xalign(GTK_LABEL(uvScaleLabel), 0);
	gtk_box_append(GTK_BOX(wallSelectionSidebar), uvScaleLabel);

	GtkWidget *uvScaleSpin = gtk_spin_button_new_with_range(0.1, 10, 0.1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(uvScaleSpin), w->uvScale);
	g_signal_connect(uvScaleSpin, "value-changed", G_CALLBACK(wall_uv_scale_value_changed), NULL);
	gtk_box_append(GTK_BOX(wallSelectionSidebar), uvScaleSpin);

	GtkWidget *uvOffsetLabel = gtk_label_new("UV Offset");
	gtk_label_set_xalign(GTK_LABEL(uvOffsetLabel), 0);
	gtk_box_append(GTK_BOX(wallSelectionSidebar), uvOffsetLabel);

	GtkWidget *uvOffsetSpin = gtk_spin_button_new_with_range(-1, 1, 0.01);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(uvOffsetSpin), w->uvOffset);
	g_signal_connect(uvOffsetSpin, "value-changed", G_CALLBACK(wall_uv_offset_value_changed), NULL);
	gtk_box_append(GTK_BOX(wallSelectionSidebar), uvOffsetSpin);

	return wallSelectionSidebar;
}

/**
 * Create the actor sidebar
 */
GtkWidget *SetupLSidebar_ActorSelection(const Actor *a)
{
	GtkWidget *actorSelectionSidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	actorTypeLabel = gtk_label_new("Actor Type");
	gtk_label_set_xalign(GTK_LABEL(actorTypeLabel), 0);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), actorTypeLabel);

	actorTypeBox = gtk_combo_box_text_new();
	PopulateComboBoxActors(actorTypeBox);
	gtk_combo_box_set_active(GTK_COMBO_BOX(actorTypeBox), a->actorType);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), actorTypeBox);
	g_signal_connect(actorTypeBox, "changed", G_CALLBACK(actor_type_changed), NULL);

	GtkWidget *actorNameLabel = gtk_label_new("Actor Name");
	gtk_label_set_xalign(GTK_LABEL(actorNameLabel), 0);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), actorNameLabel);

	actorNameBox = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(actorNameBox), "Name");
	gtk_entry_set_max_length(GTK_ENTRY(actorNameBox), 60);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(actorNameBox)), a->name, -1);
	g_signal_connect(actorNameBox, "changed", G_CALLBACK(actor_name_changed), NULL);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), actorNameBox);

	GtkWidget *sep1 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_margin_top(sep1, 8);
	gtk_widget_set_margin_bottom(sep1, 8);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), sep1);

	GtkWidget *rotationLabel = gtk_label_new("Rotation");
	gtk_label_set_xalign(GTK_LABEL(rotationLabel), 0);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), rotationLabel);
	GtkWidget *rotationSpin = gtk_spin_button_new_with_range(0, 359, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(rotationSpin), radToDeg(a->rotation));
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(rotationSpin), TRUE);
	g_signal_connect(rotationSpin, "value-changed", G_CALLBACK(actor_rot_value_changed), NULL);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), rotationSpin);

	GtkWidget *sep3 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_widget_set_margin_top(sep3, 8);
	gtk_widget_set_margin_bottom(sep3, 8);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), sep3);

	GtkWidget *editKvButton = gtk_button_new_with_label("Edit Parameters");
	gtk_widget_set_margin_bottom(editKvButton, 4);
	g_signal_connect(editKvButton, "clicked", G_CALLBACK(edit_kv_clicked), NULL);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), editKvButton);

	GtkWidget *editIoButton = gtk_button_new_with_label("I/O Connections");
	g_signal_connect(editIoButton, "clicked", G_CALLBACK(edit_io_clicked), NULL);
	gtk_box_append(GTK_BOX(actorSelectionSidebar), editIoButton);

	UpdateActorSidebar();

	return actorSelectionSidebar;
}

/**
 * Create the player sidebar
 */
GtkWidget *SetupLSidebar_PlayerSelection()
{
	GtkWidget *playerSelectionSidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	GtkWidget *rotationLabel = gtk_label_new("Rotation");
	gtk_label_set_xalign(GTK_LABEL(rotationLabel), 0);
	gtk_box_append(GTK_BOX(playerSelectionSidebar), rotationLabel);
	GtkWidget *rotationSpin = gtk_spin_button_new_with_range(0, 359, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(rotationSpin), radToDeg(l->player.rotation));
	gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(rotationSpin), TRUE);
	g_signal_connect(rotationSpin, "value-changed", G_CALLBACK(plr_rot_value_changed), NULL);
	gtk_box_append(GTK_BOX(playerSelectionSidebar), rotationSpin);

	return playerSelectionSidebar;
}

#pragma endregion

/**
 * Apply the CSS styles
 */
void SetupCss(GtkWindow *)
{
	// Load CSS
	GtkCssProvider *provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider, "./Assets/style.css"); // Your CSS file path

	// Get the default GdkDisplay and GtkStyleContext
	GdkDisplay *display = gdk_display_get_default();

	// Add the provider to the display's default screen
	gtk_style_context_add_provider_for_display(display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
}

void MainWindowActivate(GtkApplication *app, gpointer *)
{
	if (!IsValidGameDirectory(&options))
	{
		OptionsWindowShow(mainWindow, app, true);
		while (optionsWindowOpen)
		{
			g_main_context_iteration(NULL, TRUE);
		}
	}

	if (!RescanAssets())
	{
		printf("Failed to rescan assets\n");
	}

	mainWindowApplication = app;

	GtkWidget *window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "Game Level Editor");
	gtk_window_set_default_size(GTK_WINDOW(window), 1440, 900);

	SetupCss(GTK_WINDOW(window));

	GtkWidget *menuBar = SetupMenuBar(app);

	GtkWidget *drawingArea = SetupDrawingArea();

	GtkWidget *leftSidebar = gtk_scrolled_window_new();
	gtk_widget_set_margin_start(leftSidebar, 8);
	gtk_widget_set_size_request(leftSidebar, 250, -1);
	gtk_widget_set_hexpand(leftSidebar, FALSE);
	leftSidebarVLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	GtkWidget *leftSidebarHeader = gtk_label_new("Selection Properties");
	gtk_label_set_xalign(GTK_LABEL(leftSidebarHeader), 0);
	gtk_widget_add_css_class(leftSidebarHeader, "sidebarHeader");
	gtk_widget_set_margin_bottom(leftSidebarHeader, 8);
	gtk_box_append(GTK_BOX(leftSidebarVLayout), leftSidebarHeader);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(leftSidebar), leftSidebarVLayout);

	SelectionTypeChanged(SELTYPE_NONE, 0);

	GtkWidget *rightSidebar = SetupRSidebar();

	GtkGrid *mainHLayout = GTK_GRID(gtk_grid_new());
	gtk_grid_set_column_homogeneous(mainHLayout, FALSE);
	gtk_grid_set_column_spacing(mainHLayout, 8);
	gtk_grid_attach(mainHLayout, leftSidebar, 0, 0, 1, 1);
	gtk_grid_attach(mainHLayout, drawingArea, 1, 0, 1, 1);
	gtk_grid_attach(mainHLayout, rightSidebar, 2, 0, 1, 1);

	GtkWidget *toolbar = SetupToolbar();

	GtkWidget *mainVLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
	gtk_box_append(GTK_BOX(mainVLayout), menuBar);
	gtk_box_append(GTK_BOX(mainVLayout), toolbar);
	gtk_box_append(GTK_BOX(mainVLayout), GTK_WIDGET(mainHLayout));

	gtk_window_set_child(GTK_WINDOW(window), mainVLayout);

	fileDialog = gtk_file_dialog_new();
	GtkFileFilter *binFilter = gtk_file_filter_new();
	gtk_file_filter_add_pattern(binFilter, "*.bin");
	gtk_file_filter_set_name(binFilter, "Level Files (Uncompressed) (*.bin)");

	gtk_file_dialog_set_filters(fileDialog, G_LIST_MODEL(g_list_store_new(GTK_TYPE_FILE_FILTER)));
	g_list_store_append(G_LIST_STORE(gtk_file_dialog_get_filters(fileDialog)), binFilter);

	mainWindow = GTK_WINDOW(window);

	gtk_window_present(GTK_WINDOW(window));
}

void SelectionTypeChanged()
{
	if (leftSidebarCurrent != NULL)
	{
		gtk_box_remove(GTK_BOX(leftSidebarVLayout), leftSidebarCurrent);
	}

	GtkWidget *newSidebar;
	switch (selectionType)
	{
		case SELTYPE_NONE:
			newSidebar = SetupLSidebar_NoSelection();
			break;
		case SELTYPE_WALL_A:
		case SELTYPE_WALL_B:
		case SELTYPE_WALL_LINE:
			newSidebar = SetupLSidebar_WallSelection(ListGet(l->walls, selectionIndex));
			break;
		case SELTYPE_ACTOR:
			newSidebar = SetupLSidebar_ActorSelection(ListGet(l->actors, selectionIndex));
			break;
		case SELTYPE_PLAYER:
			newSidebar = SetupLSidebar_PlayerSelection();
			break;
		default:
			newSidebar = SetupLSidebar_NoSelection();
			break;
	}
	gtk_box_append(GTK_BOX(leftSidebarVLayout), newSidebar);
	leftSidebarCurrent = newSidebar;
}
