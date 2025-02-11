#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Job{
    int id;
    int len;
    int timeRemaning;
    int resTime;
    int turn;
    int waitTime;
    bool first;
}Job;


void processJob(Job j) {
    printf("Job %i ran for: %i\n", j.id, j.len);
}

void analysisTime(Job* j, int len){
    float totRes = 0;
    float totTurn = 0;
    float totWait = 0;
    for (int i = 0; i < len; i++) {
        printf("Job %i -- Response time: %i  Turnaround: %i  Wait: %i\n", j[i].id, j[i].resTime, j[i].turn, j[i].waitTime);
        totRes += j[i].resTime;
        totTurn += j[i].turn;
        totWait += j[i].waitTime;
    }
    printf("Average -- Response: %.2f  Turnaround: %.2f  Wait: %.2f\n", (totRes / len), (totTurn / len), (totWait / len));
}

void fifo(FILE *input) {
    int fileLen = 0, num;
    while (fscanf(input, "%d", &num) == 1) {
        fileLen++;
    }

    rewind(input);

    Job* jobs = malloc(sizeof(Job) * fileLen);
    int next;
    for (int i = 0; fscanf(input, "%d", &next) == 1; i++) {
        jobs[i] = (Job){ .id = i, .len = next};
    }

    int clock = 0;

    printf("Execution trace with FIFO:\n");
    for (int i = 0; i < fileLen; i++){
        processJob(jobs[i]);
        jobs[i].resTime = clock;
        clock += jobs[i].len;
        jobs[i].turn = clock;
        jobs[i].waitTime = jobs[i].resTime;
    }
    printf("End of execution with FIFO.\n");

    printf("Begin analyzing FIFO:\n");
    analysisTime(jobs, fileLen);
    printf("End analyzing FIFO.\n");
    
    free(jobs);
}

void sjf(FILE* input) {
    int fileLen = 0, num;
    while (fscanf(input, "%d", &num) == 1) {
        fileLen++;
    }

    rewind(input);

    Job* jobs = malloc(sizeof(Job) * fileLen);
    int next;
    for (int i = 0; fscanf(input, "%d", &next) == 1; i++) {
        jobs[i] = (Job){ .id = i, .len = next};
    }

    for (int i = 0; i < fileLen - 1; i++) {
        for (int j = 0; j < fileLen - i - 1; j++) {
            if (jobs[j].len > jobs[j + 1].len) {
                Job temp = jobs[j];
                jobs[j] = jobs[j + 1];
                jobs[j + 1] = temp;
            }
        }
    }

    int clock = 0;

    printf("Execution trace with SJF:\n");
    for (int i = 0; i < fileLen; i++){
        processJob(jobs[i]);
        jobs[i].resTime = clock;
        clock += jobs[i].len;
        jobs[i].turn = clock;
        jobs[i].waitTime = jobs[i].resTime;
    }
    printf("End of execution with SJF.\n");

    printf("Begin analyzing SJF:\n");
    analysisTime(jobs, fileLen);
    printf("End analyzing SJF.\n");

    free(jobs);
}

void rr(FILE* input, int time) {
    int fileLen = 0, num;
    while (fscanf(input, "%d", &num) == 1) {
        fileLen++;
    }

    rewind(input);

    Job* jobs = malloc(sizeof(Job) * fileLen);
    int next;
    for (int i = 0; fscanf(input, "%d", &next) == 1; i++) {
        jobs[i] = (Job){ .id = i, .len = next, .timeRemaning = next, .resTime = -1, .turn = -1, .waitTime = -1, .first = true};
    }
    int clock = 0;
    int numDone = 0;

    printf("Execution trace with RR:\n");
    while (numDone < fileLen){
        for (int i = 0; i < fileLen; i++) {
            if (jobs[i].timeRemaning == -1){
                continue;
            }else if (jobs[i].timeRemaning <= time) {
                printf("Job %i ran for: %i\n", jobs[i].id, jobs[i].timeRemaning);
                if(jobs[i].first){
                    jobs[i].resTime = clock;
                    jobs[i].first = false;
                }
                clock += jobs[i].timeRemaning;
                jobs[i].turn = clock;
                jobs[i].waitTime = jobs[i].turn - jobs[i].len;
                jobs[i].timeRemaning = -1;
                numDone++;
            }else{
                printf("Job %i ran for: %i\n", jobs[i].id, time);
                if(jobs[i].first){
                    jobs[i].resTime = clock;
                    jobs[i].first = false;
                }
                clock += time;
                jobs[i].timeRemaning -= time;
            }
        }
    }
    printf("End of execution with RR.\n");

    printf("Begin analyzing RR:\n");
    analysisTime(jobs, fileLen);
    printf("End analyzing RR.\n");

    free(jobs);
}

int main(int argc, char **argv){
    if (argv[2] == NULL) return -1;
    FILE *input = fopen(argv[2], "r");
    if (input == NULL) {
        printf("ERROR OPENING FILE\n");
        return -1;
    }
    switch (argv[1][0]) {
        case 'F':
            fifo(input);
        break;

        case 'S':
            sjf(input);
        break;

        case 'R':
            rr(input, atoi(argv[3]));
        break;

        default:
        printf("ERROR SCHEDULER TYPE\n");
        return -1;
    }
    

}