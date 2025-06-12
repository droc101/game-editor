//
// Created by droc101 on 5/29/25.
//

#include "UiHelpers.h"

#include "../Editor.h"
#include "../Helpers/GameInterface.h"
#include "TextureBrowserWindow.h"

AdwApplication *application;

/**
 * Populate a combo box with texture names
 */
void PopulateComboBoxTextures(GtkWidget *box)
{
	GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(box);
	for (int i = 0; i < textureList->length; i++)
	{
		const char *tex = ListGet(*textureList, i);
		gtk_combo_box_text_append_text(combo, tex);
	}
}

void PopulateComboBoxActors(GtkWidget *box)
{
	GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(box);
	for (int i = 0; i < GetActorTypeCount(); i++)
	{
		const ActorDefinition *def = GetActorDefByLoadIndex(i);
		gtk_combo_box_text_append_text(combo, def->actorName);
	}
}

/**
 * Populate a combo box with music names
 */
void PopulateComboBoxMusic(GtkWidget *box)
{
	GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(box);
	gtk_combo_box_text_append_text(combo, "none");
	for (int i = 0; i < musicList->length; i++)
	{
		const char *tex = ListGet(*musicList, i);
		gtk_combo_box_text_append_text(combo, tex);
	}
}

void PopulateComboBoxModels(GtkWidget *box)
{
	GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(box);
	for (int i = 0; i < modelList->length; i++)
	{
		const char *tex = ListGet(*modelList, i);
		gtk_combo_box_text_append_text(combo, tex);
	}
}

void PopulateComboBoxSounds(GtkWidget *box)
{
	GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(box);
	for (int i = 0; i < soundList->length; i++)
	{
		const char *tex = ListGet(*soundList, i);
		gtk_combo_box_text_append_text(combo, tex);
	}
}

void PopulateComboBoxLevels(GtkWidget *box)
{
	GtkComboBoxText *combo = GTK_COMBO_BOX_TEXT(box);
	for (int i = 0; i < levelList->length; i++)
	{
		const char *tex = ListGet(*levelList, i);
		gtk_combo_box_text_append_text(combo, tex);
	}
}

void PopulateComboBoxWithActorNames(GtkWidget *box, const char *selected)
{
	int selectedIndex = -1;
	int location = 0;
	gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(box));
	for (int i = 0; i < l->actors.length; i++)
	{
		const Actor *actor = ListGet(l->actors, i);
		if (actor->name[0] == '\0')
		{
			continue;
		}

		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(box), actor->name);
		if (strncmp(actor->name, selected, 64) == 0)
		{
			selectedIndex = location;
		}
		location++;
	}
	if (selectedIndex != -1)
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX(box), selectedIndex);
	}
}

void PopulateComboBoxWithSignals(GtkWidget *box, const size_t count, const ActorDefSignal *signals)
{
	gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT(box));
	for (int i = 0; i < count; i++)
	{
		const ActorDefSignal *signal = &signals[i];
		gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(box), signal->name);
	}
}

void TextureBrowseButtonPressed(GtkButton *self, gpointer *)
{
	GtkWidget *button = GTK_WIDGET(self);
	GtkWindow *wnd = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "window"));
	GtkWidget *comboBox = GTK_WIDGET(g_object_get_data(G_OBJECT(button), "comboBox"));
	TextureBrowserShow(wnd, comboBox);
}

GtkWidget *CreateTextureComboBox(const char *selected, GtkWidget *comboBox, GtkWindow *window)
{
	GtkWidget *hBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(hBox, TRUE);

	comboBox = gtk_combo_box_text_new_with_entry();
	PopulateComboBoxTextures(comboBox);
	GtkEntry *entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(comboBox)));
	gtk_entry_set_max_length(entry, 60);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), selected, -1);
	gtk_box_append(GTK_BOX(hBox), comboBox);

	GtkWidget *browseButton = gtk_button_new_with_label("Browse");
	gtk_widget_set_size_request(browseButton, 80, -1);
	g_object_set_data(G_OBJECT(browseButton), "comboBox", comboBox);
	g_object_set_data(G_OBJECT(browseButton), "window", window);
	g_signal_connect(browseButton, "clicked", G_CALLBACK(TextureBrowseButtonPressed), NULL);
	gtk_box_append(GTK_BOX(hBox), browseButton);

	return hBox;
}
