typedef enum {Player1, Player2} Player;
typedef enum {Black, White, Empty} Point;
typedef enum {Black, White, None} Mill;
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

GameState update_game_state(GameState old_state, Action action);

