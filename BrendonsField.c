#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define FOOTBALL_PLAYERS 44
#define BASEBALL_PLAYERS 36
#define RUGBY_PLAYERS 60

typedef enum{
    FOOTBALL,
    BASEBALL,
    RUGBY
}Sport;


typedef struct{
    Sport currentSport;
    int requiredPlayers;
    pthread_t *players;
}Field;

typedef struct{
    Sport sport;
    int number;
}Player;

Field initField(Sport sport, pthread_t *tid){
    //Determine how may rugby players needed
    Field field;
    FILE* seed = fopen("seed.txt","r");
    int seet;
    fscanf(seed, "%d", &seet);
    srand(seet);
    fclose(seed);
    int rugbyPlayers = 2 * (rand() % 15 + 1);

    field.currentSport = sport;
    if(sport == FOOTBALL){
        field.requiredPlayers = 18;
    }else if(sport == BASEBALL){
        field.requiredPlayers = 22;
    }else if(sport == RUGBY){
        field.requiredPlayers = rugbyPlayers;
    }
    field.players = tid;
    return field;
}

void *footballPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am football player %ld of %d\n", tid, FOOTBALL_PLAYERS);
    return(NULL);
}

void *baseballPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am baseball player %ld of %d\n", tid, BASEBALL_PLAYERS);
    return(NULL);
}

void *rugbyPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am rugby player %ld of %d\n", tid, RUGBY_PLAYERS);
    return(NULL);
}

int main () {    
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
