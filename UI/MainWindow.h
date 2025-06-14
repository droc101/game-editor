//
// Created by droc101 on 1/13/25.
//

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <gtk/gtk.h>
#include <adwaita.h>

void UpdateActorSidebar();

/**
 * Show the main window
 * @param app The GTK Application
 */
void MainWindowActivate(AdwApplication *app, gpointer *);

/**
 * Inform the main window that the selection type has changed, causing it to recreate the left sidebar
 */
void SelectionTypeChanged();

#endif //MAINWINDOW_H
