
#define FOOTBALL_PLAYERS 44
#define FOOTBALL_PLAYER_CAP 22
#define FOOTBALL_GAME_TIME 5

void init_football();

void football_run_game();

int football_ready();

void football_join_game(long tid);

void *footballPlayer(void *arg);
