#include <stdbool.h>

void DISK_Init();
void DISK_Close();
void DISK_StorePage(int pid, int pa, char* page);
void DISK_GetPage(int pid, int pa, char* page);
void DISK_StorePT(int pid, char* page);
void DISK_GetPT(int pid, char* page);
int DISK_PTGetOffset(int pid);
int DISK_PageGetOffset(int pid, int pa);