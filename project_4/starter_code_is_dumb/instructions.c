#include <stdio.h>
#include <stdint.h>
#include "memsim.h"
#include "pagetable.h"
#include "disk.h"

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
    uint8_t* physmem = Memsim_GetPhysMem();
    PageTable *pt = getPageTable(pid);
    printf("%i",pt->entries[VPN(virtual_address)].bits);
    if (!PT_PageTableExists(pid) || !PT_PIDHasWritePerm(pid,VPN(virtual_address))) {
        printf("Error: virtual address %i does not have write permissions.\n", virtual_address);
    } else {
        int pa = PT_VPNtoPA(pid,virtual_address);
        if(pa >= 0){
            physmem[pa] = value;
            printf("Stored value %u at virtual address %i (physical address %i)\n", physmem[pa], virtual_address, pa);
        } else {
            printf("Error: Do not have access to that memory\n");
        }
    }
}


int Instruction_Load(int pid, int va){
    uint8_t* physmem = Memsim_GetPhysMem();
    if (!PT_PageTableExists(pid)) {
        printf("Error: PT no existo\n");
    } else {
        int pa = PT_VPNtoPA(pid,va);
        if(pa >= 0){
            int val = (int) physmem[pa];
            printf("Loaded the value %i from virtual address %i.\n", (unsigned int)val, va);
            return val;
        } else {
            printf("Error: virtual address %i does not have write permissions.\n", va);
        }
    }
}