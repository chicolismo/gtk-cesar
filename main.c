#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "common.h"
#include "simulator.h"

// sudo apt-get install libgtk-3-dev
// gcc -o main main.c -Wall `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

// Globais
byte *memory;
GtkWidget *window;
GtkWidget *left_table;
GtkWidget *right_table;
GtkWidget *tables[2];
CPU *cpu;

char current_file_path[256] = { 0 }; // TODO: Qual o maior nome de arquivo?

enum {
    LIST_ITEM = 0,
    N_COLUMNS
};

// Protótipos
void close_program();
void show_open_file_dialog();
void save_file();
void read_file_to_memory(char *filename, byte *memory);
long get_file_size(FILE *file);
void table_set_value(int row, byte value);
void table_fill(CPU *cpu);
void initialize_components(GtkBuilder *builder);


void close_program() {
    // TODO: Se a memória for alterada, devemos pergutar se o usuário quer salvar.
    gtk_main_quit();
}


void show_open_file_dialog() {
    GtkWidget *open_file_dialog = gtk_file_chooser_dialog_new(
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

    if (gtk_dialog_run(GTK_DIALOG(open_file_dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(open_file_dialog));
        read_file_to_memory(filename, memory);
        strcpy(current_file_path, filename);
        g_free(filename);
    }

    gtk_widget_destroy(open_file_dialog);
}


void save_file() {

}


long get_file_size(FILE *file) {
    fseek(file, 0, SEEK_END);
    long nbytes = ftell(file);
    rewind(file);
    return nbytes;
}


void read_file_to_memory(char *filename, byte *memory) {
    FILE *file = NULL;
    file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Erro ao abrir o arquivo.");
        exit(EXIT_FAILURE);
    }
    long size = get_file_size(file);

    byte *temp_memory = malloc(sizeof(byte) * size);
    fread(temp_memory, size, 1, file);
    fclose(file);

    cpu_set_memory(cpu, temp_memory, size);

    free(temp_memory);
    memory = cpu->memory;

    table_fill(cpu);
}


void table_fill(CPU *cpu) {
    for (int i = 0; i < MEM_SIZE; i += 1) {
        table_set_value(i, cpu_get_byte(cpu, (word) i));
    }
}


void table_set_value(int row, byte value) {
    GtkTreeView *table;

    for (int i = 0; i < 2; ++i) {
        table = GTK_TREE_VIEW(tables[i]);
        GtkTreeIter iter;
        GtkTreeModel *store = gtk_tree_view_get_model(table);
        GtkTreePath *path = gtk_tree_path_new_from_indices(row, -1);
        gtk_tree_model_get_iter(store, &iter, path);
        char string[10];
        sprintf(string, "%d (%02x)", value, value);
        gtk_list_store_set(GTK_LIST_STORE(store), &iter, 0, row, 1, string, -1);
        gtk_tree_path_free(path);
    }
}


void initialize_components(GtkBuilder *builder) {
    cpu = cpu_new();

    gtk_builder_connect_signals(builder, NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(window, "destroy", G_CALLBACK(close_program), NULL);
    /*cairo_font_options_t options = cairo_font_options_create();*/
    /*cairo_font_options_set*/
    /*gtk_widget_set_fonts_options(window, options);*/

    //--------------------------------------------------------
    // Inicializando as tabelas
    //--------------------------------------------------------
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;
    GtkTreeView *table;

    left_table = GTK_WIDGET(gtk_builder_get_object(builder, "left_table"));
    right_table = GTK_WIDGET(gtk_builder_get_object(builder, "right_table"));
    tables[0] = left_table;
    tables[1] = right_table;

    //--------------------------------------------------------
    // Tabela da esquerda
    //--------------------------------------------------------
    table = GTK_TREE_VIEW(left_table);
    renderer = gtk_cell_renderer_text_new();
    // TODO: Acrescentar outra coluna na tabela da esquerda
    column = gtk_tree_view_column_new_with_attributes("Endereço", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(table, column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Valor", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(table, column);
    store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    gtk_tree_view_set_model(table, GTK_TREE_MODEL(store));
    GtkTreeIter iter;
    for (int i = 0; i < MEM_SIZE; ++i) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, "0", -1);
    }
    g_object_unref(store);

    //--------------------------------------------------------
    // Tabela da direita
    //--------------------------------------------------------
    table = GTK_TREE_VIEW(right_table);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Endereço", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(table, column);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Valor", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(table, column);
    store = gtk_list_store_new(2, G_TYPE_INT, G_TYPE_STRING);
    gtk_tree_view_set_model(table, GTK_TREE_MODEL(store));
    for (int i = 0; i < MEM_SIZE; ++i) {
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, i, 1, "0", -1);
    }
    g_object_unref(store);
    // Fazer scroll para a posição 1024
    GtkTreePath *path = gtk_tree_path_new_from_indices(1024, -1);
    gtk_tree_view_scroll_to_cell(table, path, column, FALSE, 0, 0);
    gtk_tree_path_free(path);

    //--------------------------------------------------------
    // Inicializando os itens do menu
    //--------------------------------------------------------
    GtkWidget *menu_item;

    // Abrir
    menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "menu_item_open"));
    g_signal_connect(menu_item, "activate", G_CALLBACK(show_open_file_dialog), NULL);

    // Salvar
    menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "menu_item_save"));
    g_signal_connect(menu_item, "activate", G_CALLBACK(save_file), NULL);

    // Sair
    menu_item = GTK_WIDGET(gtk_builder_get_object(builder, "menu_item_quit"));
    g_signal_connect(menu_item, "activate", G_CALLBACK(close_program), NULL);
}


// main() {{{
int main(int argc, char **argv) {
    memory = malloc(sizeof(byte) * (1 << 16));

    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "cesar.glade", NULL);

    initialize_components(builder);

    GdkScreen *screen = gdk_screen_get_default();
    GtkSettings *settings = gtk_settings_get_for_screen(screen);
    g_object_set(settings, "gtk-theme-name", "Daloa", NULL);

    g_object_unref(builder);
    gtk_widget_show(window);
    gtk_main();

    return EXIT_SUCCESS;
}
// }}}
