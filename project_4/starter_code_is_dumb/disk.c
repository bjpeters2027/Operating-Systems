#include <stdio.h>

FILE *file;

#define FILENAME "disk.txt"

void write(int loc, char* val, int len) {
    fseek(file, loc, SEEK_SET);
}

void DISK_Init() {
    fopen(FILENAME, "r+");


}


