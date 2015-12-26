#include <gtk/gtk.h>
#include "gui.h"

#define MAIN_MENU_NAME "main_menu"
#define GAME_SESSION_NAME "game_session"

extern GtkApplication *app;

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

static void activate (GtkApplication* app, gpointer user_data)
{
  GtkWidget *window;
  GtkWidget *label_mm;
  GtkWidget *label_gs;
  
  window = gtk_application_window_new (app);
  view_stack = gtk_stack_new();
  
  main_menu_view = gtk_label_new(MAIN_MENU_NAME);
  game_session_view = gtk_label_new(GAME_SESSION_NAME);
  
  gtk_stack_add_named(GTK_STACK (view_stack), main_menu_view, MAIN_MENU_NAME);
  gtk_stack_add_named(GTK_STACK (view_stack), game_session_view, GAME_SESSION_NAME);
  
  switch_view(main_menu_view);
  
  gtk_container_add (GTK_CONTAINER (window), view_stack);
  gtk_window_set_title (GTK_WINDOW (window), "Mills");
  gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
  gtk_widget_show_all (window);
}

GtkApplication *init_app()
{
  GtkApplication *app;
  app = gtk_application_new (NULL, G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  return app;
}
