#include <stdbool.h>

#define NUM_PROCESSES 4

/*
* Public Interface:
*/
void PT_Init();
bool PT_PageTableExists(int pid);
int PT_PageTableInit(int pid, int pageAddress);
void PT_SetWritable(int pid, int vpn, bool protection);
void PT_AddEntry(int pid, int pageAddress, bool writable);
//  int PT_GetRootPtrRegVal(int process_id);
//  int PT_Evict();
int PT_VPNtoPA(int pid, int VPN);
bool PT_PIDHasWritePerm(int pid, int vpn);
bool PT_HasEntry(int pid, int vpn);