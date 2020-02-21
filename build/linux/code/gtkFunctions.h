//
// Created by Unknow on 16/02/2020.
//

#ifndef CODECC_GTKFUNCTIONS_H
#define CODECC_GTKFUNCTIONS_H

#include <gtk-3.0/gtk/gtk.h>
#include "codecFunctions.h"

GtkBuilder *builder;

typedef struct {
    GtkWidget *window;
    GtkButton *encodeButton;
    GtkButton *decodeButton;
    GtkLevelBar *levelBar;
    GtkSpinner *spinner;
    GtkFileChooserButton *fileChooserButton;
    GtkFileChooserButton *keyChooserButton;
    GtkLabel *statusLabel;
} App_widgets;


App_widgets *widgets;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

G_MODULE_EXPORT void on_keyChooserButton_file_set();

G_MODULE_EXPORT void on_fileChooserButton_file_set();

G_MODULE_EXPORT void on_encodeButton_clicked();

G_MODULE_EXPORT void on_decodeButton_clicked();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void startGTK(int *argc, char ***argv, char *gladeFile);

void connectWidgets();

void onDestroy();

void setSpinnerStatus(gboolean status);

void levelBarSetValue(double value);

void updateStatus(const char *message);

#endif //CODECC_GTKFUNCTIONS_H
