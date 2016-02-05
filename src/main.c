#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include "mills.h"
#include "gui.h"
#include "concurrency.h"

GObject *window;
GameState game_state;
Player me;
PipesPtr fifo_pipes;

int main (int argc, char **argv)
{
  if (argc != 2) {
    printf("app usage: mills player_number\nwhere player_number is 1 or 2\n");
    exit(0);
  } else if (argv[1][0] == '1') {
    me = PlayerWhite;
  } else {
    me = PlayerBlack;
  }
  fifo_pipes = init_pipes(me);
  gtk_init (&argc, &argv);
  init_app();
  game_state = init_state();
  
  
  gtk_main();
  
  return 0;
}
