#include "disk.h"
#include "input.h"
#include "instructions.h"
#include "memsim.h"
#include "pagetable.h"
#include <stdio.h>



int main(int argc, char **argv) {
    printf("Start\n");
    if (argc != 2) {
        printf("Error: Bad input");
        return -1;
    }

    FILE* input = fopen(argv[1],"r");
    if (!input) {
        printf("Error reading file: %s\n", argv[1]);
        return -1;
    }

    char line[MAX_GETLINE_CHARS];
    
    PT_Init();
    DISK_Init();
    while (fgets(line, sizeof(line), input)){
        line[strcspn(line, "\n")] = '\0';
        printf("Instruction?\t");
        if(!Input_NextInstruction(line)){
            printf("Error: Bad input\n");
            break;
        }
    }
    printf("End\n");
    DISK_Close();
    fclose(input);

    
    return 0;
}