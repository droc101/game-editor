//
// Created by droc101 on 1/16/25.
//

#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <gtk/gtk.h>

/**
 * Whether the options window is open
 */
extern bool optionsWindowOpen;

/**
 * Show the options window
 * @param parent The parent window (@c NULL is OK)
 * @param app The GTK application
 * @param required Whether this is required (turns cancel into quit)
 */
void OptionsWindowShow(GtkWindow *parent, GtkApplication *app, bool required);

#endif //OPTIONSWINDOW_H
