#include <pthread.h>
#include <semaphore.h>

#include "football.h"
#include "field.h"



struct { /* data shared by producer and consumer */
    pthread_mutex_t m;
    int onField; // Value of how many players to bring onto field (gets decremented to 0 as they enter)
    int offField; // Value of how many players to take off the field (gets decremented to 0 as the exit)
    pthread_cond_t startGame;
    pthread_cond_t finishGame;
    int inqueue;
    sem_t emptySpots;
    sem_t gameCaptain;
} football;


void init_football() {
    football.m = PTHREAD_MUTEX_INITIALIZER;
    // football.ready = 0;
    football.startGame = PTHREAD_COND_INITIALIZER;
    football.finishGame = PTHREAD_COND_INITIALIZER;
    int v = sem_init(&football.emptySpots, 0, FOOTBALL_PLAYER_CAP-1);
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
    printf("Football Player #%i (Captain): Waiting for the field to be open.\n", tid);
    pthread_mutex_lock(&getField()->m);
    while (getField()->gameInPlay || !football.get) {
        pthread_cond_wait(&getField()->fieldReady, &getField()->m);
    }
    getField()->gameInPlay = TRUE;
    pthread_mutex_unlock(&getField()->m);

    printf("Football Player #%ld (Captain): Moving players to field.\n", tid);
    pthread_mutex_lock(&football.m);
    football.onField = FOOTBALL_PLAYER_CAP-1;
    pthread_mutex_unlock(&football.m);
    pthread_cond_broadcast(&football.startGame);
    sem_post(&football.gameCaptain);

    sleep(FOOTBALL_GAME_TIME);

    printf("Football Player #%i (Captain): Finished game, moving off field.\n", tid);
    pthread_mutex_lock(&football.m);
    football.offField = FOOTBALL_PLAYER_CAP-1;
    pthread_mutex_unlock(&football.m);
    pthread_cond_broadcast(&football.finishGame);

    printf("Football Player #%i (Captain): Opening field for next game.\n", tid);
    pthread_mutex_lock(&getField()->m);
    getField()->gameInPlay = FALSE;
    pthread_mutex_unlock(&getField()->m);
}

void football_join_game(long tid) {
    // Enter queue and wait until next up to play
    printf("Football Player #%d: Entering queue.\n", tid);
    
    if (!sem_trywait(&football.gameCaptain)) {
        printf("Football Player #%i: I am the next game captain.\n", tid);
        football_run_game(tid);
    } else {
        sem_wait(&football.emptySpots);
    }
    
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
