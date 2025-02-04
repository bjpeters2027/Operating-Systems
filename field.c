#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>


#define FOOTBALL_PLAYERS 44
#define FOOTBALL_PLAYER_CAP 22
#define FOOTBALL_GAME_TIME 5

#define BASEBALL_PLAYERS 36
#define RUGBY_PLAYERS 60

void player_sleep() {
    sleep(rand()%10 + 0);
}

typedef enum{
    Football = 0,
    Baseball = 1,
    Rugby = 2
} Sport;


struct{
    Sport lastPlayed;
    pthread_cond_t sportReady;
} field;

void init_field() {
    field.lastPlayed = 0;
    field.sportReady = PTHREAD_COND_INITIALIZER;
}

struct { /* data shared by producer and consumer */
    pthread_mutex_t m;
    int onField; // Value of how many players to bring onto field (gets decremented to 0 as they enter)
    int offField; // Value of how many players to take off the field (gets decremented to 0 as the exit)
    pthread_cond_t startGame;
    pthread_cond_t finishGame;
    int inqueue;
    sem_t emptySpots;
} football;

void init_football() {
    football.m = PTHREAD_MUTEX_INITIALIZER;
    // football.ready = 0;
    football.startGame = PTHREAD_COND_INITIALIZER;
    football.finishGame = PTHREAD_COND_INITIALIZER;
    int v = sem_init(&football.emptySpots, 0, FOOTBALL_PLAYER_CAP);
    printf("Return of sem_init: %d\n", v);
}

void football_run_game() {
    printf("Starting football game\n");
    pthread_mutex_lock(&football.m);
    football.onField = FOOTBALL_PLAYER_CAP;
    pthread_mutex_unlock(&football.m);
    pthread_cond_broadcast(&football.startGame);

    sleep(FOOTBALL_GAME_TIME);

    printf("FInished football game\n");
    pthread_mutex_lock(&football.m);
    football.offField = FOOTBALL_PLAYER_CAP;
    pthread_mutex_unlock(&football.m);
    pthread_cond_broadcast(&football.finishGame);
}

int football_ready() {
    int queue;
    sem_getvalue(&football.emptySpots, &queue);
    printf("Spots left in queue: %d, people in queue: %d \n", queue, football.inqueue);
    return queue <= 0;
}

void football_join_game(long tid) {
    // Enter queue and wait until next up to play
    printf("Football Player #%d: Entering queue.\n", tid);
    
    sem_wait(&football.emptySpots);
    
    // Wait for next football game to start
    printf("Football Player #%d: Next up to play football.\n", tid);
    pthread_mutex_lock(&football.m);
    while (football.onField == 0)
        pthread_cond_wait(&football.startGame, &football.m);
    football.onField--;
    pthread_mutex_unlock(&football.m);
    sem_post(&football.emptySpots);

    // Start game and wait for it to finish
    printf("Football Player #%d: Entering the field.\n", tid);
    pthread_mutex_lock(&football.m);
    while (football.offField == 0)
        pthread_cond_wait(&football.finishGame, &football.m);
    football.offField--;
    pthread_mutex_unlock(&football.m);
}

void *footballPlayer(void *arg) {
    long tid = (long)arg;
    srand(time(NULL));

    while (1) {
        player_sleep();
        football_join_game(tid);
        printf("Football Player #%d: Finished a game.\n", tid);
    }
    
    return(NULL);
}

void *baseballPlayer(void *arg) {
    long tid = (long)arg;
    return(NULL);
}

void *rugbyPlayer(void *arg) {
    long tid = (long)arg;
    return(NULL);
}

int main () {    
    init_field();
    init_football();
    pthread_t *tid;
    long i;
    printf("Size in bytes: %d", sizeof(football));
    // allocate vector and initialize
    tid = (pthread_t *)malloc(sizeof(pthread_t)*(FOOTBALL_PLAYERS+BASEBALL_PLAYERS+RUGBY_PLAYERS));

    // create threads
    for (i = 0; i < FOOTBALL_PLAYERS; i++)
        pthread_create(&tid[i], NULL, footballPlayer, (void *)(i + 1));
    
    // for (i = FOOTBALL_PLAYERS; i < FOOTBALL_PLAYERS + BASEBALL_PLAYERS; i++)
    //     pthread_create(&tid[i], NULL, baseballPlayer, (void *)(i-FOOTBALL_PLAYERS + 1));

    // for (i = FOOTBALL_PLAYERS + BASEBALL_PLAYERS; i < FOOTBALL_PLAYERS + BASEBALL_PLAYERS +RUGBY_PLAYERS; i++)
    //     pthread_create(&tid[i], NULL, rugbyPlayer, (void *)(i - (FOOTBALL_PLAYERS + BASEBALL_PLAYERS) + 1));

    while (1) {
        while (!football_ready()) sleep(1);
        football_run_game();
    }

    // wait for them to complete
    for (i = 0; i < FOOTBALL_PLAYERS+BASEBALL_PLAYERS+RUGBY_PLAYERS; i++)
        pthread_join(tid[i], NULL);

    printf("Exiting main program\n");

    return 0;
}
