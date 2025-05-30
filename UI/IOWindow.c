//
// Created by droc101 on 4/13/25.
//

#include "IOWindow.h"
#include "../Editor.h"
#include "../Helpers/GameInterface.h"
#include "UiHelpers.h"

GtkWindow *ioWindow;
GtkApplication *ioApplication;
Actor *ioActor;
GtkWidget *listBox;
GtkWidget *myOutputCombo;
GtkWidget *targetActorCombo;
GtkWidget *targetSignalCombo;
GtkWidget *paramOverrideBox;
GtkWidget *paramOverrideTypeCombo;
GtkWidget *paramOverrideValue = NULL;

int updateCount = 0;

ActorConnection *GetCurrentConnection()
{
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(listBox));
	if (row == NULL)
	{
		return NULL;
	}
	ActorConnection *conn = g_object_get_data(G_OBJECT(row), "connection");
	return conn;
}

#pragma region UI helpers

void IOWindowReloadList()
{
	updateCount++;

	GtkListBoxRow *selectedRow = gtk_list_box_get_selected_row(GTK_LIST_BOX(listBox));
	int selection = -1;
	if (selectedRow != NULL)
	{
		selection = gtk_list_box_row_get_index(GTK_LIST_BOX_ROW(selectedRow));
	}

	gtk_list_box_remove_all(GTK_LIST_BOX(listBox));

	for (int i = 0; i < ioActor->ioConnections.length; i++)
	{
		const ActorConnection *conn = ListGet(ioActor->ioConnections, i);
		GtkWidget *row = gtk_list_box_row_new();
		g_object_set_data(G_OBJECT(row), "connection", (gpointer)conn);
		GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

		const ActorDefSignal *outSignal = GetActorDefOutput(ioActor->actorType, conn->myOutput);
		GtkWidget *outputLabel = gtk_label_new(outSignal->name);
		gtk_label_set_xalign(GTK_LABEL(outputLabel), 0);
		gtk_label_set_ellipsize(GTK_LABEL(outputLabel), PANGO_ELLIPSIZE_END);
		gtk_label_set_max_width_chars(GTK_LABEL(outputLabel), 20);
		gtk_widget_set_size_request(outputLabel, 150, -1);
		gtk_box_append(GTK_BOX(hbox), outputLabel);

		GtkWidget *targetLabel = gtk_label_new(conn->outActorName);
		gtk_label_set_xalign(GTK_LABEL(targetLabel), 0);
		gtk_label_set_ellipsize(GTK_LABEL(targetLabel), PANGO_ELLIPSIZE_END);
		gtk_label_set_max_width_chars(GTK_LABEL(targetLabel), 20);
		gtk_widget_set_size_request(targetLabel, 150, -1);
		gtk_box_append(GTK_BOX(hbox), targetLabel);

		const Actor *target = NULL;
		for (int j = 0; j < l->actors.length; j++)
		{
			const Actor *a = ListGet(l->actors, j);
			if (a->name[0] != '\0' && strncmp(a->name, conn->outActorName, 64) == 0)
			{
				target = a;
				break;
			}
		}

		if (target)
		{
			const ActorDefSignal *inSignal = GetActorDefInput(target->actorType, conn->targetInput);
			GtkWidget *inputLabel = gtk_label_new(inSignal->name);
			gtk_label_set_xalign(GTK_LABEL(inputLabel), 0);
			gtk_label_set_ellipsize(GTK_LABEL(inputLabel), PANGO_ELLIPSIZE_END);
			gtk_label_set_max_width_chars(GTK_LABEL(inputLabel), 20);
			gtk_widget_set_size_request(inputLabel, 150, -1);
			gtk_box_append(GTK_BOX(hbox), inputLabel);
		} else
		{
			gtk_label_set_markup(GTK_LABEL(targetLabel), "<span foreground=\"red\">unknown</span>");
			GtkWidget *inputLabel = gtk_label_new("Invalid!");
			gtk_label_set_markup(GTK_LABEL(inputLabel), "<span foreground=\"red\">unknown</span>");
			gtk_label_set_xalign(GTK_LABEL(inputLabel), 0);
			gtk_label_set_ellipsize(GTK_LABEL(inputLabel), PANGO_ELLIPSIZE_END);
			gtk_label_set_max_width_chars(GTK_LABEL(inputLabel), 20);
			gtk_widget_set_size_request(inputLabel, 150, -1);
			gtk_box_append(GTK_BOX(hbox), inputLabel);
		}

		GtkWidget *paramLabel = gtk_label_new("Param: ");
		gtk_label_set_xalign(GTK_LABEL(paramLabel), 0);
		gtk_label_set_ellipsize(GTK_LABEL(paramLabel), PANGO_ELLIPSIZE_END);
		gtk_label_set_max_width_chars(GTK_LABEL(paramLabel), 20);
		gtk_widget_set_size_request(paramLabel, 148, -1);
		char paramText[128];
		switch (conn->outParamOverride.type)
		{
			case PARAM_TYPE_NONE:
				strncpy(paramText, "None", 128);
				break;
			case PARAM_TYPE_BYTE:
				snprintf(paramText, 128, "%d (byte)", conn->outParamOverride.byteValue);
				break;
			case PARAM_TYPE_INTEGER:
				snprintf(paramText, 128, "%d (int)", conn->outParamOverride.intValue);
				break;
			case PARAM_TYPE_FLOAT:
				snprintf(paramText, 128, "%.2f", conn->outParamOverride.floatValue);
				break;
			case PARAM_TYPE_BOOL:
				snprintf(paramText, 128, "%d (bool)", conn->outParamOverride.boolValue);
				break;
			case PARAM_TYPE_STRING:
				snprintf(paramText, 128, "\"%s\"", conn->outParamOverride.stringValue);
				break;
		}
		gtk_label_set_text(GTK_LABEL(paramLabel), paramText);
		gtk_box_append(GTK_BOX(hbox), paramLabel);


		gtk_list_box_row_set_child(GTK_LIST_BOX_ROW(row), hbox);

		gtk_list_box_append(GTK_LIST_BOX(listBox), row);
	}

	const size_t rows = ioActor->ioConnections.length;
	if (selection <= rows)
	{
		gtk_list_box_select_row(GTK_LIST_BOX(listBox), gtk_list_box_get_row_at_index(GTK_LIST_BOX(listBox), selection));
	}


	updateCount--;
}

void RemoveExistingParamValueWidget()
{
	if (!paramOverrideValue)
	{
		return;
	}
	gtk_box_remove(GTK_BOX(paramOverrideBox), paramOverrideValue);
	paramOverrideValue = NULL;
}

void param_byte_value_changed(GtkSpinButton *self, gpointer)
{
	GetCurrentConnection()->outParamOverride.type = PARAM_TYPE_BYTE;
	GetCurrentConnection()->outParamOverride.byteValue = gtk_spin_button_get_value_as_int(self);
	IOWindowReloadList();
}

void CreateParamValueByte()
{
	RemoveExistingParamValueWidget();
	paramOverrideValue = gtk_spin_button_new_with_range(0, 255, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(paramOverrideValue), GetCurrentConnection()->outParamOverride.byteValue);
	g_signal_connect(paramOverrideValue, "value-changed", G_CALLBACK(param_byte_value_changed), NULL);
	gtk_box_append(GTK_BOX(paramOverrideBox), paramOverrideValue);
}

void param_int_value_changed(GtkSpinButton *self, gpointer)
{
	GetCurrentConnection()->outParamOverride.type = PARAM_TYPE_INTEGER;
	GetCurrentConnection()->outParamOverride.intValue = gtk_spin_button_get_value_as_int(self);
	IOWindowReloadList();
}

void CreateParamValueInteger()
{
	RemoveExistingParamValueWidget();
	paramOverrideValue = gtk_spin_button_new_with_range(-2147483648, 2147483647, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(paramOverrideValue), GetCurrentConnection()->outParamOverride.intValue);
	g_signal_connect(paramOverrideValue, "value-changed", G_CALLBACK(param_int_value_changed), NULL);
	gtk_box_append(GTK_BOX(paramOverrideBox), paramOverrideValue);
}

void param_float_value_changed(GtkSpinButton *self, gpointer)
{
	GetCurrentConnection()->outParamOverride.type = PARAM_TYPE_FLOAT;
	GetCurrentConnection()->outParamOverride.floatValue = gtk_spin_button_get_value(self);
	IOWindowReloadList();
}

void CreateParamValueFloat()
{
	RemoveExistingParamValueWidget();
	paramOverrideValue = gtk_spin_button_new_with_range(-2147483648, 2147483647, 0.01);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(paramOverrideValue), GetCurrentConnection()->outParamOverride.floatValue);
	g_signal_connect(paramOverrideValue, "value-changed", G_CALLBACK(param_float_value_changed), NULL);
	gtk_box_append(GTK_BOX(paramOverrideBox), paramOverrideValue);
}

void param_bool_toggled(GtkCheckButton *self, gpointer)
{
	GetCurrentConnection()->outParamOverride.type = PARAM_TYPE_BOOL;
	GetCurrentConnection()->outParamOverride.boolValue = gtk_check_button_get_active(self);
	IOWindowReloadList();
}

void CreateParamValueBool()
{
	RemoveExistingParamValueWidget();
	paramOverrideValue = gtk_check_button_new();
	gtk_check_button_set_active(GTK_CHECK_BUTTON(paramOverrideValue),
								GetCurrentConnection()->outParamOverride.boolValue);
	g_signal_connect(paramOverrideValue, "toggled", G_CALLBACK(param_bool_toggled), NULL);
	gtk_box_append(GTK_BOX(paramOverrideBox), paramOverrideValue);
}

void param_string_changed(GtkEditable *self, gpointer)
{
	GetCurrentConnection()->outParamOverride.type = PARAM_TYPE_STRING;
	const char *text = gtk_entry_buffer_get_text(gtk_entry_get_buffer(GTK_ENTRY(self)));
	strncpy(GetCurrentConnection()->outParamOverride.stringValue, text, 64);
	IOWindowReloadList();
}

void CreateParamValueString()
{
	RemoveExistingParamValueWidget();
	paramOverrideValue = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(paramOverrideValue), 64);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(GTK_ENTRY(paramOverrideValue)),
							  GetCurrentConnection()->outParamOverride.stringValue,
							  -1);
	g_signal_connect(paramOverrideValue, "changed", G_CALLBACK(param_string_changed), NULL);
	gtk_box_append(GTK_BOX(paramOverrideBox), paramOverrideValue);
}

void CreateParamValueNone()
{
	RemoveExistingParamValueWidget();
}

void IOWindowReloadBoxes()
{
	updateCount++;
	GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(listBox));
	if (row == NULL)
	{
		gtk_widget_set_sensitive(myOutputCombo, false);
		gtk_widget_set_sensitive(targetActorCombo, false);
		gtk_widget_set_sensitive(targetSignalCombo, false);
		gtk_widget_set_sensitive(paramOverrideValue, false);
		gtk_widget_set_sensitive(paramOverrideTypeCombo, false);
	} else
	{
		gtk_widget_set_sensitive(myOutputCombo, true);
		gtk_widget_set_sensitive(targetActorCombo, true);
		gtk_widget_set_sensitive(targetSignalCombo, true);
		gtk_widget_set_sensitive(paramOverrideValue, false);
		gtk_widget_set_sensitive(paramOverrideTypeCombo, false);

		const ActorDefinition *def = GetActorDef(ioActor->actorType);
		PopulateComboBoxWithSignals(myOutputCombo, def->numOutputs, def->outputs);
		gtk_combo_box_set_active(GTK_COMBO_BOX(myOutputCombo), GetCurrentConnection()->myOutput);

		PopulateComboBoxWithActors(targetActorCombo, GetCurrentConnection()->outActorName);

		const ActorConnection *conn = g_object_get_data(G_OBJECT(row), "connection");
		const Actor *target = NULL;
		for (int j = 0; j < l->actors.length; j++)
		{
			const Actor *a = ListGet(l->actors, j);
			if (a->name[0] != '\0' && strncmp(a->name, conn->outActorName, 64) == 0)
			{
				target = a;
				break;
			}
		}
		if (target == NULL)
		{
			gtk_widget_set_sensitive(targetSignalCombo, false);
			updateCount--;
			return;
		}

		const ActorDefinition *targetDef = GetActorDef(target->actorType);
		PopulateComboBoxWithSignals(targetSignalCombo, targetDef->numInputs, targetDef->inputs);
		gtk_combo_box_set_active(GTK_COMBO_BOX(targetSignalCombo), conn->targetInput);

		gtk_widget_set_sensitive(paramOverrideValue, true);
		gtk_widget_set_sensitive(paramOverrideTypeCombo, true);
		gtk_combo_box_set_active(GTK_COMBO_BOX(paramOverrideTypeCombo), conn->outParamOverride.type);

		switch (conn->outParamOverride.type)
		{
			case PARAM_TYPE_BYTE:
				CreateParamValueByte();
				break;
			case PARAM_TYPE_INTEGER:
				CreateParamValueInteger();
				break;
			case PARAM_TYPE_FLOAT:
				CreateParamValueFloat();
				break;
			case PARAM_TYPE_BOOL:
				CreateParamValueBool();
				break;
			case PARAM_TYPE_STRING:
				CreateParamValueString();
				break;
			case PARAM_TYPE_NONE:
			default:
				CreateParamValueNone();
				break;
		}
	}
	updateCount--;
}

#pragma endregion

#pragma region Callbacks

/**
 * Callback for when the OK button is pressed
 */
void io_ok_clicked(GtkButton *, gpointer)
{
	gtk_window_close(ioWindow);
}

void io_add_clicked(GtkButton *, gpointer)
{
	ActorConnection *conn = malloc(sizeof(ActorConnection));
	memset(conn, 0, sizeof(ActorConnection));
	conn->myOutput = 0;
	conn->outActorName[0] = '\0';
	conn->targetInput = 0;
	conn->outParamOverride.type = PARAM_TYPE_NONE;
	ListAdd(&ioActor->ioConnections, conn);
	IOWindowReloadList();
}

void io_delete_clicked(GtkButton *, gpointer)
{
	ActorConnection *conn = GetCurrentConnection();
	if (conn == NULL)
	{
		return;
	}
	ListRemoveAt(&ioActor->ioConnections, ListFind(ioActor->ioConnections, conn));
	free(conn);
	IOWindowReloadList();
	IOWindowReloadBoxes();
}

void io_row_selected(GtkListBox *, GtkListBoxRow *, gpointer)
{
	if (updateCount != 0)
	{
		return;
	}
	IOWindowReloadBoxes();
	IOWindowReloadList();
}

void my_output_changed(GtkComboBox *self, gpointer)
{
	if (updateCount != 0)
	{
		return;
	}
	GetCurrentConnection()->myOutput = gtk_combo_box_get_active(GTK_COMBO_BOX(self));
	IOWindowReloadBoxes();
	IOWindowReloadList();
}

void target_actor_changed(GtkComboBox *self, gpointer)
{
	if (updateCount != 0)
	{
		return;
	}
	strncpy(GetCurrentConnection()->outActorName, gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(self)), 64);
	IOWindowReloadBoxes();
	IOWindowReloadList();
}

void target_input_changed(GtkComboBox *self, gpointer)
{
	if (updateCount != 0)
	{
		return;
	}
	GetCurrentConnection()->targetInput = gtk_combo_box_get_active(GTK_COMBO_BOX(self));
	IOWindowReloadBoxes();
	IOWindowReloadList();
}

void param_type_changed(GtkComboBox *self, gpointer)
{
	if (updateCount != 0)
	{
		return;
	}

	GetCurrentConnection()->outParamOverride.type = gtk_combo_box_get_active(GTK_COMBO_BOX(self));
	// zero out the value to avoid reading data of previous type as if it were the new type
	memset(&GetCurrentConnection()->outParamOverride, 0, sizeof(GetCurrentConnection()->outParamOverride));

	IOWindowReloadBoxes();
	IOWindowReloadList();
}

#pragma endregion

void IOWindowShow(GtkWindow *parent, GtkApplication *app, Actor *actor)
{
	ioActor = actor;
	GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "I/O Connections");
	gtk_window_set_transient_for(GTK_WINDOW(window), parent);
	gtk_window_set_modal(GTK_WINDOW(window), TRUE);
	// gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_window_set_default_size(GTK_WINDOW(window), 600, -1);

	GtkWidget *mainStack = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	gtk_widget_set_margin_start(mainStack, 4);
	gtk_widget_set_margin_end(mainStack, 4);
	gtk_widget_set_margin_top(mainStack, 4);
	gtk_widget_set_margin_bottom(mainStack, 4);

	GtkWidget *headerLabel = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(headerLabel), "<big><b>I/O Connections</b></big>");
	gtk_label_set_xalign(GTK_LABEL(headerLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), headerLabel);

	GtkWidget *headerBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(headerBox, TRUE);

	GtkWidget *myOutputHeaderLabel = gtk_label_new("Output");
	gtk_label_set_xalign(GTK_LABEL(myOutputHeaderLabel), 0);
	gtk_widget_set_size_request(myOutputHeaderLabel, 150, -1);
	gtk_box_append(GTK_BOX(headerBox), myOutputHeaderLabel);

	GtkWidget *targetActorHeaderLabel = gtk_label_new("Target Actor");
	gtk_label_set_xalign(GTK_LABEL(targetActorHeaderLabel), 0);
	gtk_widget_set_size_request(targetActorHeaderLabel, 150, -1);
	gtk_box_append(GTK_BOX(headerBox), targetActorHeaderLabel);

	GtkWidget *targetInputHeaderLabel = gtk_label_new("Target Input");
	gtk_label_set_xalign(GTK_LABEL(targetInputHeaderLabel), 0);
	gtk_widget_set_size_request(targetInputHeaderLabel, 150, -1);
	gtk_box_append(GTK_BOX(headerBox), targetInputHeaderLabel);

	GtkWidget *paramOverrideHeaderLabel = gtk_label_new("Param Override");
	gtk_label_set_xalign(GTK_LABEL(paramOverrideHeaderLabel), 0);
	gtk_widget_set_size_request(paramOverrideHeaderLabel, 150, -1);
	gtk_box_append(GTK_BOX(headerBox), paramOverrideHeaderLabel);

	gtk_box_append(GTK_BOX(mainStack), headerBox);

	GtkWidget *scrollBox = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollBox), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrollBox, -1, 200);
	gtk_widget_set_vexpand(scrollBox, TRUE);
	gtk_widget_set_hexpand(scrollBox, TRUE);
	gtk_box_append(GTK_BOX(mainStack), scrollBox);

	listBox = gtk_list_box_new();
	gtk_widget_set_size_request(listBox, -1, 200);
	gtk_widget_set_vexpand(listBox, TRUE);
	gtk_widget_set_hexpand(listBox, TRUE);
	gtk_list_box_set_selection_mode(GTK_LIST_BOX(listBox), GTK_SELECTION_SINGLE);
	g_signal_connect(listBox, "row-selected", G_CALLBACK(io_row_selected), NULL);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollBox), listBox);

	IOWindowReloadList();

	GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep);

	GtkWidget *inSignalHBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	GtkWidget *inSignalLabel = gtk_label_new("Output");
	gtk_label_set_xalign(GTK_LABEL(inSignalLabel), 0);
	gtk_widget_set_size_request(inSignalLabel, 100, -1);
	gtk_box_append(GTK_BOX(inSignalHBox), inSignalLabel);
	myOutputCombo = gtk_combo_box_text_new();
	g_signal_connect(G_OBJECT(myOutputCombo), "changed", G_CALLBACK(my_output_changed), NULL);
	gtk_widget_set_hexpand(myOutputCombo, TRUE);
	gtk_box_append(GTK_BOX(inSignalHBox), myOutputCombo);
	gtk_box_append(GTK_BOX(mainStack), inSignalHBox);

	GtkWidget *targetActorHBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	GtkWidget *targetActorLabel = gtk_label_new("Target Actor");
	gtk_label_set_xalign(GTK_LABEL(targetActorLabel), 0);
	gtk_widget_set_size_request(targetActorLabel, 100, -1);
	gtk_box_append(GTK_BOX(targetActorHBox), targetActorLabel);
	targetActorCombo = gtk_combo_box_text_new();
	g_signal_connect(G_OBJECT(targetActorCombo), "changed", G_CALLBACK(target_actor_changed), NULL);
	gtk_widget_set_hexpand(targetActorCombo, TRUE);
	gtk_box_append(GTK_BOX(targetActorHBox), targetActorCombo);
	gtk_box_append(GTK_BOX(mainStack), targetActorHBox);

	GtkWidget *outSignalHBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	GtkWidget *outSignalLabel = gtk_label_new("Target Input");
	gtk_label_set_xalign(GTK_LABEL(outSignalLabel), 0);
	gtk_widget_set_size_request(outSignalLabel, 100, -1);
	gtk_box_append(GTK_BOX(outSignalHBox), outSignalLabel);
	targetSignalCombo = gtk_combo_box_text_new();
	g_signal_connect(G_OBJECT(targetSignalCombo), "changed", G_CALLBACK(target_input_changed), NULL);
	gtk_widget_set_hexpand(targetSignalCombo, TRUE);
	gtk_box_append(GTK_BOX(outSignalHBox), targetSignalCombo);
	gtk_box_append(GTK_BOX(mainStack), outSignalHBox);

	paramOverrideBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	GtkWidget *paramOverrideLabel = gtk_label_new("Param Override");
	gtk_label_set_xalign(GTK_LABEL(paramOverrideLabel), 0);
	gtk_widget_set_size_request(paramOverrideLabel, 100, -1);
	gtk_box_append(GTK_BOX(paramOverrideBox), paramOverrideLabel);
	paramOverrideTypeCombo = gtk_combo_box_text_new();
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(paramOverrideTypeCombo), "Byte");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(paramOverrideTypeCombo), "Integer");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(paramOverrideTypeCombo), "Float");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(paramOverrideTypeCombo), "Bool");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(paramOverrideTypeCombo), "String");
	gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(paramOverrideTypeCombo), "None");
	gtk_combo_box_set_active(GTK_COMBO_BOX(paramOverrideTypeCombo), 0);
	gtk_widget_set_size_request(paramOverrideTypeCombo, 100, -1);
	g_signal_connect(G_OBJECT(paramOverrideTypeCombo), "changed", G_CALLBACK(param_type_changed), NULL);
	gtk_box_append(GTK_BOX(paramOverrideBox), paramOverrideTypeCombo);
	gtk_box_append(GTK_BOX(mainStack), paramOverrideBox);

	IOWindowReloadBoxes();

	GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep2);

	GtkWidget *okCancelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(okCancelBox, TRUE);
	gtk_widget_set_halign(okCancelBox, GTK_ALIGN_END);

	GtkWidget *okButton = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okButton, 80, -1);
	g_signal_connect(okButton, "clicked", G_CALLBACK(io_ok_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), okButton);

	GtkWidget *addDeleteBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(addDeleteBox, TRUE);
	gtk_widget_set_halign(addDeleteBox, GTK_ALIGN_START);

	GtkWidget *addButton = gtk_button_new_with_label("Add");
	gtk_widget_set_size_request(addButton, 80, -1);
	g_signal_connect(addButton, "clicked", G_CALLBACK(io_add_clicked), NULL);
	gtk_box_append(GTK_BOX(addDeleteBox), addButton);

	GtkWidget *deleteButton = gtk_button_new_with_label("Delete");
	gtk_widget_set_size_request(deleteButton, 80, -1);
	g_signal_connect(deleteButton, "clicked", G_CALLBACK(io_delete_clicked), NULL);
	gtk_box_append(GTK_BOX(addDeleteBox), deleteButton);

	GtkWidget *buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_append(GTK_BOX(buttonBox), addDeleteBox);
	gtk_box_append(GTK_BOX(buttonBox), okCancelBox);
	gtk_box_append(GTK_BOX(mainStack), buttonBox);
	gtk_widget_set_hexpand(buttonBox, TRUE);

	gtk_window_set_child(GTK_WINDOW(window), mainStack);

	ioWindow = GTK_WINDOW(window);
	ioApplication = app;

	gtk_window_present(GTK_WINDOW(window));
}
