//
// Created by droc101 on 5/31/25.
//

#include "BenchmarkWindow.h"

#include "../Editor.h"
#include "UiHelpers.h"

GtkWindow *bmWindow;
GtkApplication *bmApplication;

size_t bmWallCount;
size_t bmActorCount;
int bmActorType;
float bmHalfSize;

void bm_ok_clicked(GtkButton *, gpointer)
{
	GenerateBenchmarkLevel(bmWallCount, bmActorCount, bmActorType, bmHalfSize);
	gtk_window_close(bmWindow);
}

void bm_cancel_clicked(GtkButton *, gpointer)
{
	gtk_window_close(bmWindow);
}

void
bm_walls_value_changed (
  GtkSpinButton* self,
  gpointer
)
{
	bmWallCount = gtk_spin_button_get_value_as_int(self);
}

void
bm_actors_value_changed (
  GtkSpinButton* self,
  gpointer
)
{
	bmActorCount = gtk_spin_button_get_value_as_int(self);
}

void
bm_half_size_value_changed (
  GtkSpinButton* self,
  gpointer
)
{
	bmHalfSize = gtk_spin_button_get_value(self);
}


void bm_actor_type_changed(GtkComboBox *self, gpointer)
{
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
	bmActorType = type;
}

void BMWindowShow(GtkWindow *parent, GtkApplication *app)
{
	bmWallCount = 2000;
	bmActorCount = 0;
	bmActorType = GetActorLoadIndexByName("Test Actor");
	bmHalfSize = 10.0f;
	GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
	gtk_window_set_title(GTK_WINDOW(window), "Generate Benchmark");
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
	gtk_label_set_markup(GTK_LABEL(headerLabel), "<big><b>Generate Benchmark</b></big>");
	gtk_label_set_xalign(GTK_LABEL(headerLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), headerLabel);

	GtkWidget *wallCountLabel = gtk_label_new("Wall Count");
	gtk_label_set_xalign(GTK_LABEL(wallCountLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), wallCountLabel);
	GtkWidget *wallCountBox = gtk_spin_button_new_with_range(0, 10000, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(wallCountBox), bmWallCount);
	g_signal_connect(wallCountBox, "value-changed", G_CALLBACK(bm_walls_value_changed), NULL);
	gtk_box_append(GTK_BOX(mainStack), wallCountBox);

	GtkWidget *actorCountLabel = gtk_label_new("Actor Count");
	gtk_label_set_xalign(GTK_LABEL(actorCountLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), actorCountLabel);
	GtkWidget *actorCountBox = gtk_spin_button_new_with_range(0, 10000, 1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(actorCountBox), bmActorCount);
	g_signal_connect(actorCountBox, "value-changed", G_CALLBACK(bm_actors_value_changed), NULL);
	gtk_box_append(GTK_BOX(mainStack), actorCountBox);

	GtkWidget *actorTypeLabel = gtk_label_new("Actor Type");
	gtk_label_set_xalign(GTK_LABEL(actorTypeLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), actorTypeLabel);
	GtkWidget *actorTypeCombo = gtk_combo_box_text_new();
	PopulateComboBoxActors(actorTypeCombo);
	gtk_combo_box_set_active(GTK_COMBO_BOX(actorTypeCombo), bmActorType);
	g_signal_connect(actorTypeCombo, "changed", G_CALLBACK(bm_actor_type_changed), NULL);
	gtk_widget_set_size_request(actorTypeCombo, 200, -1);
	gtk_box_append(GTK_BOX(mainStack), actorTypeCombo);

	GtkWidget *halfSizeLabel = gtk_label_new("Half Size");
	gtk_label_set_xalign(GTK_LABEL(halfSizeLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), halfSizeLabel);
	GtkWidget *halfSizeBox = gtk_spin_button_new_with_range(0.1, 100, 0.1);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(halfSizeBox), bmHalfSize);
	g_signal_connect(halfSizeBox, "changed", G_CALLBACK(bm_half_size_value_changed), NULL);
	gtk_box_append(GTK_BOX(mainStack), halfSizeBox);

	GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep2);

	GtkWidget *okCancelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(okCancelBox, TRUE);
	gtk_widget_set_halign(okCancelBox, GTK_ALIGN_END);

	GtkWidget *okButton = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okButton, 80, -1);
	g_signal_connect(okButton, "clicked", G_CALLBACK(bm_ok_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), okButton);

	GtkWidget *cancelButton = gtk_button_new_with_label("Cancel");
	gtk_widget_set_size_request(cancelButton, 80, -1);
	g_signal_connect(cancelButton, "clicked", G_CALLBACK(bm_cancel_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), cancelButton);

	GtkWidget *buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_append(GTK_BOX(buttonBox), okCancelBox);
	gtk_box_append(GTK_BOX(mainStack), buttonBox);
	gtk_widget_set_hexpand(buttonBox, TRUE);

	gtk_window_set_child(GTK_WINDOW(window), mainStack);

	bmWindow = GTK_WINDOW(window);
	bmApplication = app;

	gtk_window_present(GTK_WINDOW(window));
}
