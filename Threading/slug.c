#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    // Reading file
    int input = atoi(argv[1]);
    char* slugFile = malloc(sizeof(char) * 256);
    sprintf(slugFile, "seed_slug_%d.txt", input);
    FILE* slug = fopen(slugFile, "r");
    int seed;
    fscanf(slug, "%d", &seed);
    srand(seed);
    fclose(slug);
    free(slugFile);

    // Randoms
    int seconds = 2 + rand() % 5;
    int coin = rand() % 2;
   
    // Important strings
    char *faces[2][4] = {{"last", "-i", "-x", NULL}, {"id", "--group", NULL}};
    char *heads = "last -i -x";
    char *tails = "id -group";
    char *format[] = {heads, tails};

    // Pre-prints
    printf("[Slug PID: %d] Read seed value: %d", getpid(), seed);
    printf(" \n");
    printf("[Slug PID: %d] Read seed value (converted to integer): %d\n", getpid(), seed);
    printf("[Slug PID: %d] Delay time is %d seconds. Coin flip: %d\n", getpid(), seconds, coin);
    printf("[Slug PID: %d] I,ll get the job done. Eventually...\n", getpid());

    // Actual work
    sleep(seconds);
    printf("[Slug PID: %d] Break time is over! I am running the \'%s\' command.\n", getpid(),format[coin]);
    execvp(faces[coin][0], faces[coin]);
}