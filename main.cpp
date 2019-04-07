
//#include <stdlib.h>
//#include <stdio.h>
#include <gtk/gtk.h>
#include <iostream>
#include <vector>

// sudo apt-get install libgtk-3-dev
// gcc -o main main.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic


//typedef unsigned int byte;
using byte = unsigned int;

#define MEM_SIZE 1<<16

std::vector<byte> memory;

GtkWidget *window;
GtkWidget *open_button;
GtkWidget *open_file_dialog;
GtkFileFilter *open_file_filter;

void close_program(GtkWidget *widget, GdkEvent *event, gpointer data) {
    gtk_main_quit();
}

void show_open_file_dialog() {
    // Somente arquivos come extensão .mem serão selecionados
    if (gtk_dialog_run(GTK_DIALOG(open_file_dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(open_file_dialog));
        printf("%s", filename);
        g_free(filename);
    }
}


void read_file_to_memory(char *filename, std::vector<byte> &mem) {
    FILE *file = NULL;
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo.");
        exit(1);
    }
    mem.clear();
}


void initialize_components(GtkBuilder *builder) {
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    gtk_builder_connect_signals(builder, NULL);

    // Botão de abrir
    open_button = GTK_WIDGET(gtk_builder_get_object(builder, "open_button"));

    // Diálogo de abertura de arquivo
    open_file_dialog = gtk_file_chooser_dialog_new(
                           "Abrir arquivo",
                           GTK_WINDOW(window),
                           GTK_FILE_CHOOSER_ACTION_OPEN,
                           "Cancelar", GTK_RESPONSE_CANCEL,
                           "Abrir", GTK_RESPONSE_ACCEPT,
                           NULL
                       );

    // Apenas arquivos .mem serão abertos, portanto um filtro deve ser
    // acrescentado ao diálogo de abertura.
    GtkFileFilter *open_file_filter = gtk_file_filter_new();
    gtk_file_filter_add_pattern(open_file_filter, "*.mem");
    gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(open_file_dialog), open_file_filter);
}


int main(int argc, char **argv) {
    //memory = malloc(sizeof(byte) * (1 << 16));

    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "cesar.glade", NULL);

    initialize_components(builder);

    g_object_unref(builder);
    gtk_widget_show(window);
    gtk_main();

    gtk_widget_destroy(open_file_dialog);
    return 0;
}
