#include <gtk/gtk.h>
#include "gui.h"


GtkApplication *app;

int main (int argc, char **argv)
{
  int status;
  app = init_app();
  status = g_application_run (G_APPLICATION (app), argc, argv);
  
  g_object_unref (app);

  return status;
}
