#include <stdbool.h>

typedef struct pipes *PipesPtr;

PipesPtr init_pipes(Player player_no);
void     send_string_to_pipe(PipesPtr channel, const char *data);
bool     get_string_from_pipe(PipesPtr channel, char *buffer, size_t size);
void     close_pipes(PipesPtr channel);
