#include <stdio.h>
#include <stdlib.h>

// for accessing # pages, frame size & num processes
#include "memsim.h"
#include "pagetable.h"

#define LOCATION(PID, PA) (PAGE_SIZE * 8) * (PID * NUM_PAGES + PA)


FILE *file;

#define FILENAME "disk.txt"

void write(int loc, char* val, int len) {
    fseek(file, loc, SEEK_SET);
    char *tmp = malloc(sizeof(char)*8*len);
    for (int i = 0; i < len*8; i++) {
        tmp[i] = '0'+ !!(val[i/8] & (0b10000000 >> i%8));
    }
    fwrite(tmp, sizeof(char), len*8, file);
}

void read(int loc, char* val, int len) {
    fseek(file, loc, SEEK_SET);
    char *tmp = malloc(sizeof(char)*8*len);
    fread(tmp, sizeof(char), len*8, file);
    for (int i = 0; i < len; i++) {
        val[i] = 0;
        for (int b = 0; b < 8; b++) 
            val[i] |= (tmp[i*8+b]-'0') << (7-b);
    }
}

void DISK_Init() {
    file = fopen(FILENAME, "r+");
}
void DISK_Close() {
    fclose(file);
}

void DISK_StorePage(int pid, int pa, char* page) {
    write(LOCATION(pid, pa), page, PAGE_SIZE);
}

void DISK_GetPage(int pid, int pa, char* page) {
    read(LOCATION(pid, pa), page, PAGE_SIZE);
}