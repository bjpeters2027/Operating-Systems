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

int pageToEvict = 1;

// typedef struct {
// 	char pfn;
// 	char bits;
// } Entry;

// typedef struct {
//     Entry entries[NUM_PAGES];
//     bool exists;
// } PageTable;


void loadPTFromDisk(int pid, int frame) {
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
                if (pt->entries[j].exists && pt->entries[j].present) {
                    pageToEvict = (++pageToEvict)%4;
                    return PT_Evict();
                }
            }
            // If nothing in phys mem break out and evict pt
            break;
        } else {
            // Check if page is an entry of the current pt
            for (int j = 0; j < 4; j++) {
                // If page table entry is the pageToEvict mark it then end for loop
                if (pt->entries[j].present && pt->entries[j].pfn == pageToEvict) {
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
        pt->entries[isPage].present = false;
        DISK_StorePage(pid, isPage, PAGE_START(pageToEvict) + Memsim_GetPhysMem());
        printf("Swapped Frame %i to disk at offset %i.\n", pageToEvict, DISK_PageGetOffset(pid, isPage));
    } else if (isPT >= 0) {
        // Evict page table
        ptRegVals[isPT].present = false;
        DISK_StorePT(isPT, PAGE_START(pageToEvict) + Memsim_GetPhysMem());
        printf("Put page table for PID %i to disk at offset %i.\n", isPT, DISK_PTGetOffset(isPT));
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
    if (PT_PageTableExists(pid)) printf("Error pt already exists");
    ptRegVals[pid].ptStartPA = PT_GetFreeFrame();
    ptRegVals[pid].present = true;

    PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem();
    for (int i = 0; i < 4; i++) {
        pt->entries[i].exists = false;
        pt->entries[i].present = false;
        pt->entries[i].write = false;
        pt->entries[i].pfn = 0;
    }
    printf("Put page table for PID %i into physical frame %i.\n",pid, ptRegVals[pid].ptStartPA);
}

void PT_SetWritable(int pid, int vpn, bool protection) {
    assert(PT_PageTableExists(pid)); 
    PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem();
    pt->entries[vpn].write = protection;
}

int PT_GetFreeFrame(){
    int memSim = Memsim_FirstFreePFN();
    int frame;
    if(memSim != -1){
        frame = PFN(memSim);
    } else {
        frame = PT_Evict();
    }
    memset(Memsim_GetPhysMem() + PAGE_START(frame), 0, 16);
    return frame;
}

void PT_AddEntry(int pid, int pageAddress, bool writable) {
    // Get page table for PID, create new one if doesn't exist
    int vpn = VPN(pageAddress);
    if (!PT_PageTableExists(pid)) { 
        PT_PageTableInit(pid);
    } else if (!ptRegVals[pid].present) {
        int frame = PT_GetFreeFrame();
        loadPTFromDisk(pid, frame);
        printf("Put page table for PID %i into physical frame %i\n", pid, frame);
        ptRegVals[pid].ptStartPA = frame;
        ptRegVals[pid].present = true;
    }
    PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem(); //Gets page table
    
    if (pt->entries[vpn].exists) {
        printf("Error: page already mapped\n");
        return;
    }
    pt->entries[vpn].exists = true;
    pt->entries[vpn].write = writable;
    pt->entries[vpn].pfn = PT_GetFreeFrame();

	pt->entries[vpn].present = true;
    printf("Mapped virtual address %i (page %i) into physical frame %i.\n",pageAddress, vpn, pt->entries[vpn].pfn);
}

int PT_GetFN(int pid, int va){
    if(!PT_PageTableExists(pid)){
        return -1;
    }
    PageTable* pt = getPageTable(pid);
    if(pt->entries[va].exists && pt->entries[va].present){
        return pt->entries[va].pfn;
    }
    return -1;
}

int getFrameAddr(int pid, int vpn) {
    // If page table doesn't exist return error
    if (!PT_PageTableExists(pid)) {
        printf("Error: PT not exist"); 
        return -1;
    }
    // If its not in phys mem bring it into mem
    if (ptRegVals[pid].present == false){
        int frame = PT_GetFreeFrame();
        loadPTFromDisk(pid, frame);
        printf("Put page table for PID %i into physical frame %i\n", pid, frame);
        ptRegVals[pid].ptStartPA = frame;
        ptRegVals[pid].present = true;
    }
    
	PageTable* pt = PAGE_START(ptRegVals[pid].ptStartPA) + Memsim_GetPhysMem();
    
    // If entry exists 
    if (!pt->entries[vpn].exists) return -1;
    if (!pt->entries[vpn].present) {
        int frame = PT_GetFreeFrame();
        loadPageFromDisk(pid, vpn, frame);
        pt->entries[vpn].pfn = frame;
        pt->entries[vpn].present = true;
    }
    return pt->entries[vpn].pfn;
}

int PT_VPNtoPA(int pid, int va) {
    int pageAddr = getFrameAddr(pid, VPN(va)) * PAGE_SIZE;
    if (pageAddr < 0) return pageAddr;
    return (pageAddr) + PAGE_OFFSET(va);
}

bool PT_PIDHasWritePerm(int pid, int vpn) {
    assert(PT_PageTableExists(pid));
    // If its not in phys mem bring it into mem
    if (!ptRegVals[pid].present){
        int frame = PT_GetFreeFrame();
        loadPTFromDisk(pid, frame);
        printf("Put page table for PID %i into physical frame %i\n", pid, frame);
        ptRegVals[pid].ptStartPA = frame;
        ptRegVals[pid].present = true;
    }
    PageTable *pt = getPageTable(pid);
    return pt->entries[vpn].write;
}

bool PT_HasEntry(int pid, int vpn) {
    if (!PT_PageTableExists(pid)) return false;
    return getPageTable(pid)->entries[vpn].exists;
}