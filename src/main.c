#include <gtk/gtk.h>
#include "gui.h"

GObject *window;
GameState game_state;

int main (int argc, char **argv)
{
  gtk_init (&argc, &argv);
  init_app();
  game_state = init_state();
  
  gtk_main();
  
  
  return 0;
}
