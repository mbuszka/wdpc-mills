#include <gtk/gtk.h>
#include "gui.h"

GObject *window;
GameState game_state;

int main (int argc, char **argv)
{
  gtk_init (&argc, &argv);
  init_app();
  
  gtk_main();
  game_state = init_state();
  
  return 0;
}
