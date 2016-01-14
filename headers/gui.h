#include <gtk/gtk.h>
#include "mills.h"
#define PLAYER_WHITE_NAME "Red"
#define PLAYER_BLACK_NAME "Blue"

void init_app();
void switch_to_main_menu(GtkWidget *btn, gpointer data);
void signal_error(char* msg);
void draw_game_state(GameState state);
void game_ended_dialog(Player *p);
