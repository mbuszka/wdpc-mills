char *serialize_state(GameState state);
GameState deserialize_state(char *json_str);
char *serialize_action(Action a);
Action deserialize_action(char *str);
