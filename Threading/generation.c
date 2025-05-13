#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

void createFamily(int lifespan);

int main(){
    // File Reading
    FILE* seed = fopen("seed.txt","r");
    int seet;
    fscanf(seed, "%d", &seet);
    printf("Read seed value (converted to integer): %d\n", seet);
    printf(" \n");
    srand(seet);

    // Important variables
    int lifespan = 5 + (rand() % 8);

    printf("Random Descendant Count: %d\n", lifespan);
    printf("Time to meet the kids/grandkids/great grand kids/...\n");
    
    createFamily(lifespan);
}

// Recursive function
void createFamily(int lifespan){
    // Base case
    if (lifespan <= 0) {
        exit(0);
    }
    int spoon = fork(); 
    if (spoon > 0) {
        int exitCode;
        printf("[Parent, PID: %d]: I am waiting for PID %d to finish.\n", getpid(), spoon);
        waitpid(spoon, &exitCode, 0);
        printf("[Parent, PID: %d]: Child %d finished with status code %d. It's now my turn to exit.\n", getpid(), spoon, WEXITSTATUS(exitCode));
        // Increment exit code
        exit(WEXITSTATUS(exitCode) + 1);
    } else{
        printf("\t[Child, PID: %d]: I was called with lifespan=%d. I'll have %d descendant(s).\n", getpid(), lifespan, lifespan - 1);
        // Recursive call
        createFamily(lifespan - 1);
    }
}