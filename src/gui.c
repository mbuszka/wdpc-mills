#include <gtk/gtk.h>
#include "gui.h"
#include <stdlib.h>

#define MAIN_MENU_NAME "main_menu"
#define GAME_SESSION_NAME "game_session"
#define VIEW_STACK_NAME "view_stack"
#define PLAYER_WHITE_NAME "Red"
#define PLAYER_BLACK_NAME "Blue"

void build_game_area();
void set_label(Player p);

extern GObject *window;
extern GameState game_state;


static GtkWidget *view_stack;
static GtkWidget *current_view;
static GtkWidget *main_menu_view;
static GtkWidget *game_session_view;
static GtkWidget *board_tiles[24];
static GtkWidget *background_tiles[25];
static GtkWidget *game_area;
static GtkWidget *current_player_label;
static GtkWidget *player_1_men_count_label;
static GtkWidget *player_2_men_count_label;
static GtkWidget *phase_indicator;

static void switch_view(GtkWidget *new_view)
{
  gtk_widget_show(new_view);
  gtk_stack_set_visible_child(GTK_STACK (view_stack), new_view);
  current_view = new_view;
}

void switch_to_main_menu(GtkWidget *btn, gpointer data)
{
  switch_view(main_menu_view);
  return;
}

void switch_to_game_session(GtkWidget *btn, gpointer data)
{
  if (game_area == NULL) {
    build_game_area();
    gtk_container_add(GTK_CONTAINER (game_session_view), game_area);
    gtk_widget_show_all(game_session_view);
  }
  draw_game_state(game_state);
  switch_view(game_session_view);
  return;
}

void handle_board_click(GtkWidget *btn, gpointer data)
{
  static bool waiting_for_drop = false;
  static short int drag_source;
  int idx=0;
  
  for (;idx<24; idx++) {
    if (btn == board_tiles[idx]) break;
  }
  if (idx>=24) printf("error didnt find right board tile");
  
  if (same_colour(game_state.current_player, game_state.board[idx])) {
    waiting_for_drop = true;
    drag_source = idx;
    return;
  }
    
  Action a = { Add, idx, idx };
  if (waiting_for_drop) {
    a.action_type = Move;
    a.source = drag_source;
    a.destination = idx;
    waiting_for_drop = false;
    g_print("moving!\n");
  } else if (game_state.board[idx] == Empty) {
    a.action_type = Add;
  } else {
    a.action_type = Remove;
  }
  if (is_valid_action(game_state, a)) {
    game_state = update_game_state(game_state, a);
  }
  draw_game_state(game_state);
  
  Player *p = malloc(sizeof(Player));
  if (is_finished(game_state, &p)) {
    game_ended_dialog(p);
    clean_state();
    switch_to_main_menu(NULL,NULL);
  }
  return;
}
  
void set_label(Player p)
{
  gtk_label_set_label(GTK_LABEL(current_player_label), p == PlayerWhite ? PLAYER_WHITE_NAME : PLAYER_BLACK_NAME);
  return;
}

void build_game_area()
{
  GtkWidget *grid = gtk_grid_new();
  for (int i=0; i<24; i++) {
    board_tiles[i] = gtk_event_box_new();
    GtkWidget *img = gtk_image_new_from_file("res/empty.png");
    g_signal_connect(board_tiles[i], "button-press-event", G_CALLBACK (handle_board_click), NULL);
    gtk_container_add(GTK_CONTAINER (board_tiles[i]), img);
  }
  for (int i=0; i<25; i++) {
    background_tiles[i] = gtk_image_new_from_file("res/background.png");
  }
  int tile_ctr=0; int bgr_ctr=0;
  
  current_player_label = gtk_label_new(PLAYER_WHITE_NAME);
  player_1_men_count_label = gtk_label_new("Men left: ");
  player_2_men_count_label = gtk_label_new("Men left: ");
  phase_indicator = gtk_label_new("Phase 1");
  GtkWidget *scorebox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  GtkWidget *infobox  = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous(GTK_BOX(scorebox), TRUE);
  gtk_box_set_homogeneous(GTK_BOX(infobox), TRUE);
  gtk_box_pack_end(GTK_BOX(scorebox), player_2_men_count_label, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(scorebox), player_1_men_count_label, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(infobox), current_player_label, TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(infobox), phase_indicator, TRUE, TRUE, 0);
  gtk_grid_attach(GTK_GRID (grid), infobox, 0, 0, 7, 1);
  
  for (int i=0; i<7; i++) {
    gtk_grid_attach(GTK_GRID (grid), i%3 == 0 ? board_tiles[tile_ctr++] : background_tiles[bgr_ctr++], i, 1, 1, 1);
  }
  for (int i=0; i<7; i++) {
    gtk_grid_attach(GTK_GRID (grid), i%2 == 1 ? board_tiles[tile_ctr++] : background_tiles[bgr_ctr++], i, 2, 1, 1);
  }
  for (int i=0; i<7; i++) {
    gtk_grid_attach(GTK_GRID (grid), i>1 && i<5 ? board_tiles[tile_ctr++] : background_tiles[bgr_ctr++], i, 3, 1, 1);
  }
  for (int i=0; i<7; i++) {
    gtk_grid_attach(GTK_GRID (grid), i%7 != 3 ? board_tiles[tile_ctr++] : background_tiles[bgr_ctr++], i, 4, 1, 1);
  }
  for (int i=0; i<7; i++) {
    gtk_grid_attach(GTK_GRID (grid), i>1 && i<5 ? board_tiles[tile_ctr++] : background_tiles[bgr_ctr++], i, 5, 1, 1);
  }
  for (int i=0; i<7; i++) {
    gtk_grid_attach(GTK_GRID (grid), i%2 == 1 ? board_tiles[tile_ctr++] : background_tiles[bgr_ctr++], i, 6, 1, 1);
  }
  for (int i=0; i<7; i++) {
    gtk_grid_attach(GTK_GRID (grid), i%3 == 0 ? board_tiles[tile_ctr++] : background_tiles[bgr_ctr++], i, 7, 1, 1);
  }
  gtk_grid_attach(GTK_GRID (grid), scorebox, 0, 8, 7, 1);
  game_area = grid;
  return;
}

void set_tile_image(short int tile, Point p)
{
  char *file;
  if (p == Empty) file = "res/empty.png";
  else file = p == White ? "res/player_1.png" : "res/player_2.png";
  gtk_image_set_from_file(GTK_IMAGE (gtk_bin_get_child(GTK_BIN (board_tiles[tile]))), file);
  return;
}

void draw_game_state(GameState state)
{
  char msg1[50], msg2[50], phase[20];
  sprintf(msg1, "Player %s men left: %d", PLAYER_WHITE_NAME, state.available_men[0]-state.men_count[0]);
  sprintf(msg2, "Player %s men left: %d", PLAYER_BLACK_NAME, state.available_men[1]-state.men_count[1]);
  sprintf(phase, "Phase %d", state.phase + 1);
  gtk_label_set_label(GTK_LABEL(player_1_men_count_label), msg1);
  gtk_label_set_label(GTK_LABEL(player_2_men_count_label), msg2);
  gtk_label_set_label(GTK_LABEL(phase_indicator), phase);
  for (int i=0; i<24; i++) {
    set_tile_image(i, state.board[i]);
  }
  set_label(game_state.current_player);
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
  switch_view(main_menu_view);
  
  return;
}

void game_ended_dialog(Player *p) {
  char msg[50];
  if (p == NULL) sprintf(msg, "Draw");
  else sprintf(msg, "Game won by player %s", *p == PlayerWhite ? PLAYER_WHITE_NAME : PLAYER_BLACK_NAME);
  GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new (GTK_WINDOW(window),
                                   flags,
                                   GTK_MESSAGE_ERROR,
                                   GTK_BUTTONS_CLOSE,
                                   msg);

  // Destroy the dialog when the user responds to it
  // (e.g. clicks a button)

  g_signal_connect_swapped (dialog, "response",
                            G_CALLBACK (gtk_widget_destroy),
                            dialog);
                            
  gtk_dialog_run (GTK_DIALOG (dialog));
}

void signal_error(char *msg)
{
  g_print("%s\n", msg);
  return;
}
