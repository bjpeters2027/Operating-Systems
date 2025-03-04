#include "disk.h"
#include "input.h"
#include "instructions.h"
#include "memsim.h"
#include <stdio.h>



int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: Bad input");
        return -1;
    }

    FILE* input = fopen(argv[1],"W");

    char* line;

    while (fscanf(input, "%s", &line) == 1){
        if(!Input_NextInstruction(line)){
            printf("Error: Bad input");
            break;
        }
    }

    
    return 0;
}