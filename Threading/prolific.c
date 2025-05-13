#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(){
    // File Reading
    FILE* seed = fopen("seed.txt","r");
    int seet;
    fscanf(seed, "%d", &seet);
    printf(" Read seed value (converted to integer): %d\n", seet);
    printf(" \n");
    srand(seet);

    // Important variables
    int numOfChildren = 8 + (rand() % 6);
    int rands[numOfChildren];
    printf("Random Child Count: %d\n", numOfChildren);
    printf("I'm feeling prolific!\n");

    int i;
    for(i = 0;i < numOfChildren; i++){
        // Assigns array of random numbers
        rands[i] = rand();
    }

    // Counter
    int counter = -1;
    int j;

    for (j = 0; j < numOfChildren; j++) {

        int temp = fork();
        counter ++;

        if(temp != 0){ // Parent
            int exitCode;
            printf("[Parent]: I am waiting for PID %d to finish.\n", temp);
            waitpid(temp, &exitCode, 0);
            printf("[Parent]: Child %d finished with status code %d. Onward!\n", temp, WEXITSTATUS(exitCode));
        }else{ // Child
            int childPID = getpid();
            int childRand = rands[counter];
            int exitCode = ((childRand % 50) + 1);
            int waitTime = ((childRand % 3) + 1);
            printf("\t[Child, PID: %d]: I am the child and I will wait %d seconds and exit with code %d.\n", childPID, waitTime, exitCode);
            sleep(waitTime);
            printf("\t[Child, PID: %d]: Now exiting...\n", childPID);
            exit(exitCode);
        }
    }
}