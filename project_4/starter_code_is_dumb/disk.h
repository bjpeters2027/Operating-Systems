#include <stdbool.h>

void DISK_Init();
void DISK_Close();
void DISK_StorePage(int pid, int pa, char* page);
void DISK_GetPage(int pid, int pa, char* page);