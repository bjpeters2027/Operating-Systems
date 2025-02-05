#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>


typedef enum{
    Football = 0,
    Baseball = 1,
    Rugby = 2
} Sport;


struct{
    Sport lastPlayed;
    pthread_cond_t sportReady;
} field;

void player_sleep();

Field *getField();
