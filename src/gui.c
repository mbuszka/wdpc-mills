#include <gtk/gtk.h>
#include "gui.h"

#define MAIN_MENU_NAME "main_menu"
#define GAME_SESSION_NAME "game_session"
#define VIEW_STACK_NAME "view_stack"

extern GObject *window;

static GtkWidget *view_stack;
static GtkWidget *current_view;
static GtkWidget *main_menu_view;
static GtkWidget *game_session_view;

static void switch_view(GtkWidget *new_view)
{
  gtk_widget_show(new_view);
  gtk_stack_set_visible_child(GTK_STACK (view_stack), new_view);
  current_view = new_view;
}

void return_to_main_menu(GtkWidget *btn, gpointer data)
{
  switch_view(main_menu_view);
  return;
}

void init_app()
{ 
  GtkBuilder *builder;
  builder = gtk_builder_new();
  gtk_builder_add_from_file(builder, "gtk/layout.ui", NULL);
  window = gtk_builder_get_object (builder, "window");
  
  gtk_builder_connect_signals(builder, NULL);
  view_stack = GTK_WIDGET (gtk_builder_get_object(builder, VIEW_STACK_NAME));
  main_menu_view = GTK_WIDGET (gtk_builder_get_object(builder, MAIN_MENU_NAME));
  game_session_view = GTK_WIDGET (gtk_builder_get_object(builder, GAME_SESSION_NAME));
  switch_view(game_session_view);
  
  return;
}
