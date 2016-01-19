#include "mills.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json.h"

#define serialize_arr(json_arr, arr, length) for (int i=0; i<length; i++) { \
    JsonNode *buf = json_mknumber((double) arr[i]); \
    json_append_element(json_arr, buf); \
  }

char *serialize_state(GameState state)
{
  JsonNode *json = json_mkobject();
  
  JsonNode *current_player = json_mknumber((double) state.current_player);
  json_append_member(json, "current_player", current_player);
  
  JsonNode *board = json_mkarray();
  serialize_arr(board, state.board, 24);
  json_append_member(json, "board", board);
  
  JsonNode *mills = json_mkarray();
  serialize_arr(mills, state.mills, 16);
  json_append_member(json, "mills", mills);
  
  JsonNode *men_count = json_mkarray();
  serialize_arr(men_count, state.men_count, 2);
  json_append_member(json, "men_count", men_count);
  
  JsonNode *available_men = json_mkarray();
  serialize_arr(available_men, state.available_men, 2);
  json_append_member(json, "available_men", available_men);
  
  JsonNode *phase = json_mknumber((double) state.phase);
  json_append_member(json, "phase", phase);
  
  JsonNode *mill_created = json_mkbool(state.mill_created);
  json_append_member(json, "mill_created", mill_created);
  
  char *out = json_encode(json);
  return out;
}

GameState deserialize_state(char *json_str)
{
  JsonNode json = json_decode(json_str);
}
