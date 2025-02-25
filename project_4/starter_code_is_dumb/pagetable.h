#include <stdbool.h>

#define NUM_PROCESSES 4

/*
* Public Interface:
*/
void PT_Init();
int PT_PageTableExists(int pid);
int PT_PageTableInit(int pid, int pageAddress);
void PT_SetWritable(int pid, int VPN, bool protection);
//  int PT_GetRootPtrRegVal(int process_id);
//  int PT_Evict();
int PT_VPNtoPA(int pid, int VPN);
int PT_PIDHasWritePerm(int pid, int vpn);