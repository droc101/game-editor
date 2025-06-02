//
// Created by droc101 on 5/29/25.
//

#include "UiHelpers.h"
#include "../Editor.h"
#include "../Helpers/GameInterface.h"

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
	gtk_combo_box_set_active(GTK_COMBO_BOX(box), selectedIndex);
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
