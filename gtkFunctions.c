//
// Created by Unknow on 16/02/2020.
//

#include "gtkFunctions.h"
#include "codecFunctions.h"

extern bool codecKeyLoaded;
char *filePath = NULL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void on_keyChooserButton_file_set() {
    char *path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widgets->keyChooserButton));
    updateStatus("Building encode/decode matrix...");
    levelBarSetValue(0.);
    setSpinnerStatus(TRUE);
    codecKeyLoaded = !readKey(path);
    setSpinnerStatus(FALSE);
    codecKeyLoaded ? updateStatus("Key loaded, encode/decode matrix loaded") : NULL;

}

void on_fileChooserButton_file_set() {
    filePath = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widgets->fileChooserButton));
}

void on_encodeButton_clicked() {
    updateStatus("Encoding...");
    levelBarSetValue(0.);
    setSpinnerStatus(TRUE);
    encode();
    setSpinnerStatus(FALSE);
}

void on_decodeButton_clicked() {
    updateStatus("Decoding...");
    levelBarSetValue(0.);
    setSpinnerStatus(TRUE);
    decode();
    setSpinnerStatus(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void startGTK(int *argc, char ***argv, char *gladeFile) {
    gtk_init(argc, argv);
    builder = gtk_builder_new_from_file(gladeFile);
    connectWidgets();

    g_signal_connect(widgets->window, "destroy", G_CALLBACK(onDestroy), NULL);
    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(widgets->window);

    gtk_main();

    g_slice_free(App_widgets, widgets);
}

void connectWidgets() {
    widgets = g_slice_new(App_widgets);

    widgets->window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
    widgets->encodeButton = GTK_BUTTON(gtk_builder_get_object(builder, "encodeButton"));
    widgets->decodeButton = GTK_BUTTON(gtk_builder_get_object(builder, "decodeButton"));
    widgets->levelBar = GTK_LEVEL_BAR(gtk_builder_get_object(builder, "levelBar"));
    widgets->spinner = GTK_SPINNER(gtk_builder_get_object(builder, "spinner"));
    widgets->fileChooserButton = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "fileChooserButton"));
    widgets->keyChooserButton = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "keyChooserButton"));
    widgets->statusLabel = GTK_LABEL(gtk_builder_get_object(builder, "statusLabel"));
}

void onDestroy() {
    gtk_main_quit();
}

void setSpinnerStatus(gboolean status) {
    status ?
    gtk_spinner_start(widgets->spinner)
           :
    gtk_spinner_stop(widgets->spinner);
}

void levelBarSetValue(double value) {
    gtk_level_bar_set_value(widgets->levelBar, value);
}

void updateStatus(const char *message) {
    gtk_label_set_text(widgets->statusLabel, message);
}

void GTKEncode() {

}