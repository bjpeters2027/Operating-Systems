#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <sys/time.h>

#define BASEBALL_PLAYERS 36
#define BASEBALL_PLAYER_CAP 18

#define RUGBY_PLAYERS 60
#define RUGBY_PLAYER_CAP 2

#define FOOTBALL_PLAYERS 44
#define FOOTBALL_PLAYER_CAP 22

// #region Structs ----------------------------------------------------------------------

typedef enum{
    Football = 0,
    Baseball = 1,
    Rugby = 2
} Sport;

struct{
    Sport currentSport;
    pthread_cond_t fieldReady;
    bool gameInPlay;
    pthread_mutex_t m;
} field;

struct { /* data shared by producer and consumer */
    pthread_mutex_t m;
    int onField; // Value of how many players to bring onto field (gets decremented to 0 as they enter)
    int offField; // Value of how many players to take off the field (gets decremented to 0 as they exit)
    pthread_cond_t startGame;
    pthread_cond_t finishGame;
    int inqueue;
    sem_t emptySpots;
    sem_t gameCaptain;
} football;

struct { /* data shared by producer and consumer */
    pthread_mutex_t m;
    int onField; // Value of how many players to bring onto field (gets decremented to 0 as they enter)
    int offField; // Value of how many players to take off the field (gets decremented to 0 as they exit)
    pthread_cond_t startGame;
    pthread_cond_t finishGame;
    int inqueue;
    sem_t emptySpots;
    sem_t gameCaptain;
} baseball;

struct { /* data shared by producer and consumer */
    pthread_mutex_t m;
    int onField; // Value of how many players to bring onto field (gets decremented to 0 as they enter)
    int offField; // Value of how many players to take off the field (gets decremented to 0 as they exit)
    pthread_cond_t startGame;
    pthread_cond_t finishGame;
    int inqueue;
    int numOfCaps; // Ensures that only 30 players on at once
    int gameCount; // Limits the total number of pairs to enter the field
    sem_t emptySpots;
    sem_t gameCaptain;
} rugby;

// #endregion

// #region General Functions --------------------------------------------------------------------

void init_field() {
    field.currentSport = Football;
    pthread_cond_init(&field.fieldReady, NULL);
    field.gameInPlay = false;
}

Sport nextSport() {
    return (field.currentSport + 1) % 3;
}

void player_sleep() {
    usleep((rand() % 500) * 10000 + 0);
}

void init_player(long int tid) {
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec);
}

// #endregion

// #region Football ------------------------------------------------------------

void football_sleep() {
    int sleepTime = rand() % 5  + 3;
    printf("The Football game will run for %i seconds.\n", sleepTime);
    sleep(sleepTime);
}

void init_football() {
    pthread_mutex_init(&football.m, NULL);
    football.onField = 0;
    football.offField = 0;
    pthread_cond_init(&football.startGame, NULL);
    pthread_cond_init(&football.finishGame, NULL);
    int v = sem_init(&football.emptySpots, 0, FOOTBALL_PLAYER_CAP);
    sem_init(&football.gameCaptain, 0, 1);
    printf("Return of sem_init: %d\n", v);
}

int football_ready() {
    int queue;
    sem_getvalue(&football.emptySpots, &queue);
    return queue <= 0;
}

void football_run_game(long tid) {
    // Wait untill enough players read to play a game then until the field is clear and football is the next sport to play
    printf("Football Player #%li (Next Captain): Waiting for the field to be open and players to be ready.\n", tid);
    pthread_mutex_lock(&field.m);
    while (!football_ready()) {sleep(1);}
    while (field.gameInPlay || nextSport() != Football) {
        pthread_cond_wait(&field.fieldReady, &field.m);
    }
    field.gameInPlay = true;
    pthread_mutex_unlock(&field.m);

    // Move players onto field and set current sport to football
    printf("Football Player #%ld (Captain): Moving players to field.\n", tid);
    sem_post(&football.gameCaptain);
    pthread_mutex_lock(&football.m);
    football.onField = FOOTBALL_PLAYER_CAP-1;
    pthread_mutex_unlock(&football.m);
    pthread_cond_broadcast(&football.startGame);
    pthread_mutex_lock(&field.m);
    field.currentSport = Football;
    pthread_mutex_unlock(&field.m);

    
    // Wait for the football game to be over
    football_sleep();

    // Move the players off the field 
    printf("Football Player #%li (Captain): Finished game, moving players off the field.\n", tid);
    pthread_mutex_lock(&football.m);
    football.offField = FOOTBALL_PLAYER_CAP-1;
    pthread_mutex_unlock(&football.m);
    pthread_cond_broadcast(&football.finishGame);

    // Broadcast that the field is now ready for the next sport to play
    printf("Football Player #%li (Captain): Opening field for next game.\n", tid);
    pthread_mutex_lock(&field.m);
    field.gameInPlay = false;
    pthread_mutex_unlock(&field.m);
    pthread_cond_broadcast(&field.fieldReady);
}

void football_join_game(long tid) {
    // Enter queue and wait to be the next up to play
    // printf("Football Player #%ld: Entering football queue.\n", tid);
    
    sem_wait(&football.emptySpots);

    if (!sem_trywait(&football.gameCaptain)) {
        printf("Football Player #%li: I am the next game captain.\n", tid);
        return football_run_game(tid);
    }
    
    // Wait for next football game to start to go on field
    printf("Football Player #%ld: Next up to play football.\n", tid);
    pthread_mutex_lock(&football.m);
    while (football.onField == 0)
        pthread_cond_wait(&football.startGame, &football.m);
    football.onField--;
    pthread_mutex_unlock(&football.m);
    sem_post(&football.emptySpots);

    // Once signaled leave the field after the game
    pthread_mutex_lock(&football.m);
    while (football.offField == 0)
        pthread_cond_wait(&football.finishGame, &football.m);
    football.offField--;
    pthread_mutex_unlock(&football.m);
}

void *footballPlayer(void *arg) {
    long tid = (long)arg;
    init_player(tid);
    while (1) {
        player_sleep();
        football_join_game(tid);
    }
    return(NULL);
}
// #endregion

// #region Baseball ------------------------------------------------------------

void baseball_sleep() {
    int sleepTime = rand() % 5  + 3;
    printf("The Baseball game will run for %i seconds.\n", sleepTime);
    sleep(sleepTime);
}

void init_baseball() {
    pthread_mutex_init(&baseball.m, NULL);
    pthread_cond_init(&baseball.startGame, NULL);
    pthread_cond_init(&baseball.finishGame, NULL);

    int v = sem_init(&baseball.emptySpots, 0, BASEBALL_PLAYER_CAP);
    sem_init(&baseball.gameCaptain, 0, 1);
    printf("Return of sem_init: %d\n", v);
}

int baseball_ready() {
    int queue;
    sem_getvalue(&baseball.emptySpots, &queue);
    return queue <= 0;
}

void baseball_run_game(long tid) {
    // Wait untill enough players ready to play a game then until the field is clear and baseball is the next sport to play
    printf("Baseball Player #%li (Next Captain): Waiting for the field to be open and players to be ready.\n", tid);
    pthread_mutex_lock(&field.m);

    while (!baseball_ready()) {sleep(1);}
    while (field.gameInPlay || nextSport() != Baseball) {
        pthread_cond_wait(&field.fieldReady, &field.m);
    }
    field.gameInPlay = true;
    pthread_mutex_unlock(&field.m);

    // Move other players onto feild and set current sport to baseball
    printf("Baseball Player #%ld (Captain): Moving players to field.\n", tid);
    sem_post(&baseball.gameCaptain);
    sem_post(&baseball.emptySpots);
    pthread_mutex_lock(&baseball.m);
    baseball.onField = BASEBALL_PLAYER_CAP-1;
    pthread_mutex_unlock(&baseball.m);
    pthread_cond_broadcast(&baseball.startGame);

    pthread_mutex_lock(&field.m);
    field.currentSport = Baseball;
    pthread_mutex_unlock(&field.m);

    
    // Wait for the baseball game to be over
    baseball_sleep();

    // Take the players off the field 
    printf("Baseball Player #%li (Captain): Finished game, moving players off the field.\n", tid);
    pthread_mutex_lock(&baseball.m);
    baseball.offField = BASEBALL_PLAYER_CAP-1;
    pthread_mutex_unlock(&baseball.m);
    pthread_cond_broadcast(&baseball.finishGame);

    // Broadcast that the field is now ready for the next sport to play
    printf("Baseball Player #%li (Captain): Opening field for next game.\n", tid);
    pthread_mutex_lock(&field.m);
    field.gameInPlay = false;
    pthread_mutex_unlock(&field.m);
    pthread_cond_broadcast(&field.fieldReady);
}

void baseball_join_game(long tid) {
    // Enter queue and wait until next up to play
    sem_wait(&baseball.emptySpots);
     
    // Try to be captain (one per game) otherwise continue as normal player
    if (!sem_trywait(&baseball.gameCaptain)) {
        printf("Baseball Player #%li: I am the next game captain.\n", tid);
        return baseball_run_game(tid);
    }
    
    // Wait for next baseball game to start before going on the field
    printf("Baseball Player #%ld: Next up to play baseball.\n", tid);
    pthread_mutex_lock(&baseball.m);
    while (baseball.onField == 0)
        pthread_cond_wait(&baseball.startGame, &baseball.m);
    baseball.onField--;
    pthread_mutex_unlock(&baseball.m);
    sem_post(&baseball.emptySpots);

    // Wait for game to be finished then exit field
    pthread_mutex_lock(&baseball.m);
    while (baseball.offField == 0)
        pthread_cond_wait(&baseball.finishGame, &baseball.m);
    baseball.offField--;
    pthread_mutex_unlock(&baseball.m);
}

void *baseballPlayer(void *arg) {
    long tid = (long)arg;
    init_player(tid);
    while (1) {
        player_sleep();
        baseball_join_game(tid);
    }
    return(NULL);
}
// #endregion

// #region Rugby ----------------------------------------------------------------------

void rugby_sleep() {
    int sleepTime = rand() % 5  + 3;
    printf("The Rugby game will run for %i seconds.\n", sleepTime);
    sleep(sleepTime);
}

void init_rugby() {
    pthread_mutex_init(&rugby.m, NULL);
    rugby.onField = 0;
    rugby.offField = 0;
    rugby.numOfCaps = 0;
    rugby.gameCount = 0;
    pthread_cond_init(&rugby.startGame, NULL);
    pthread_cond_init(&rugby.finishGame, NULL);
    int v = sem_init(&rugby.emptySpots, 0, RUGBY_PLAYER_CAP);
    sem_init(&rugby.gameCaptain, 0, 1);
    printf("Return of sem_init: %d\n", v);
}

int rugby_ready() {
    int queue;
    sem_getvalue(&rugby.emptySpots, &queue);
    return queue <= 0;
}

void rugby_run_game(long tid) {
    // Wait untill enough players read to play a game then until the field is clear and rugby is the next sport to play
    // NOTE: because of the while loop in the if statement,
    // the system will allow up to 30 (15 pairs) of rugby players to enter total,
    // Then once it reaches that the next captain will enter the loop and wont
    // exit until the next start of a rugby game
    // (because the fieldReady condition wont be signaled as rugby players leave and spots open)
    printf("Rugby Player #%li (Captain): Waiting for the field to be open and players to be ready.\n", tid);
    pthread_mutex_lock(&field.m);
    // Wait until partner is ready to enter field
    while (!rugby_ready()) {sleep(1);}
    printf("Rugby Player #%li (Captain): Waiting on field to open.\n", tid);
    //If no game in play, the current sport is rugby, and less than 30 rugby players in field move on
    if (!field.gameInPlay || field.currentSport != Rugby || rugby.gameCount >= 15) {
        // Otherwise wait untill good to start new game (if rugby currently in play with slots open still wont enter)
        while (field.gameInPlay || nextSport() != Rugby) {
            pthread_cond_wait(&field.fieldReady, &field.m);
        }
    }
    field.gameInPlay = true;
    pthread_mutex_unlock(&field.m);


    // Signal partner to enter field and set current sport to rugby (Also increase num of caps accordingly)
    sem_post(&rugby.gameCaptain);
    sem_post(&rugby.emptySpots);
    pthread_mutex_lock(&rugby.m);
    rugby.numOfCaps++;
    rugby.gameCount++;
    printf("Rugby Player #%ld (Captain): Moving players to field (Players on field: %i).\n", tid, rugby.numOfCaps*2);
    rugby.onField = RUGBY_PLAYER_CAP-1;
    pthread_mutex_unlock(&rugby.m);
    pthread_cond_signal(&rugby.startGame);
    pthread_mutex_lock(&field.m);
    field.currentSport = Rugby;
    pthread_mutex_unlock(&field.m);


    // Sleep for rugby game length
    rugby_sleep();

    
    // Decrement num of captains and signal for another player to leave
    pthread_mutex_lock(&rugby.m);
    int caps = --rugby.numOfCaps;
    printf("Rugby Player #%li (Captain): Finished game, moving players off the field (Players on field: %i).\n", tid, rugby.numOfCaps * 2);
    rugby.offField = RUGBY_PLAYER_CAP-1;
    pthread_mutex_unlock(&rugby.m);
    pthread_cond_signal(&rugby.finishGame);


    if (caps == 0) {
        // Broadcast that the field is now ready for the next sport to play
        printf("Rugby Player #%li (Captain): Opening field for next game.\n", tid);
        pthread_mutex_lock(&field.m);
        field.gameInPlay = false;
        pthread_mutex_unlock(&field.m);
        pthread_cond_broadcast(&field.fieldReady);
        pthread_mutex_lock(&rugby.m);
        rugby.gameCount = 0;
        pthread_mutex_unlock(&rugby.m);
    }

}

void rugby_join_game(long tid) {
    // Enter queue and wait until next up to play
    
    // Allow two players to pass and seperate out one as a captain to organize the game
    sem_wait(&rugby.emptySpots);
    if (!sem_trywait(&rugby.gameCaptain)) {
        printf("Rugby Player #%li: I am the next game captain.\n", tid);
        return rugby_run_game(tid);
    }
    
    // Wait for rugby game to start
    printf("Rugby Player #%ld: Next up to play rugby.\n", tid);
    pthread_mutex_lock(&rugby.m);
    while (rugby.onField == 0)
        pthread_cond_wait(&rugby.startGame, &rugby.m);
    rugby.onField--;
    pthread_mutex_unlock(&rugby.m);
    sem_post(&rugby.emptySpots);

    // Wait for signal to leave field
    pthread_mutex_lock(&rugby.m);
    while (rugby.offField == 0)
        pthread_cond_wait(&rugby.finishGame, &rugby.m);
    rugby.offField--;
    pthread_mutex_unlock(&rugby.m);
}

void *rugbyPlayer(void *arg) {
    long tid = (long)arg;
    init_player(tid);
    while (1) {
        player_sleep();
        rugby_join_game(tid);
    }
    return(NULL);
}

// #endregion

int main () {    
    init_field();
    init_football();
    init_baseball();
    init_rugby();
    pthread_t *tid;
    long i;
    // allocate vector and initialize
    tid = (pthread_t *)malloc(sizeof(pthread_t)*(FOOTBALL_PLAYERS+BASEBALL_PLAYERS+RUGBY_PLAYERS));

    // create threads
    for (i = 0; i < FOOTBALL_PLAYERS; i++)
        pthread_create(&tid[i], NULL, footballPlayer, (void *)(i + 1));
    
    for (i = FOOTBALL_PLAYERS; i < FOOTBALL_PLAYERS + BASEBALL_PLAYERS; i++)
        pthread_create(&tid[i], NULL, baseballPlayer, (void *)(i-FOOTBALL_PLAYERS + 1));

    for (i = FOOTBALL_PLAYERS + BASEBALL_PLAYERS; i < FOOTBALL_PLAYERS + BASEBALL_PLAYERS +RUGBY_PLAYERS; i++)
        pthread_create(&tid[i], NULL, rugbyPlayer, (void *)(i - (FOOTBALL_PLAYERS + BASEBALL_PLAYERS) + 1));


    // wait for them to complete
    for (i = 0; i < FOOTBALL_PLAYERS+BASEBALL_PLAYERS+RUGBY_PLAYERS; i++)
        pthread_join(tid[i], NULL);

    printf("Exiting main program\n");

    return 0;
}