//
// Created by droc101 on 6/2/25.
//

#include "TextureBrowserWindow.h"

#include <string.h>
#include "../Editor.h"
#include "../Helpers/TextureReader.h"
#include "UiHelpers.h"

GtkWindow *tbWindow;
GtkWidget *tbFilter;
GtkWidget *tbFlowBox;
GtkWidget *tbComboBox;

List *images = NULL;

void tb_ok_clicked(GtkButton *, gpointer)
{
	GList *selectedChildren = gtk_flow_box_get_selected_children(GTK_FLOW_BOX(tbFlowBox));
	if (selectedChildren == NULL || g_list_length(selectedChildren) == 0)
	{
		return;
	}
	GtkFlowBoxChild *selectedFbChild = GTK_FLOW_BOX_CHILD(selectedChildren->data);
	GtkWidget *selectedChild = gtk_flow_box_child_get_child(selectedFbChild);
	const Image *img = (Image *)g_object_get_data(G_OBJECT(selectedChild), "image");
	if (img == NULL)
	{
		printf("No image data found for selected child\n");
		return;
	}
	GtkEntry *entry = GTK_ENTRY(gtk_combo_box_get_child(GTK_COMBO_BOX(tbComboBox)));
	gtk_entry_set_max_length(entry, 60);
	gtk_entry_buffer_set_text(gtk_entry_get_buffer(entry), img->path, -1);

	tbComboBox = NULL;
	tbFlowBox = NULL;
	tbFilter = NULL;

	gtk_window_close(tbWindow);
}

void tb_cancel_clicked(GtkButton *, gpointer)
{
	gtk_window_close(tbWindow);
}

GdkPixbuf *CreatePixbuf(Image *img)
{
	GdkPixbuf *pixbuf = NULL;
	if (img->pixelData == NULL)
	{
		pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, img->width, img->height);
		gdk_pixbuf_fill(pixbuf, 0x00000000); // Fill with transparent
	} else
	{
		pixbuf = gdk_pixbuf_new_from_data(img->pixelData,
										  GDK_COLORSPACE_RGB,
										  TRUE,
										  8,
										  img->width,
										  img->height,
										  img->width * 4,
										  NULL,
										  NULL);
	}
	if (pixbuf == NULL)
	{
		printf("Failed to create pixbuf for image %s\n", img->path);
		return NULL;
	}
	gdk_pixbuf_set_option(pixbuf, "gdk-pixbuf:compression", "0");
	gdk_pixbuf_set_option(pixbuf, "gdk-pixbuf:quality", "100");

	return pixbuf;
}

gboolean filterfunc(GtkFlowBoxChild *fb_child, gpointer)
{
	GtkWidget *child = gtk_flow_box_child_get_child(fb_child);
	const Image *img = (Image *)g_object_get_data(G_OBJECT(child), "image");
	GtkEntryBuffer *buf = gtk_entry_get_buffer(GTK_ENTRY(tbFilter));
	const char *filterText = gtk_entry_buffer_get_text(buf);
	if (filterText == NULL || filterText[0] == '\0')
	{
		return TRUE; // No filter text, show all
	}
	const char *imgPath = img->path;
	return g_strrstr(imgPath, filterText) != NULL;
}

void filter_changed(GtkEditable *, gpointer)
{
	gtk_flow_box_invalidate_filter(GTK_FLOW_BOX(tbFlowBox));
}

void flowbox_child_activated(GtkFlowBox *, GtkFlowBoxChild *, gpointer)
{
	tb_ok_clicked(NULL, NULL);
}


void TextureBrowserShow(GtkWindow *parent, GtkWidget *comboBox)
{
	tbComboBox = comboBox;

	if (images == NULL)
	{
		images = malloc(sizeof(List));
		ListCreate(images);
		for (int i = 0; i < textureList->length; i++)
		{
			const char *tex = ListGet(*textureList, i);
			Image *img = LoadImage(tex);
			if (img != NULL)
			{
				ListAdd(images, img);
			}
		}
	}
	GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(application));
	gtk_window_set_title(GTK_WINDOW(window), "Texture Browser");
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
	gtk_label_set_markup(GTK_LABEL(headerLabel), "<big><b>Texture Browser</b></big>");
	gtk_label_set_xalign(GTK_LABEL(headerLabel), 0);
	gtk_box_append(GTK_BOX(mainStack), headerLabel);

	GtkWidget *flowBox = gtk_flow_box_new();
	gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(flowBox), GTK_SELECTION_SINGLE);
	gtk_flow_box_set_filter_func(GTK_FLOW_BOX(flowBox), filterfunc, NULL, NULL);
	gtk_flow_box_set_activate_on_single_click(GTK_FLOW_BOX(flowBox), FALSE);

	for (int i = 0; i < images->length; i++)
	{
		Image *img = ListGet(*images, i);
		GdkPixbuf *pixbuf = CreatePixbuf(img);
		if (pixbuf == NULL)
		{
			continue;
		}

		GtkWidget *label = gtk_label_new("");
		gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
		gtk_label_set_text(GTK_LABEL(label), img->path);

		GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);
		gtk_widget_set_size_request(image, 96, 96);

		GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
		gtk_box_append(GTK_BOX(box), image);
		gtk_box_append(GTK_BOX(box), label);

		g_object_set_data(G_OBJECT(box), "image", img);

		gtk_flow_box_insert(GTK_FLOW_BOX(flowBox), box, -1);

		g_object_unref(pixbuf);
	}

	gtk_flow_box_select_child(GTK_FLOW_BOX(flowBox), gtk_flow_box_get_child_at_index(GTK_FLOW_BOX(flowBox), 0));
	g_signal_connect(flowBox, "child-activated", G_CALLBACK(flowbox_child_activated), NULL);

	tbFlowBox = flowBox;

	GtkWidget *scrollBox = gtk_scrolled_window_new();
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollBox), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_set_size_request(scrollBox, -1, 400);
	gtk_widget_set_vexpand(scrollBox, TRUE);
	gtk_widget_set_hexpand(scrollBox, TRUE);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrollBox), flowBox);
	gtk_box_append(GTK_BOX(mainStack), scrollBox);

	GtkWidget *sep2 = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
	gtk_box_append(GTK_BOX(mainStack), sep2);

	GtkWidget *okCancelBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_widget_set_hexpand(okCancelBox, TRUE);
	gtk_widget_set_halign(okCancelBox, GTK_ALIGN_END);

	GtkWidget *searchEntry = gtk_entry_new();
	gtk_entry_set_placeholder_text(GTK_ENTRY(searchEntry), "Search Textures...");
	g_signal_connect(searchEntry, "changed", G_CALLBACK(filter_changed), flowBox);
	gtk_box_append(GTK_BOX(okCancelBox), searchEntry);
	tbFilter = searchEntry;

	GtkWidget *spacer = gtk_label_new("");
	gtk_widget_set_hexpand(spacer, TRUE);
	gtk_box_append(GTK_BOX(okCancelBox), spacer);

	GtkWidget *okButton = gtk_button_new_with_label("OK");
	gtk_widget_set_size_request(okButton, 80, -1);
	g_signal_connect(okButton, "clicked", G_CALLBACK(tb_ok_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), okButton);

	GtkWidget *cancelButton = gtk_button_new_with_label("Cancel");
	gtk_widget_set_size_request(cancelButton, 80, -1);
	g_signal_connect(cancelButton, "clicked", G_CALLBACK(tb_cancel_clicked), NULL);
	gtk_box_append(GTK_BOX(okCancelBox), cancelButton);

	GtkWidget *buttonBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	gtk_box_append(GTK_BOX(buttonBox), okCancelBox);
	gtk_box_append(GTK_BOX(mainStack), buttonBox);
	gtk_widget_set_hexpand(buttonBox, TRUE);

	gtk_window_set_child(GTK_WINDOW(window), mainStack);

	tbWindow = GTK_WINDOW(window);

	gtk_window_present(GTK_WINDOW(window));
}
