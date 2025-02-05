#ifndef FIELD
#define FIELD 1
#include <pthread.h>


typedef enum{
    Football = 0,
    Baseball = 1,
    Rugby = 2
} Sport;


typedef struct {
    pthread_mutex_t m;
    Sport lastPlayed;
    int gameInPlay;
    pthread_cond_t fieldReady;
} Field;

Field *getField();

#endif