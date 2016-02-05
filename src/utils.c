#include "mills.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "json.h"

#define serialize_arr(json_arr, arr, length) for (int i=0; i<length; i++) { \
    JsonNode *buf = json_mknumber((double) arr[i]); \
    json_append_element(json_arr, buf); \
  }

#define deserialize_arr(json_arr, arr, length) for (int i=0; i<length; i++) { \
    arr[i] = (int) json_find_element(json_arr, i)->number_; \
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
  json_delete(json);
  
  return out;
}

GameState deserialize_state(char *json_str)
{
  GameState state;
  JsonNode *json = json_decode(json_str);
  
  JsonNode *current_player = json_find_member(json, "current_player");
  state.current_player = (Point) current_player->number_;
  
  JsonNode *board = json_find_member(json, "board");
  deserialize_arr(board, state.board, 24);
  
  JsonNode *mills = json_find_member(json, "mills");
  deserialize_arr(mills, state.mills, 16);
  
  JsonNode *men_count = json_find_member(json, "men_count");
  deserialize_arr(men_count, state.men_count, 2);
  
  JsonNode *available_men = json_find_member(json, "available_men");
  deserialize_arr(available_men, state.available_men, 2);
  
  JsonNode *phase = json_find_member(json, "phase");
  state.phase = (Phase) phase->number_;
  
  JsonNode *mill_created = json_find_member(json, "mill_created");
  state.mill_created = mill_created->bool_;
  
  return state;
}

char *serialize_action(Action a)
{
  JsonNode *json = json_mkobject();
  
  JsonNode *a_type = json_mknumber((double) a.action_type);
  JsonNode *src = json_mknumber((double) a.source);
  JsonNode *dst = json_mknumber((double) a.destination);
  
  json_append_member(json, "action_type", a_type);
  json_append_member(json, "source", src);
  json_append_member(json, "destination", dst);
  
  char *out = json_encode(json);
  json_delete(json);
  
  return out;
}

Action deserialize_action(char *str)
{
  Action a;
  JsonNode *json = json_decode(str);
  a.action_type = (ActionType) json_find_member(json, "action_type")->number_;
  a.source = (int) json_find_member(json, "source")->number_;
  a.destination = (int) json_find_member(json, "destination")->number_;
  json_delete(json);
  return a;
}


