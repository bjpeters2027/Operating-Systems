#include <stdio.h>
#include <stdlib.h>

// for accessing # pages, frame size & num processes
#include "memsim.h"
#include "pagetable.h"

#define PAGE_LOCATION(PID, PA) (PAGE_SIZE) * (PID * NUM_PAGES + PA)
#define PT_LOCATION(PID) PAGE_LOCATION(NUM_PROCESSES-1, NUM_PAGES-1) + (PAGE_SIZE * PID)


FILE *file;

#define FILENAME "disk.txt"

void write(int loc, char* val, int len) {
    fseek(file, loc, SEEK_SET);
    fwrite(val, sizeof(char), len, file);
}

void read(int loc, char* val, int len) {
    fseek(file, loc, SEEK_SET);
    fread(val, sizeof(char), len, file);
}

void DISK_Init() {
    file = fopen(FILENAME, "r+");
}

void DISK_Close() {
    fclose(file);
}

void DISK_StorePage(int pid, int pa, char* page) {
    write(PAGE_LOCATION(pid, pa), page, PAGE_SIZE);
}

void DISK_GetPage(int pid, int pa, char* page) {
    read(PAGE_LOCATION(pid, pa), page, PAGE_SIZE);
}

void DISK_StorePT(int pid, char* page) {
    write(PT_LOCATION(pid), page, PAGE_SIZE);
}

void DISK_GetPT(int pid, char* page) {
    read(PT_LOCATION(pid), page, PAGE_SIZE);
}

int DISK_PageGetOffset(int pid, int pa) {
    return PAGE_SIZE * (pid * NUM_PAGES + pa);
}

int DISK_PTGetOffset(int pid) {
    return DISK_PageGetOffset(NUM_PROCESSES-1, NUM_PAGES-1) + (PAGE_SIZE * pid);
}