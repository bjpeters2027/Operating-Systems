#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <assert.h>
#include "pagetable.h"
#include "memsim.h"

// Page table root pointer register values (one stored per process, would be "swapped in" to MMU with process, when scheduled)
typedef struct{
	int ptStartPA;
	bool present;
} ptRegister;

// Page table root pointer register values 
// One stored for each process, swapped in to MMU when process is scheduled to run)
ptRegister ptRegVals[NUM_PROCESSES]; 
int PT_Frame; // TODO: find way not to need to store value

typedef struct {
	char pfn;
	char bits;
} Entry;

typedef struct {
    Entry entries[NUM_PAGES];
    bool exists;
} PageTable;

bool entryWritable(Entry e) {
	return e.bits &= 0b00000010;
}
void entryWritable_set(Entry *e, bool v) {
	e->bits |= (v&1) << 1;
}

bool entryPhysPresent(Entry e) {
	return e.bits &= 0b00000001;
}

void entryPhysPresent_set(Entry *e, bool v) {
	e->bits |= (v&1);
}

bool entryExists(Entry e) {
	return e.bits &= 0b00000100;
}

void entryExists_set(Entry *e, bool v) {
	e->bits |= (v&1) << 2;
}


PageTable *getPageTable(int pid) {
    if (!PT_PageTableExists(pid)) return NULL;
    return ptRegVals[pid].ptStartPA + Memsim_GetPhysMem();
}

void PT_Init() {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        ptRegVals[i].ptStartPA = 0;
        ptRegVals[i].present = false;
    }
    PT_Frame = Memsim_FirstFreePFN();
}

bool PT_PageTableExists(int pid) {
    return ptRegVals[pid].present;
}

int PT_PageTableInit(int pid, int pageAddress) {
    PageTable* pt = (PT_Frame*PAGE_SIZE) + Memsim_GetPhysMem(); //Array of page tables
    int i = 0;
    for (; pt->exists && i < 4; i++, pt++); //First free space
    pt->exists = true;
    ptRegVals[pid].present = true;
    ptRegVals[pid].ptStartPA = (sizeof(PageTable) * i) + (PT_Frame*PAGE_SIZE);
}

void PT_SetWritable(int pid, int vpn, bool protection) {
    assert(PT_PageTableExists(pid)); 
    PageTable* pt = ptRegVals[pid].ptStartPA + Memsim_GetPhysMem();
    entryWritable_set(&pt->entries[vpn], protection);
}

void PT_AddEntry(int pid, int pageAddress, bool writable) {
    // Get page table for PID, create new one if doesn't exist
    PageTable* pt; //Array of page tables
    if (!PT_PageTableExists(pid)) { 
        PT_PageTableInit(pid,pageAddress);
    } else {
        PageTable* pt = ptRegVals[pid].ptStartPA + Memsim_GetPhysMem(); //Gets page table
    }

    if (entryExists(pt->entries[pageAddress])) {
        printf("Error: page already mapped");
        return;
    }
	entryExists_set(&pt->entries[pageAddress], true);
	entryWritable_set(&pt->entries[pageAddress], writable);
	
    int frame = Memsim_FirstFreePFN();

    if (frame == -1) {
        // TODO: kick out other page
    } else {
        pt->entries[pageAddress].pfn = frame;
    }
	
	entryPhysPresent_set(&pt->entries[pageAddress], true);
}

int getFrameAddr(int pid, int pa) {
    if (!PT_PageTableExists(pid)) return -1;
	PageTable* pt = ptRegVals[pid].ptStartPA + Memsim_GetPhysMem();
    if (!pt->exists) return -1;
	return pt->entries[pa].pfn;
}

int PT_VPNtoPA(int pid, int vpn) {
    int pageAddr = getFrameAddr(pid, VPN(vpn)) * PAGE_SIZE;
    if (pageAddr < 0) return pageAddr;
    return (pageAddr) + PAGE_OFFSET(vpn);
}

bool PT_PIDHasWritePerm(int pid, int vpn) {
    assert(PT_PageTableExists(pid));
}

bool PT_HasEntry(int pid, int vpn) {
    if (!PT_PageTableExists(pid)) return false;
    return entryExists(getPageTable(pid)->entries[vpn]);
}