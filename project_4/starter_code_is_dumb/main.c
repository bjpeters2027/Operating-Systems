#include "disk.h"


int main() {
    DISK_Init();

    char a[16] = {0};
    char b[16] = {0};
    a[15] = 0xF7;
    a[14] = 0x00;
    a[13] = 0xFF;
    b[2] = 0x00;
    b[1] = 0xFF;
    b[0] = 0x00;
    DISK_StorePage(0, 3, a);
    DISK_StorePage(1, 0, b);

    return -1;
}