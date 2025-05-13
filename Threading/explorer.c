#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(){
    // Reading file
    FILE* seed = fopen("seed.txt","r");
    int seet;
    fscanf(seed, "%d", &seet);
    printf("Read seed value (converted to integer): %d\n", seet);
    printf(" \n");
    srand(seet);
    fclose(seed);
    printf("It's time to see the world/file system!\n");

    // Important strings
    char *dirs[] = {"/home", "/proc", "/proc/sys", "/usr", "/usr/bin", "/bin"};
    char *cwd = malloc(256);
    char *childArgs[] = {"ls", "-tr", NULL};

    // Main Loop
    for (int i = 0; i < 5; i++) {
        int dirInt = rand() % 6;

        chdir(dirs[dirInt]);
        printf("Selection #%d: %s [SUCCESS]\n",(i + 1), dirs[dirInt]);
        printf("Current reported directory: %s\n", getcwd(cwd, 256));
        int spoon = fork();
        if (spoon > 0) { // Parent
            printf("[Parent]: I am waiting for PID %d to finish.\n", spoon);
            int exitCode;
            waitpid(spoon, &exitCode, 0);
            printf("[Parent]: Child %d finished with status code %d. Onward!.\n", spoon, WEXITSTATUS(exitCode));
        }else if (spoon == 0) { // Child
            printf("[Child, PID: %d]: Executing ,ls-tr, command...", getpid());
            execvp(childArgs[0], childArgs);
        }

    }
    free(cwd);
}