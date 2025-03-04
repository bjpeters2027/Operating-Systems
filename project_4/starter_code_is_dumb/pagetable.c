#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <assert.h>
#include "pagetable.h"
#include "memsim.h"
#include "disk.h"

// Page table root pointer register values (one stored per process, would be "swapped in" to MMU with process, when scheduled)
typedef struct{
	int ptStartPA;
	bool present;
} ptRegister;

// Page table root pointer register values 
// One stored for each process, swapped in to MMU when process is scheduled to run)
ptRegister ptRegVals[NUM_PROCESSES]; 

int pageToEvict = 0;

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

void loadPTFromDisk(int pid, int frame) {
    printf("Put page table for PID %i into physical frame %i.\n", pid, frame);
    DISK_GetPT(pid, Memsim_GetPhysMem() + PAGE_START(frame));
}

void loadPageFromDisk(int pid, int pa, int frame) {
    DISK_GetPage(pid, pa, Memsim_GetPhysMem() + PAGE_START(frame));
}

PageTable *getPageTable(int pid) {
    if (!PT_PageTableExists(pid)) return NULL;
    return PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem();
}

int PT_Evict() {
    int isPT = -1; // If page to evict is a table entry (if so pid of pt)
    int isPage = -1; // If pageToEvict is a process page (if so pa for pid)
    int pid; // If pageToEvict is a process page, its the coresponding 
    PageTable *pt;

    // Handle cases for what evicting frame (pageToEvict) is:
    //  if pt has an entry in phys mem
    //      evict next frame instead and return
    //  if pt has no entry in phys mem
    //      set isPT to the pid coresponding to pt
    //  if evicting page is a process' page
    //      set isPage to the coresponding entry index of its pt
    //      set pid to the pid of the page table
    for (int i = 0; i < 4; i++) {
        pt = getPageTable(i);
        if (!pt || !ptRegVals[i].present) continue; //If not in phys mem disregard
        if (ptRegVals[i].ptStartPA == pageToEvict) { // If page is the current pt
            
            isPT = i;
            
            // Check if entry is in phys mem, if so inc pageToEvict and try evicting next frame instead
            for (int j = 0; j < 4; j++) {
                if (entryPhysPresent(pt->entries[j])) {
                    pageToEvict = (++pageToEvict)%4;
                    return PT_Evict();
                }
            }
            // If nothing in phys mem break out and evict pt
            break;
        } else {
            // Check if page is an entry of the current pt
            for (int j = 0; j < 4; j++) {
                if (!entryPhysPresent(pt->entries[j])) continue;
                // If page table entry is the pageToEvict mark it then end for loop
                if (pt->entries[j].pfn == pageToEvict) {
                    isPage = j;
                    pid = i;
                    break;
                }
            }
            if (isPage >= 0) break; // If match found break out of loop
        }
    }

    if (isPage >= 0) {
        // evict process page
        DISK_StorePage(pid, isPage, PAGE_START(pageToEvict) + Memsim_GetPhysMem());
        printf("Swapped Frame %i to disk at offset %i.\n", pageToEvict, DISK_GetOffset(pid, isPage));
    } else if (isPT >= 0) {
        // Evict page table
        DISK_StorePT(isPT, PAGE_START(pageToEvict) + Memsim_GetPhysMem());
        printf("Swapped Frame %i to disk at offset %i.\n", pageToEvict, DISK_GetOffset(isPT));
    } else {
        printf("ERROR: invalid unreachable state in PT_Evict()\n");
    }

    int retPage = pageToEvict;
    pageToEvict = (++pageToEvict)%4;
    return retPage;
}



void PT_Init() {
    for (int i = 0; i < NUM_PROCESSES; i++) {
        ptRegVals[i].ptStartPA = -1;
        ptRegVals[i].present = false;
    }
}

bool PT_PageTableExists(int pid) {
    return ptRegVals[pid].ptStartPA != -1;
}

int PT_PageTableInit(int pid) {
    ptRegVals[pid].ptStartPA = PT_GetFreeFrame();
    ptRegVals[pid].present = true;

    PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem();
    pt->exists = true;
    for (int i = 0; i < 3; i++) {
        pt->entries[i].bits = 0;
        pt->entries[i].pfn = 0;
    }
    printf("Put page table for PID %i into physical frame %i.\n",pid, ptRegVals[pid].ptStartPA);
}

void PT_SetWritable(int pid, int vpn, bool protection) {
    assert(PT_PageTableExists(pid)); 
    PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem();
    entryWritable_set(&pt->entries[vpn], protection);
}

int PT_GetFreeFrame(){
    int memSim = Memsim_FirstFreePFN();
    if(!(memSim == -1)){
        return memSim;
    } else {
        return PT_Evict();
    }
}

void PT_AddEntry(int pid, int pageAddress, bool writable) {
    // Get page table for PID, create new one if doesn't exist
    PageTable* pt; //Array of page tables
    int vpn = VPN(pageAddress);
    if (!PT_PageTableExists(pid)) { 
        PT_PageTableInit(pid);
    } else if (!ptRegVals[pid].present) {
        loadPTFromDisk(pid, PT_GetFreeFrame());
    }
    PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem(); //Gets page table
    
    if (entryExists(pt->entries[vpn])) {
        printf("Error: page already mapped");
        return;
    }
	entryExists_set(&pt->entries[vpn], true);
	entryWritable_set(&pt->entries[vpn], writable);
	
    pt->entries[vpn].pfn = PT_GetFreeFrame();
	
	entryPhysPresent_set(&pt->entries[vpn], true);
    printf("Mapped virtual address %i (page %i) into physical frame %i.\n",pageAddress, vpn,pt->entries[pageAddress].pfn);
}

int PT_GetFN(int pid, int va){
    if(!PT_PageTableExists(pid) && ){
        return -1;
    }
    
}

int getFrameAddr(int pid, int pa) {
    // If page table doesn't exist return error
    if (!PT_PageTableExists(pid)) {
        printf("Error: PT not exist"); 
        return -1;
    }
    // If its not in phys mem bring it into mem
    if (ptRegVals[pid].present = false){
        loadPTFromDisk(pid, PT_GetFreeFrame());
    }
    
	PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem();
    
    // If entry exists 
    if (!entryExists(pt->entries[pa])) return -1;
    if (!entryPhysPresent(pt->entries[pa])) {
        loadPageFromDisk(pid, pa, PT_GetFreeFrame());
    }
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