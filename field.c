#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define FOOTBALL_PLAYERS 44
#define BASEBALL_PLAYERS 36
#define RUGBY_PLAYERS 60

void footballPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am football player %ld of %d\n", tid, nthreads);
    return (NULL);
}

void baseballPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am football player %ld of %d\n", tid, nthreads);
    return (NULL);
}

void rugbyPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am football player %ld of %d\n", tid, nthreads);
    return (NULL);
}

main () {
    // allocate vector and initialize
    tid = (pthread_t *)malloc(sizeof(pthread_t)*(FOOTBALL_PLAYERS+BASEBALL_PLAYERS+RUGBY_PLAYERS));

    // create threads
    for ( i = 0; i < FOOTBALL_PLAYERS; i++)
        pthread_create(&tid[i], NULL, footballPlayer, (void *)i);
    
    for ( i = 0; i < BASEBALL_PLAYERS; i++)
        pthread_create(&tid[i], NULL, footballPlayer, (void *)i);

    for ( i = 0; i < RUGBY_PLAYERS; i++)
        pthread_create(&tid[i], NULL, footballPlayer, (void *)i);

    // wait for them to complete
    for ( i = 0; i < nthreads; i++)
    pthread_join(tid[i], NULL);

    printf("Exiting main program\n");

    return 0;
}