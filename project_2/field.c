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
    sem_t emptySpots;
    sem_t gameCaptain;
} rugby;

void init_field() {
    field.currentSport = rand()%3;
    pthread_cond_init(&field.fieldReady, NULL);
    field.gameInPlay = false;
}

Sport nextSport() {
    return (field.currentSport + 1) % 3;
}

void player_sleep() {
    usleep((rand() % 500) * 10000 + 0);
}

void football_sleep() {
    sleep(rand() % 3 + 5);
}

void baseball_sleep() {
    sleep(rand() % 3 + 5);
}

void rugby_sleep() {
    sleep(rand() % 3 + 5);
}

void init_player(long int tid) {
    struct timeval t;
    gettimeofday(&t, NULL);
    srand(t.tv_usec);
}

// Football player ------------------------------------------------------------

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
    printf("Spots left in queue: %d, people in queue: %d \n", queue, football.inqueue);
    return queue <= 0;
}

void football_run_game(long tid) {
    // Wait untill enough players read to play a game then until the field is clear and football is the next sport to play
    printf("Football Player #%li (Captain): Waiting for the field to be open and players to be ready.\n", tid);
    pthread_mutex_lock(&field.m);
    while (!football_ready()) {sleep(1);}
    while (field.gameInPlay || nextSport() != Football) {
        pthread_cond_wait(&field.fieldReady, &field.m);
    }
    field.gameInPlay = true;
    pthread_mutex_unlock(&field.m);

    // Move players onto feild and set current sport to football
    printf("Football Player #%ld (Captain): Moving players to field.\n", tid);
    sem_post(&football.gameCaptain);
    pthread_mutex_lock(&football.m);
    football.onField = FOOTBALL_PLAYER_CAP;
    pthread_mutex_unlock(&football.m);
    pthread_cond_broadcast(&football.startGame);
    pthread_mutex_lock(&field.m);
    field.currentSport = Football;
    pthread_mutex_unlock(&field.m);

    
    // Wait for the football game to be over
    football_sleep();

    // Take the players of the field 
    printf("Football Player #%li (Captain): Finished game, moving players off the field.\n", tid);
    pthread_mutex_lock(&football.m);
    football.offField = FOOTBALL_PLAYER_CAP;
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
    // Enter queue and wait until next up to play
    printf("Football Player #%ld: Entering queue.\n", tid);
    
    sem_wait(&football.emptySpots);

    if (!sem_trywait(&football.gameCaptain)) {
        printf("Football Player #%li: I am the next game captain.\n", tid);
        return football_run_game(tid);
    }
    
    // Wait for next football game to start
    printf("Football Player #%ld: Next up to play football.\n", tid);
    pthread_mutex_lock(&football.m);
    while (football.onField == 0)
        pthread_cond_wait(&football.startGame, &football.m);
    football.onField--;
    pthread_mutex_unlock(&football.m);
    sem_post(&football.emptySpots);

    // Start game and wait for it to finish
    printf("Football Player #%ld: Entering the field.\n", tid);
    pthread_mutex_lock(&football.m);
    while (football.offField == 0)
        pthread_cond_wait(&football.finishGame, &football.m);
    football.offField--;
    pthread_mutex_unlock(&football.m);
    printf("Football Player #%ld: Finished a game.\n", tid);
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


// Baseball player ------------------------------------------------------------

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
    printf("Spots left in queue: %d, people in queue: %d \n", queue, baseball.inqueue);
    return queue <= 0;
}

void baseball_run_game(long tid) {
    // Wait untill enough players read to play a game then until the field is clear and baseball is the next sport to play
    printf("Baseball Player #%li (Captain): Waiting for the field to be open and players to be ready.\n", tid);
    pthread_mutex_lock(&field.m);

    while (!baseball_ready()) {sleep(1);}
    while (field.gameInPlay || nextSport() != Baseball) {
        pthread_cond_wait(&field.fieldReady, &field.m);
    }
    field.gameInPlay = true;
    pthread_mutex_unlock(&field.m);

    // Move players onto feild and set current sport to baseball
    printf("Baseball Player #%ld (Captain): Moving players to field.\n", tid);
    sem_post(&baseball.gameCaptain);
    pthread_mutex_lock(&baseball.m);
    baseball.onField = BASEBALL_PLAYER_CAP;
    pthread_mutex_unlock(&baseball.m);
    pthread_cond_broadcast(&baseball.startGame);
    pthread_mutex_lock(&field.m);
    field.currentSport = Baseball;
    pthread_mutex_unlock(&field.m);

    
    // Wait for the baseball game to be over
    baseball_sleep();

    // Take the players of the field 
    printf("Baseball Player #%li (Captain): Finished game, moving players off the field.\n", tid);
    pthread_mutex_lock(&baseball.m);
    baseball.offField = BASEBALL_PLAYER_CAP;
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
    printf("Baseball Player #%ld: Entering queue.\n", tid);
    
    sem_wait(&baseball.emptySpots);

    if (!sem_trywait(&baseball.gameCaptain)) {
        printf("Baseball Player #%li: I am the next game captain.\n", tid);
        return baseball_run_game(tid);
    }
    
    // Wait for next baseball game to start
    printf("Baseball Player #%ld: Next up to play baseball.\n", tid);
    pthread_mutex_lock(&baseball.m);
    while (baseball.onField == 0)
        pthread_cond_wait(&baseball.startGame, &baseball.m);
    baseball.onField--;
    pthread_mutex_unlock(&baseball.m);
    sem_post(&baseball.emptySpots);

    // Start game and wait for it to finish
    printf("Baseball Player #%ld: Entering the field.\n", tid);
    pthread_mutex_lock(&baseball.m);
    while (baseball.offField == 0)
        pthread_cond_wait(&baseball.finishGame, &baseball.m);
    baseball.offField--;
    pthread_mutex_unlock(&baseball.m);
    printf("Baseball Player #%ld: Finished a game.\n", tid);
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


// Rugby ----------------------------------------------------------------------


void init_rugby() {
    pthread_mutex_init(&rugby.m, NULL);
    rugby.onField = 0;
    rugby.offField = 0;
    pthread_cond_init(&rugby.startGame, NULL);
    pthread_cond_init(&rugby.finishGame, NULL);
    int v = sem_init(&rugby.emptySpots, 0, RUGBY_PLAYER_CAP);
    sem_init(&rugby.gameCaptain, 0, 1);
    printf("Return of sem_init: %d\n", v);
}

int rugby_ready() {
    int queue;
    sem_getvalue(&rugby.emptySpots, &queue);
    printf("Spots left in queue: %d, people in queue: %d \n", queue, rugby.inqueue);
    return queue <= 0;
}

void rugby_run_game(long tid) {
    // Wait untill enough players read to play a game then until the field is clear and rugby is the next sport to play
    printf("Rugby Player #%li (Captain): Waiting for the field to be open and players to be ready.\n", tid);
    pthread_mutex_lock(&field.m);
    while (!rugby_ready()) {sleep(1);}
    while (field.gameInPlay || nextSport() != Rugby) {
        pthread_cond_wait(&field.fieldReady, &field.m);
    }
    field.gameInPlay = true;
    pthread_mutex_unlock(&field.m);

    // Move players onto feild and set current sport to rugby
    printf("Rugby Player #%ld (Captain): Moving players to field.\n", tid);
    sem_post(&rugby.gameCaptain);
    pthread_mutex_lock(&rugby.m);
    rugby.onField = RUGBY_PLAYER_CAP;
    pthread_mutex_unlock(&rugby.m);
    pthread_cond_broadcast(&rugby.startGame);
    pthread_mutex_lock(&field.m);
    field.currentSport = Rugby;
    pthread_mutex_unlock(&field.m);

    
    // Wait for the rugby game to be over
    rugby_sleep();

    // Take the players of the field 
    printf("Rugby Player #%li (Captain): Finished game, moving players off the field.\n", tid);
    pthread_mutex_lock(&rugby.m);
    rugby.offField = RUGBY_PLAYER_CAP;
    pthread_mutex_unlock(&rugby.m);
    pthread_cond_broadcast(&rugby.finishGame);

    // Broadcast that the field is now ready for the next sport to play
    printf("Rugby Player #%li (Captain): Opening field for next game.\n", tid);
    pthread_mutex_lock(&field.m);
    field.gameInPlay = false;
    pthread_mutex_unlock(&field.m);
    pthread_cond_broadcast(&field.fieldReady);
}

void rugby_join_game(long tid) {
    // Enter queue and wait until next up to play
    printf("Rugby Player #%ld: Entering queue.\n", tid);
    
    sem_wait(&rugby.emptySpots);

    if (!sem_trywait(&rugby.gameCaptain)) {
        printf("Rugby Player #%li: I am the next game captain.\n", tid);
        return rugby_run_game(tid);
    }
    
    // Wait for next rugby game to start
    printf("Rugby Player #%ld: Next up to play rugby.\n", tid);
    pthread_mutex_lock(&rugby.m);
    while (rugby.onField == 0)
        pthread_cond_wait(&rugby.startGame, &rugby.m);
    rugby.onField--;
    pthread_mutex_unlock(&rugby.m);
    sem_post(&rugby.emptySpots);

    // Start game and wait for it to finish
    printf("Rugby Player #%ld: Entering the field.\n", tid);
    pthread_mutex_lock(&rugby.m);
    while (rugby.offField == 0)
        pthread_cond_wait(&rugby.finishGame, &rugby.m);
    rugby.offField--;
    pthread_mutex_unlock(&rugby.m);
    printf("Rugby Player #%ld: Finished a game.\n", tid);
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