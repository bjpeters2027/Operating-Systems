#include <stdio.h>

struct Job {
    int id;
    int len;
};

void processJob(struct Job j) {
    printf("Job %i ran for: %i\n", j.id, j.len);
}

void fifo(FILE *input) {
    int next;
    printf("Execution trace with FIFO:\n");
    for (int i = 0; fscanf(input, "%d", &next) == 1; i++) {
        processJob((struct Job){.id = i, .len = next});
    }
    printf("End of execution with FIFO.\n");
}

int main(int argc, char **argv){
    if (argv[1] == NULL) return -1;
    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("ERROR OPENING FILE\n");
        return -1;
    }
    fifo(input);

}