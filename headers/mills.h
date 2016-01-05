#include<stdbool.h>

typedef enum {PlayerWhite, PlayerBlack} Player;
typedef enum {Black, White, Empty} Point;
typedef enum {Blacks, Whites, None} Mill;
typedef enum {Phase1, Phase2, Phase3} Phase;
typedef enum {Add, Remove, Move} ActionType;

typedef struct {
  ActionType action_type;
  short int source;
  short int destination;
} Action;

typedef struct {
  Player    current_player;
  Point     board[24];
  Mill      mills[16];
  short int links[24][3];
  short int men_count[2];
  Phase     phase;
} GameState;

GameState update_game_state(GameState old_state, Action action); // assumes action is valid
bool is_valid_action(GameState state, Action action);


