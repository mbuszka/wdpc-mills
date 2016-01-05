#include"mills.h"
#define other_player(state) (state.current_player == PlayerWhite ? PlayerBlack : PlayerWhite)


GameState update_game_state(GameState state, Action action)
{ 
  switch (action.action_type) {
  case Move : state.board[action.source] = Empty;
              state.board[action.destination] = state.current_player; break;
              
  case Add  : state.board[action.destination] = state.current_player; break;
  case Remove : state.board[action.source] = Empty; break;
  }
  state.current_player = other_player(state);
  return state;
}

bool is_neighbour(GameState state, short int source, short int destination)
{
  for (int i=0; i<3; i++)
    if (state.links[source][i] == destination) return true;
  
  return false;
}

bool is_valid_action(GameState state, Action action)
{
  bool valid = true;
  switch (action.action_type) {
  case Move :   valid = is_neighbour(state, action.source, action.destination)
                        && state.board[action.destination] == Empty; break;
  case Add :    valid = state.board[action.destination] == Empty; break;
  case Remove : valid = state.board[action.source] == 
                        (state.current_player == PlayerWhite ? Black : White);
                        break;
  }
  return valid;
}


