#include"gui.h"
#include <stdlib.h>

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

GameState update_mill(GameState state, short int mill);
GameState update_mills(GameState state, short int dest);
bool is_mill_created(GameState old_state, GameState new_state, short int dest);
bool is_neighbour(GameState state, short int source, short int destination);
bool is_finished(GameState state, Player *winner);

GameState update_game_state(GameState old_state, Action action)
{ 
  GameState new_state = old_state;
  static bool mill_created = false;
  bool switch_player = true;
  switch (action.action_type) {
  case Move : if (mill_created) break;
              new_state.board[action.source] = Empty;
              new_state.board[action.destination] = new_state.current_player == PlayerWhite ? White : Black;
              new_state = update_mills(new_state, action.source);
              new_state = update_mills(new_state, action.destination);
              mill_created = is_mill_created(old_state, new_state, action.destination);
              break;
              
  case Add  : if (mill_created) break;
              new_state.board[action.destination] = new_state.current_player == PlayerWhite ? White : Black;
              new_state = update_mills(new_state, action.destination);
              new_state.men_count[new_state.current_player]++;
              mill_created = is_mill_created(old_state, new_state, action.destination);
              break;
              
  case Remove : if (!mill_created) {switch_player = false; break;}
                if (in_mill(action.source, new_state)) {signal_error("This point is in mill, cant remove"); switch_player = false; break;}
                new_state.board[action.source] = Empty;
                new_state = update_mills(new_state, action.source);
                new_state.men_count[other_player(new_state)]--;
                new_state.available_men[other_player(new_state)]--;
                mill_created = false;
                break;
  }
  if (mill_created) {
    for (int i=0; i<24; i++) {
      if ((new_state.board[i] == opponents_color(new_state.current_player)) &&
                                (!in_mill(i,new_state))) {
        switch_player = false;
        break;
      }
    }
    if (switch_player) mill_created = false;
  }
  if (new_state.phase == Phase1 &&
      new_state.available_men[0] == new_state.men_count[0] && 
      new_state.available_men[1] == new_state.men_count[1]
      /*new_state.men_count[0] > 1*/) {
        signal_error("Phase 2");
        new_state.phase = Phase2;
      }
  
  if (new_state.phase == Phase2 && (new_state.available_men[0] == 3 || new_state.available_men[1] == 3))
    new_state.phase = Phase3;
  
  if (switch_player)
    new_state.current_player = other_player(new_state);
  
  Player p; char msg[50];
  if (is_finished(new_state, &p)) {
    sprintf(msg, "Game won by player %s", p == PlayerWhite ? PLAYER_WHITE_NAME : PLAYER_BLACK_NAME);
    signal_error(msg);
  }
  return new_state;
}

bool has_legal_moves(GameState state, Player p)
{
  for (int i=0; i<24; i++) {
    if (state.board[i] == (p == PlayerWhite ? White : Black)) {
      for (int j=0; j<4; j++) {
        int n=0;
        if ((n = state.links[i][j]) >=0 && state.board[n] == Empty)
          return true;
       }
    }
  }
  return false;
}

bool is_finished(GameState state, Player *winner)
{
  if (state.available_men[0] == 2) {
    *winner = PlayerBlack; return true;
  } else if (state.available_men[1] == 2) {
    *winner = PlayerWhite; return true;
  } else {
    if (!has_legal_moves(state, PlayerWhite)) {
      *winner = PlayerBlack;
      return true;
    } else if (!has_legal_moves(state, PlayerBlack)) {
      *winner = PlayerWhite;
      return true;
    }
  }
  return false;
}

bool is_valid_action(GameState state, Action action)
{
  bool valid = true;
  Player cur_plr = state.current_player;
  switch (action.action_type) {
  case Move :   valid = ((state.phase == Phase3 && state.men_count[state.board[action.source] == White ? PlayerWhite : PlayerBlack] == 3) || 
                        (state.phase >= Phase2 && is_neighbour(state, action.source, action.destination)))
                        && state.board[action.destination] == Empty; break;
  case Add :    valid = state.phase == Phase1 && state.board[action.destination] == Empty 
                        && state.available_men[cur_plr] > state.men_count[cur_plr]; break;
  case Remove : valid = state.board[action.source] == 
                        (state.current_player == PlayerWhite ? Black : White);
                        break;
  }
  return valid;
}

bool is_neighbour(GameState state, short int source, short int destination)
{
  for (int i=0; i<4; i++) {
    g_print("checking %d's neighbour %d\n", source, state.links[source][i]);
    if (state.links[source][i] == destination) return true;
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
  g_print("no mill nr %d", mill);
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
  GameState state = { PlayerWhite, { Empty }, { None }, LINKS, {0,0}, { 4, 4 }, Phase1};
  
  return state;
}


