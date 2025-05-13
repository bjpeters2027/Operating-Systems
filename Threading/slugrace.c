#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>


// Function to calculate elapsed time in seconds and milliseconds
double calculate_elapsed_time(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9 // 1 billion nanoseconds in a second;
}

int main() {
    // Important stuff
    struct timespec start_time, current_time;
    int spoons[4];
    int completed_slugs = 0;

    // Record the start time
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    for (int i = 0; i < 4; i++) {
        int spoon = fork();

        if (spoon == 0) { // Child process
            char slug_num[10];
            sprintf(slug_num, "%d", i + 1);

            printf("\t[Child, spoon: %d]: Executing './slug %s' command...\n", getpid(), slug_num);
            execl("./slug", "./slug", slug_num, (char *)NULL);
        } else { // Parent process
            spoons[i] = spoon;
            printf("[Parent]: I forked off child %d.\n", spoon);
        }
    }

    while (completed_slugs < 4) {
        // Check the status of each child
        for (int i = 0; i < 4; i++) {
            if (spoons[i] != -1) {
                int status;
                int result = waitpid(spoons[i], &status, WNOHANG);

                if (result > 0) {
                    // Child has finished
                    clock_gettime(CLOCK_MONOTONIC, &current_time);
                    double elapsed_time = calculate_elapsed_time(start_time, current_time);

                    printf("Child %d has crossed the finish line! It took %.6f seconds\n", spoons[i], elapsed_time);
                    spoons[i] = -1; // Mark this child as completed
                    completed_slugs++;
                }
            }
        }

        // Print race status
        if (completed_slugs < 4) {
            printf("The race is ongoing. The following children are still racing:");
            for (int i = 0; i < 4; i++) {
                if (spoons[i] != -1) {
                    printf(" %d", spoons[i]);
                }
            }
            printf("\n");

            usleep(330000);
        }
    }

    // End of race
    printf("The race is over! It took %.6f seconds\n", calculate_elapsed_time(start_time, current_time));
    return 0;
}