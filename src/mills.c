#include <stdlib.h>
#include <gtk/gtk.h>
#include "mills.h"
#include "gui.h"
#include "utils.h"
#include "concurrency.h"

#define HISTORY_SIZE 100
#define opponents_color(player) (player == PlayerBlack ? White : Black)
#define in_mill(p, state) (state.mills[mills_to_check[p][0]] != None || state.mills[mills_to_check[p][1]] != None)
#define other_player(state) (state.current_player == PlayerWhite ? PlayerBlack : PlayerWhite)
#define LINKS {{1,9,-1,-1},    {0,2,4,-1},     {1,14,-1,-1},   {4,10,-1,-1},  \
                {1,3,5,7},      {4,13,-1,-1},   {7,11,-1,-1},   {4,6,8,-1},   \
                {7,12,-1,-1},   {0,10,21,-1},   {3,9,11,18},    {6,10,15,-1}, \
                {8,13,17,-1},   {5,12,14,20},   {2,13,23,-1},   {11,16,-1,-1},\
                {15,17,19,-1},  {12,16,-1,-1},  {10,19,-1,-1},  {16,18,20,22},\
                {13,19,-1,-1},  {9,22,-1,-1},   {19,21,23,-1},  {14,22,-1,-1}}

static short int mills_to_check[24][2] = {{0,8}, {0,11}, {0,15}, {1,9}, {1,11}, {1,14},
                                          {2,10},{2,11}, {2,13}, {3,8}, {3,9},  {3,10},
                                          {4,13},{4,14}, {4,15}, {5,10},{5,12}, {5,13},
                                          {6,9}, {6,12}, {6,14}, {7,8}, {7,12}, {7,15}};
                                          
static short int points_in_mill[16][3] = {{0,1,2},    {3,4,5},    {6,7,8},    {9,10,11},
                                          {12,13,14}, {15,16,17}, {18,19,20}, {21,22,23},
                                          {0,9,21},   {3,10,18},  {6,11,15},  {1,4,7},
                                          {16,19,22}, {8,12,17},  {5,13,20},  {2,14,23}};

extern GameState game_state;
extern Player me;
extern PipesPtr fifo_pipes;
static GameState game_history[HISTORY_SIZE];
static int history_head = 0;
static int history_count = 0;
static short int links[24][4] = LINKS;

GameState update_mill(GameState state, short int mill);
GameState update_mills(GameState state, short int dest);
bool is_mill_created(GameState old_state, GameState new_state, short int dest);
bool is_neighbour(GameState state, short int source, short int destination);
bool is_finished(GameState state, Player **winner);
bool check_history_for_repeats();

void add_to_history(GameState state)
{
  game_history[history_head] = state;
  history_head = (history_head + 1) % HISTORY_SIZE;
  history_count++;
}

void clean_state()
{
  game_state = init_state();
  history_head = 0;
  history_count = 0;
}

GameState update_game_state(GameState old_state, Action action)
{ 
  GameState new_state = old_state;
  
  switch (action.action_type) {
  case Move : new_state.board[action.source] = Empty;
              new_state.board[action.destination] = new_state.current_player == PlayerWhite ? White : Black;
              new_state = update_mills(new_state, action.source);
              new_state = update_mills(new_state, action.destination);
              new_state.mill_created = is_mill_created(old_state, new_state, action.destination);
              break;
              
  case Add  : new_state.board[action.destination] = new_state.current_player == PlayerWhite ? White : Black;
              new_state = update_mills(new_state, action.destination);
              new_state.men_count[new_state.current_player]++;
              new_state.mill_created = is_mill_created(old_state, new_state, action.destination);
              break;
              
  case Remove : new_state.board[action.source] = Empty;
                new_state = update_mills(new_state, action.source);
                new_state.men_count[other_player(new_state)]--;
                new_state.available_men[other_player(new_state)]--;
                new_state.mill_created = false;
                break;
  }
  
  if (new_state.mill_created) {
    bool can_remove = false;
    for (int i=0; i<24; i++) {
      if ((new_state.board[i] == opponents_color(new_state.current_player)) &&
                                (!in_mill(i,new_state))) {
        can_remove = true;
        break;
      }
    }
    if (!can_remove) {
      new_state.mill_created = false;
      new_state.current_player = other_player(new_state);
    }
  } else new_state.current_player = other_player(new_state);
  
  if (new_state.phase == Phase1 &&
      new_state.available_men[0] == new_state.men_count[0] && 
      new_state.available_men[1] == new_state.men_count[1]) {
        g_print("Phase 2");
        new_state.phase = Phase2;
  }
  
  if (new_state.phase == Phase2 && (new_state.available_men[0] == 3 || 
      new_state.available_men[1] == 3)) {
    new_state.phase = Phase3;
  }
  g_print("%s\n", serialize_state(new_state));
  save_state(new_state);
  
  return new_state;
}

bool has_legal_moves(GameState state, Player p)
{
  for (int i=0; i<24; i++) {
    if (state.board[i] == (p == PlayerWhite ? White : Black)) {
      for (int j=0; j<4; j++) {
        int n=0;
        if ((n = links[i][j]) >=0 && state.board[n] == Empty)
          return true;
       }
    }
  }
  return false;
}

bool is_finished(GameState state, Player **winner)
{
  if (state.phase >= Phase2) {
    if (state.available_men[0] == 2) {
      **winner = PlayerBlack; return true;
    } else if (state.available_men[1] == 2) {
      **winner = PlayerWhite; return true;
    } else {
      if (!has_legal_moves(state, PlayerWhite)) {
        **winner = PlayerBlack;
        return true;
      } else if (!has_legal_moves(state, PlayerBlack)) {
        **winner = PlayerWhite;
        return true;
      }
      if (check_history_for_repeats()) {
        *winner = NULL;
        return true;
      }
    }
  }
  return false;
}

void save_state(GameState state)
{
  game_state = state;
  add_to_history(state);
  
  Player *p = malloc(sizeof(Player));
  if (is_finished(game_state, &p)) {
    game_ended_dialog(p);
  }
  
  return;
}

bool is_valid_action(GameState state, Action action)
{
  bool valid = true;
  Player cur_plr = state.current_player;
  if (cur_plr != me) return false;
  switch (action.action_type) {
  case Move :   valid = !state.mill_created &&
                        ((state.phase == Phase3 && state.men_count[state.board[action.source] == White ? PlayerWhite : PlayerBlack] == 3) || 
                        (state.phase >= Phase2 && is_neighbour(state, action.source, action.destination)))
                        && state.board[action.destination] == Empty; break;
  case Add :    valid = !state.mill_created &&
                        state.phase == Phase1 && state.board[action.destination] == Empty 
                        && state.available_men[cur_plr] > state.men_count[cur_plr]; break;
  case Remove : valid = state.mill_created &&
                        state.board[action.source] == 
                        (state.current_player == PlayerWhite ? Black : White) &&
                        !in_mill(action.source, state);
                        break;
  }
  return valid;
}

bool check_history_for_repeats()
{
  int n = (history_count < HISTORY_SIZE ? history_count : HISTORY_SIZE);
  int repeats[n];
  for (int i=0; i<n; i++) repeats[i] = 1;
  g_print("checking history for repeats\n");
  for (int i=0; i<n; i++) {
    bool same = true;
    for (int j=i+1; j<n; j++) {
      for (int k=0; k<24; k++) {
        same = game_history[i].board[k] == game_history[j].board[k];
        if (!same) break;
      }
      if (same) {
        repeats[i]++;
        repeats[j]++;
      }
    }
    g_print("repeats for %d: %d\n", i, repeats[i]);
    if (repeats[i] >= 3) {
      g_print("found repeat\n");
      return true;
    }
  }
  g_print("none found\n");
  return false;
}

bool is_neighbour(GameState state, short int source, short int destination)
{
  for (int i=0; i<4; i++) {
    g_print("checking %d's neighbour %d\n", source, links[source][i]);
    if (links[source][i] == destination) return true;
  }
  return false;
}

GameState update_mills(GameState state, short int dest)
{
  for (int i=0; i<2; i++) {
    state = update_mill(state, mills_to_check[dest][i]);
  }
  return state;
}

GameState update_mill(GameState state, short int mill)
{
  Point p = state.board[points_in_mill[mill][0]];
  if (p != Empty && state.board[points_in_mill[mill][1]] == p &&
                    state.board[points_in_mill[mill][2]] == p) {
    state.mills[mill] = p == White ? Whites : Blacks;
    g_print("mill %d of %s\n", mill, p == White ? "whites" : "blacks");
  }
  else {
    state.mills[mill] = None;
  }
  g_print("no mill nr %d\n", mill);
  return state;
}

bool is_mill_created(GameState old_state, GameState new_state, short int dest)
{
  for (int i=0; i<2; i++) {
    if (old_state.mills[mills_to_check[dest][i]] == None &&
        new_state.mills[mills_to_check[dest][i]] != None) return true;
  }
  return false;
}

GameState init_state()
{
  GameState state =  { PlayerWhite, { Empty }, { None }, {0,0}, { 9, 9 }, Phase1, false};
                  // deserialize_state("{\"current_player\":0,\"board\":[0,0,0,0,0,1,2,0,0,0,0,0,0,2,0,0,1,0,0,0,0,0,0,0],\"mills\":[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0],\"men_count\":[2,2],\"available_men\":[4,4],\"phase\":0,\"mill_created\":false}");
  
  return state;
}


