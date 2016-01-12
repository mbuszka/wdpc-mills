#include <gtk/gtk.h>
#include "mills.h"

void init_app();
void switch_to_main_menu(GtkWidget *btn, gpointer data);
void signal_error(char* msg);
void draw_game_state(GameState state);
