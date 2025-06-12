//
// Created by droc101 on 5/29/25.
//

#ifndef UIHELPERS_H
#define UIHELPERS_H

#include <gtk/gtk.h>
#include <adwaita.h>
#include "../Helpers/GameInterface.h"

extern AdwApplication *application;

void PopulateComboBoxTextures(GtkWidget *box);
void PopulateComboBoxActors(GtkWidget *box);
void PopulateComboBoxMusic(GtkWidget *box);
void PopulateComboBoxWithActorNames(GtkWidget *box, const char *selected);
void PopulateComboBoxWithSignals(GtkWidget *box, size_t count, const ActorDefSignal *signals);
void PopulateComboBoxModels(GtkWidget *box);

void PopulateComboBoxSounds(GtkWidget *box);

void PopulateComboBoxLevels(GtkWidget *box);

GtkWidget *CreateTextureComboBox(const char *selected, GtkWidget *comboBox, GtkWindow *window);

#endif //UIHELPERS_H
