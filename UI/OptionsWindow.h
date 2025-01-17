//
// Created by droc101 on 1/16/25.
//

#ifndef OPTIONSWINDOW_H
#define OPTIONSWINDOW_H

#include <gtk/gtk.h>

extern bool optionsWindowOpen;

void OptionsWindowShow(GtkWindow *parent, GtkApplication *app, bool required);

#endif //OPTIONSWINDOW_H
