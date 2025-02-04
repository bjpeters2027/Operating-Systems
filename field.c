#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <football.c>


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
