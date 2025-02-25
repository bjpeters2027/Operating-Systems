

void DISK_Init();
void DISK_LoadPage(int pid, int pa, char* page);
void DISK_getPage(int pid, int pa, char** page);
bool DISK_SwapPage(int from_pid, int from_pa, int with_pid, int with_pa, char **page);
  