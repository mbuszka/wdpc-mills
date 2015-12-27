#include <gtk/gtk.h>
#include "gui.h"


GObject *window;

int main (int argc, char **argv)
{
  gtk_init (&argc, &argv);
  init_app();
  
  gtk_main();
  
  return 0;
}
