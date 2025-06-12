//
// Created by droc101 on 5/29/25.
//

#include "KvWindow.h"

#include "../Editor.h"
#include "../Helpers/GameInterface.h"
#include "../Helpers/KVList.h"
#include "Message.h"
#include "UiHelpers.h"

Actor *kvActor = NULL;
AdwDialog *kvWindow;
GtkWidget *kvListBox;
GtkWidget *valueWidget = NULL;
GtkWidget *valueBox;

int kvUpdateCount = 0;

void KvWindowReloadList()
{
	kvUpdateCount++;

	GtkListBoxRow *selectedRow = gtk_list_box_get_selected_row(GTK_LIST_BOX(kvListBox));
	int selection = -1;
	if (selectedRow != NULL)
	{
		selection = gtk_list_box_row_get_index(GTK_LIST_BOX_ROW(selectedRow));
	}

	gtk_list_box_remove_all(GTK_LIST_BOX(kvListBox));

	for (int i = 0; i < KvListLength(&kvActor->params); i++)
	{
		const char *key = KvListGetKeyName(&kvActor->params, i);
		const Param *value = KvGet(&kvActor->params, key);
		GtkWidget *row = gtk_list_box_row_new();
		g_object_set_data(G_OBJECT(row), "key", (gpointer)key);
		GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

		GtkWidget *keyLabel = gtk_label_new(key);
		gtk_label_set_xalign(GTK_LABEL(keyLabel), 0);
		gtk_label_set_ellipsize(GTK_LABEL(keyLabel), PANGO_ELLIPSIZE_END);
		gtk_label_set_max_width_chars(GTK_LABEL(keyLabel), 20);
		gtk_widget_set_size_request(keyLabel, 200, -1);
		gtk_box_append(GTK_BOX(hbox), keyLabel);

		GtkWidget *valueLabel = gtk_label_new("Param: ");
		gtk_label_set_xalign(GTK_LABEL(valueLabel), 0);
		gtk_label_set_ellipsize(GTK_LABEL(valueLabel), PANGO_ELLIPSIZE_END);
		gtk_label_set_max_width_chars(GTK_LABEL(valueLabel), 20);
		gtk_widget_set_size_request(valueLabel, 248, -1);
		char paramText[128];
		switch (value->type)
		{
			case PARAM_TYPE_NONE:
				strncpy(paramText, "None", 128);
				break;
			case PARAM_TYPE_BYTE:
				snprintf(paramText, 128, "%d", value->byteValue);
				break;
			case PARAM_TYPE_INTEGER:
				snprintf(paramText, 128, "%d", value->intValue);
				break;
			case PARAM_TYPE_FLOAT:
				snprintf(paramText, 128, "%.2f", value->floatValue);
				break;
			case PARAM_TYPE_BOOL:
				snprintf(paramText, 128, "%s", value->boolValue ? "true" : "false");
				break;
			case PARAM_TYPE_STRING:
				snprintf(paramText, 128, "\"%s\"", value->stringValue);
				break;
			case PARAM_TYPE_COLOR:
				snprintf(paramText,
						 128,
						 "Color (%.2f, %.2f, %.2f, %.2f)",
						 value->colorValue.r,
						 value->colorValue.g,
						 value->colorValue.b,
						 value->colorValue.a);
				break;
		}
		gtk_label_set_text(GTK_LABEL(valueLabel), paramText);
		gtk_box_append(GTK_BOX(hbox), valueLabel);


		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), hbox);

		gtk_list_box_append(GTK_LIST_BOX(kvListBox), row);
	}

	const size_t rows = KvListLength(&kvActor->params);
	if (selection <= rows)
	{
		gtk_list_box_select_row(GTK_LIST_BOX(kvListBox),
								gtk_list_box_get_row_at_index(GTK_LIST_BOX(kvListBox), selection));
	}


	kvUpdateCount--;
}

char *GetCurrentKey()
{
	return gtk_list_box_get_selected_row(GTK_LIST_BOX(kvListBox))
				   ? g_object_get_data(G_OBJECT(gtk_list_box_get_selected_row(GTK_LIST_BOX(kvListBox))), "key")
				   : NULL;
}

ActorDefParam *GetCurrentParamDef()
{
	const char *key = GetCurrentKey();
	if (key == NULL)
	{
		return NULL;
	}
	return GetActorDefParam(kvActor->actorType, key);
}

void RemoveExistingKVWidget()
{
	if (valueWidget)
	{
		gtk_box_remove(GTK_BOX(valueBox), valueWidget);
		valueWidget = NULL;
	}
}

void kv_byte_value_changed(GtkSpinButton *self, gpointer)
{
	KvSetByte(&kvActor->params, GetCurrentKey(), gtk_spin_button_get_value_as_int(self));
	KvWindowReloadList();
}

void CreateKVByte()
{
	RemoveExistingKVWidget();
	const ActorDefParam *def = GetActorDefParam(kvActor->actorType, GetCurrentKey());
	valueWidget = gtk_spin_button_new_with_range(def->byteDef.minValue, def->byteDef.maxValue, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(valueWidget), KvGetByte(&kvActor->params, GetCurrentKey(), 0));
	g_signal_connect(valueWidget, "value-changed", G_CALLBACK(kv_byte_value_changed), NULL);
	gtk_widget_set_hexpand(valueWidget, true);
	gtk_box_append(GTK_BOX(valueBox), valueWidget);
}

void kv_int_value_changed(GtkSpinButton *self, gpointer)
{
	KvSetInt(&kvActor->params, GetCurrentKey(), gtk_spin_button_get_value_as_int(self));
	KvWindowReloadList();
}

void CreateKVInteger()
{
	RemoveExistingKVWidget();
	const ActorDefParam *def = GetActorDefParam(kvActor->actorType, GetCurrentKey());
	valueWidget = gtk_spin_button_new_with_range(def->intDef.minValue, def->intDef.maxValue, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(valueWidget), KvGetInt(&kvActor->params, GetCurrentKey(), 0));
	g_signal_connect(valueWidget, "value-changed", G_CALLBACK(kv_int_value_changed), NULL);
	gtk_widget_set_hexpand(valueWidget, true);
	gtk_box_append(GTK_BOX(valueBox), valueWidget);
}

void kv_float_value_changed(GtkSpinButton *self, gpointer)
{
	KvSetFloat(&kvActor->params, GetCurrentKey(), gtk_spin_button_get_value(self));
	KvWindowReloadList();
}

void CreateKVFloat()
{
	RemoveExistingKVWidget();
	const ActorDefParam *def = GetActorDefParam(kvActor->actorType, GetCurrentKey());
	valueWidget = gtk_spin_button_new_with_range(def->floatDef.minValue, def->floatDef.maxValue, def->floatDef.step);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(valueWidget), KvGetFloat(&kvActor->params, GetCurrentKey(), 0.0f));
	g_signal_connect(valueWidget, "value-changed", G_CALLBACK(kv_float_value_changed), NULL);
	gtk_widget_set_hexpand(valueWidget, true);
	gtk_box_append(GTK_BOX(valueBox), valueWidget);
}

void kv_bool_toggled(GtkSwitch *, const gboolean state, gpointer)
{
	KvSetBool(&kvActor->params, GetCurrentKey(), state);
	KvWindowReloadList();
}

void CreateKVBool()
{
	RemoveExistingKVWidget();
	const ActorDefParam *def = GetActorDefParam(kvActor->actorType, GetCurrentKey());
	valueWidget = gtk_switch_new();
	gtk_switch_set_active(GTK_SWITCH(valueWidget),
						  KvGetBool(&kvActor->params, GetCurrentKey(), def->boolDef.defaultValue));
	g_signal_connect(valueWidget, "state-set", G_CALLBACK(kv_bool_toggled), NULL);
	gtk_box_append(GTK_BOX(valueBox), valueWidget);
}

void kv_string_changed(GtkEditable *self, gpointer)
{
	KvSetString(&kvActor->params, GetCurrentKey(), gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(self))));
	KvWindowReloadList();
}

void CreateKVString()
{
	RemoveExistingKVWidget();
	valueWidget = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(valueWidget), 64);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(valueWidget)),
							  KvGetString(&kvActor->params, GetCurrentKey(), ""),
							  -1);
	g_signal_connect(valueWidget, "changed", G_CALLBACK(kv_string_changed), NULL);
	gtk_widget_set_hexpand(valueWidget, true);
	gtk_box_append(GTK_BOX(valueBox), valueWidget);
}

void CreateKVHintString(const StringParamHint hint)
{
	RemoveExistingKVWidget();
	valueWidget = gtk_combo_box_text_new_with_entry();
	switch (hint)
	{
		case HINT_MODEL:
			PopulateComboBoxModels(valueWidget);
			break;
		case HINT_TEXTURE:
			PopulateComboBoxTextures(valueWidget);
			break;
		case HINT_SOUND:
			PopulateComboBoxSounds(valueWidget);
			break;
		case HINT_LEVEL:
			PopulateComboBoxLevels(valueWidget);
			break;
		case HINT_ACTOR:
			PopulateComboBoxWithActorNames(valueWidget, "");
			break;
		case HINT_MUSIC:
			PopulateComboBoxMusic(valueWidget);
			break;
		default:
			break;
	}
	GtkEntry *entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(valueWidget)));
	gtk_entry_set_max_length(entry, 30);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), KvGetString(&kvActor->params, GetCurrentKey(), ""), -1);
	g_signal_connect(entry, "changed", G_CALLBACK(kv_string_changed), NULL);
	gtk_widget_set_hexpand(valueWidget, true);
	gtk_box_append(GTK_BOX(valueBox), valueWidget);
}

void kv_color_changed(GtkColorButton *self, gpointer)
{
	GdkRGBA color;
	gtk_color_chooser_get_rgba(GTK_COLOR_CHOOSER(self), &color);
	const Color c = {color.red, color.green, color.blue, color.alpha};
	KvSetColor(&kvActor->params, GetCurrentKey(), c);
	KvWindowReloadList();
}

void CreateKVColor()
{
	RemoveExistingKVWidget();
	const ActorDefParam *def = GetActorDefParam(kvActor->actorType, GetCurrentKey());
	const Color c = KvGetColor(&kvActor->params, GetCurrentKey(), def->colorDef.defaultValue);
	const GdkRGBA rgba = {c.r, c.g, c.b, c.a};
	valueWidget = gtk_color_button_new_with_rgba(&rgba);
	gtk_color_dialog_set_with_alpha(GTK_COLOR_DIALOG(valueWidget), def->colorDef.hasAlpha);
	g_signal_connect(valueWidget, "color-set", G_CALLBACK(kv_color_changed), NULL);
	gtk_widget_set_hexpand(valueWidget, true);
	gtk_box_append(GTK_BOX(valueBox), valueWidget);
}

void KvWindowReloadBoxes()
{
	kvUpdateCount++;
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(kvListBox));
	if (row != NULL)
	{
		const char *key = g_object_get_data(G_OBJECT(row), "key");
		const ActorDefParam *paramDef = GetActorDefParam(kvActor->actorType, key);

		switch (paramDef->type)
		{
			case PARAM_TYPE_BYTE:
				CreateKVByte();
				break;
			case PARAM_TYPE_INTEGER:
				CreateKVInteger();
				break;
			case PARAM_TYPE_FLOAT:
				CreateKVFloat();
				break;
			case PARAM_TYPE_BOOL:
				CreateKVBool();
				break;
			case PARAM_TYPE_STRING:
				if (paramDef->stringDef.hint == HINT_NONE)
				{
					CreateKVString();
				} else
				{
					CreateKVHintString(paramDef->stringDef.hint);
				}
				break;
			case PARAM_TYPE_COLOR:
				CreateKVColor();
				break;
			default:
				break;
		}
	}
	kvUpdateCount--;
}

/**
 * Callback for when the OK button is pressed
 */
void kv_ok_clicked(GtkButton *, gpointer)
{
	adw_dialog_close(kvWindow);
	valueWidget = NULL;
}

void kv_row_selected(GtkListBox *, GtkListBoxRow *, gpointer)
{
	if (kvUpdateCount != 0)
	{
		return;
	}
	KvWindowReloadBoxes();
	KvWindowReloadList();
}

void KvWindowShow(GtkWindow *parent, Actor *actor)
{
	valueWidget = NULL;
	kvActor = actor;
	AdwDialog *window = adw_dialog_new();
	adw_dialog_set_title(window, "Actor Parameters");
	adw_dialog_set_content_width(window, 600);
	kvWindow = window;

	GtkWidget *mainStack = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_margin_start(mainStack, 12);
	gtk_widget_set_margin_end(mainStack, 12);
	gtk_widget_set_margin_top(mainStack, 12);
	gtk_widget_set_margin_bottom(mainStack, 12);

	GtkWidget *headerLabel = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(headerLabel), "<big><b>Actor Parameters</b></big>");
	gtk_label_set_xalign(GTK_LABEL(headerLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), headerLabel);

	GtkWidget *headerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(headerBox, TRUE);

	GtkWidget *myOutputHeaderLabel = gtk_label_new("Key");
	gtk_label_set_xalign(GTK_LABEL(myOutputHeaderLabel), 0);
	gtk_widget_set_size_request(myOutputHeaderLabel, 200, -1);
	gtk_box_append(GTK_BOX(headerBox), myOutputHeaderLabel);

	GtkWidget *paramOverrideHeaderLabel = gtk_label_new("Value");
	gtk_label_set_xalign(GTK_LABEL(paramOverrideHeaderLabel), 0);
	gtk_widget_set_size_request(paramOverrideHeaderLabel, 250, -1);
	gtk_box_append(GTK_BOX(headerBox), paramOverrideHeaderLabel);

	gtk_box_append(GTK_BOX(mainStack), headerBox);

	GtkWidget *scrollBox = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollBox), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrollBox, -1, 200);
	gtk_widget_set_vexpand(scrollBox, TRUE);
	gtk_widget_set_hexpand(scrollBox, TRUE);
	gtk_box_append(GTK_BOX(mainStack), scrollBox);

	kvListBox = gtk_list_box_new();
	gtk_widget_set_size_request(kvListBox, -1, 200);
	gtk_widget_set_vexpand(kvListBox, TRUE);
	gtk_widget_set_hexpand(kvListBox, TRUE);
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(kvListBox), GTK_SELECTION_SINGLE);
	g_signal_connect(kvListBox, "row-selected", G_CALLBACK(kv_row_selected), NULL);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollBox), kvListBox);

	KvWindowReloadList();

	GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep);

	valueBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(valueBox, TRUE);
	gtk_box_append(GTK_BOX(mainStack), valueBox);

	GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep2);


	KvWindowReloadBoxes();

	GtkWidget *okCancelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(okCancelBox, TRUE);
	gtk_widget_set_halign(okCancelBox, GTK_ALIGN_END);

	GtkWidget *okButton = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okButton, 80, -1);
	g_signal_connect(okButton, "clicked", G_CALLBACK(kv_ok_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), okButton);

	GtkWidget *buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_append(GTK_BOX(buttonBox), okCancelBox);
	gtk_box_append(GTK_BOX(mainStack), buttonBox);
	gtk_widget_set_hexpand(buttonBox, TRUE);

	adw_dialog_set_child(window, mainStack);

	adw_dialog_present(window, GTK_WIDGET(parent));
}
