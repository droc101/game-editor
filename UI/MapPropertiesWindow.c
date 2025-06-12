//
// Created by droc101 on 6/1/25.
//

#include "MapPropertiesWindow.h"

#include "../Editor.h"
#include "UiHelpers.h"

AdwDialog *mpWindow;

void mp_ok_clicked(GtkButton *, gpointer)
{
	adw_dialog_close(mpWindow);
}

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


GtkWidget *SetupPropsView()
{
	GtkWidget *rightSidebar = gtk_scrolled_window_new();
	gtk_widget_set_margin_end(rightSidebar, 8);
	gtk_widget_set_size_request(rightSidebar, 250, -1);
	gtk_widget_set_hexpand(rightSidebar, TRUE);
	gtk_widget_set_vexpand(rightSidebar, TRUE);

	GtkWidget *rightSidebarVLayout = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(rightSidebar), rightSidebarVLayout);

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
	GtkWidget *ceilOrSkyTex = NULL;
	GtkWidget *ceilOrSkyBox = CreateTextureComboBox(l->ceilOrSkyTex, &ceilOrSkyTex, GTK_WINDOW(mpWindow));
	GtkEntry *entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(ceilOrSkyTex)));
	gtk_entry_set_max_length(entry, 60);
	g_signal_connect(entry, "changed", G_CALLBACK(level_ceil_or_sky_tex_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), ceilOrSkyBox);

	GtkWidget *floorTexLabel = gtk_label_new("Floor Texture");
	gtk_label_set_xalign(GTK_LABEL(floorTexLabel), 0);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), floorTexLabel);
	GtkWidget *floorTex = NULL;
	GtkWidget *floorTexBox = CreateTextureComboBox(l->floorTex, &floorTex, GTK_WINDOW(mpWindow));
	entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(floorTex)));
	gtk_entry_set_max_length(entry, 60);
	g_signal_connect(entry, "changed", G_CALLBACK(level_floor_tex_changed), NULL);
	gtk_box_append(GTK_BOX(rightSidebarVLayout), floorTexBox);

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
	gtk_entry_set_max_length(entry, 60);
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

void MPWindowShow(GtkWindow *parent)
{
	AdwDialog *window = adw_dialog_new();
	adw_dialog_set_title(window, "Level Properties");
	adw_dialog_set_content_width(window, 400);
	adw_dialog_set_content_height(window, 700);

	GtkWidget *mainStack = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_margin_start(mainStack, 12);
	gtk_widget_set_margin_end(mainStack, 12);
	gtk_widget_set_margin_top(mainStack, 12);
	gtk_widget_set_margin_bottom(mainStack, 12);

	GtkWidget *headerLabel = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(headerLabel), "<big><b>Level Properties</b></big>");
	gtk_label_set_xalign(GTK_LABEL(headerLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), headerLabel);

	GtkWidget *propsView = SetupPropsView();
	gtk_box_append(GTK_BOX(mainStack), propsView);

	GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep2);

	GtkWidget *okCancelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(okCancelBox, TRUE);
	gtk_widget_set_halign(okCancelBox, GTK_ALIGN_END);

	GtkWidget *okButton = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okButton, 80, -1);
	g_signal_connect(okButton, "clicked", G_CALLBACK(mp_ok_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), okButton);

	GtkWidget *buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_append(GTK_BOX(buttonBox), okCancelBox);
	gtk_box_append(GTK_BOX(mainStack), buttonBox);
	gtk_widget_set_hexpand(buttonBox, TRUE);

	adw_dialog_set_child(window, mainStack);

	mpWindow = window;

	adw_dialog_present(window, GTK_WIDGET(parent));
}
