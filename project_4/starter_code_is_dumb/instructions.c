#include <stdio.h>
#include <stdint.h>
#include "memsim.h"
#include "pagetable.h"

void Instruction_Map(int pid, int virtual_address, int value){
    if (!PT_PageTableExists(pid) || !PT_HasEntry(pid,VPN(virtual_address))) {
        PT_AddEntry(pid,virtual_address,value);
    } else {
        if (PT_PIDHasWritePerm(pid,VPN(virtual_address)) == (bool) value) {
            printf("Error: Virtual page already mapped into physical frame %i.\n", PT_GetFN(pid, VPN(virtual_address)));
        } else {
            PT_SetWritable(pid,VPN(virtual_address), (bool)value);
        }
    }
}

void Instruction_Store(int pid, int virtual_address, int value){
    char* physmem = Memsim_GetPhysMem();
    if (!PT_PageTableExists(pid) || !PT_PIDHasWritePerm(pid,VPN(virtual_address))) {
        
        printf("Error: Entry does not have write perms");
    } else {
        int pa = PT_VPNtoPA(pid,VPN(virtual_address));
        if(pa >= 0){
            physmem[pa] = value;
            pritnf("Stored value %i at virtual address %i (physical address %i)", value, virtual_address, pa);
        } else {
            printf("Error: Do not have access to that memory");
        }
    }
}


int Instruction_Load(int pid, int va){
    char* physmem = Memsim_GetPhysMem();
    if (!PT_PageTableExists(pid)) {
        printf("Error: PT no existo");
    } else {
        int pa = PT_VPNtoPA(pid,VPN(va));
        if(pa >= 0){
            int val = (int) physmem[pa];
            printf("The value %i was found at virtual address %i.", val, va);
            return val;
        } else {
            printf("Error: virtual address %i does not have write permissions.\n", va);
        }
    }
}