#include<stdbool.h>
#define same_colour(player, point) (player == PlayerWhite ? point == White : point == Black)

typedef enum {PlayerWhite, PlayerBlack} Player;
typedef enum {Empty, White, Black} Point;
typedef enum {None, Blacks, Whites} Mill;
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
  short int men_count[2];
  short int available_men[2];
  Phase     phase;
  bool      mill_created;
} GameState;

GameState update_game_state(GameState old_state, Action action); // assumes action is valid
bool is_valid_action(GameState state, Action action);
GameState init_state();
bool is_finished(GameState state, Player **winner);
void clean_state();


