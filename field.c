#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#define FOOTBALL_PLAYERS 44
#define BASEBALL_PLAYERS 36
#define RUGBY_PLAYERS 60

void footballPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am football player %ld of %d\n", tid, FOOTBALL_PLAYERS);
    return (NULL);
}

void baseballPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am baseball player %ld of %d\n", tid, BASEBALL_PLAYERS);
    return (NULL);
}

void rugbyPlayer(void *arg) {
    long tid = (long)arg;
    printf("Hello, I am rugby player %ld of %d\n", tid, RUGBY_PLAYERS);
    return (NULL);
}

main () {
    pthread_t *tid;
    // allocate vector and initialize
    tid = (pthread_t *)malloc(sizeof(pthread_t)*(FOOTBALL_PLAYERS+BASEBALL_PLAYERS+RUGBY_PLAYERS));

    // create threads
    for (int i = 0; i < FOOTBALL_PLAYERS; i++)
        pthread_create(&tid[i], NULL, footballPlayer, (void *)i);
    
    for (int i = 0; i < BASEBALL_PLAYERS; i++)
        pthread_create(&tid[i], NULL, baseballPlayer, (void *)i);

    for (int i = 0; i < RUGBY_PLAYERS; i++)
        pthread_create(&tid[i], NULL, rugbyPlayer, (void *)i);

    // wait for them to complete
    for (int i = 0; i < FOOTBALL_PLAYERS+BASEBALL_PLAYERS+RUGBY_PLAYERS; i++)
        pthread_join(tid[i], NULL);

    printf("Exiting main program\n");

    return 0;
}