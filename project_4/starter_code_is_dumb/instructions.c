#include <stdio.h>
#include <stdint.h>
#include "memsim.h"
#include "pagetable.h"
#include "mmu.h"

void Instruction_Map(int pid, int virtual_address, int value){
    if (!PT_PageTableExists(pid) || !PT_HasEntry(pid,VPN(virtual_address))) {
        PT_AddEntry(pid,VPN(virtual_address),value);
    } else {
        if (PT_PIDHasWritePerm(pid,VPN(virtual_address)) == (bool) value) {
            printf("Error: This entry already exists");
        } else {
            PT_SetWritable(pid,VPN(virtual_address), (bool)value);
        }
    }
}

void Instruction_Store(pid, virtual_address, value){
    char* physmem = Memsim_GetPhysMem();
    if (!PT_PIDHasWritePerm(pid,VPN(virtual_address))) {
        printf("Error: Entry does not have write perms");

    } else {
        int pa = PT_VPNtoPA(pid,VPN(virtual_address));
        if(pa >= 0){
            physmem[pa] = value;
        } else {
            printf("Error: Do not have access to that memory")
        }
    }
}


int Instruction_Load(int pid, int va){
    if (!PT_PageTableExists(pid)) {
        printf("Error: PT no existo");
    } else {
        
    }
}