
We start by reading our input file in mmu.c we then initialize our pagetable manager and disk
(secondary memory). We then pass each line form the input file into input.c which parses that 
and calls the requested instruction in instructions.c.

Map:
    Takes in the parameters from the input file then it checks if a pagetable for that process
    doesn't exist or an entry for that virtual address doesn't exist. If those conditions are
    not met then it checks if the write permissions for that entry match the input. If they do
    then an error is thrown and if they don't the write permissions are updated. Now if a 
    pagetable for that process doesn't exist or an entry for that virtual address doesn't exist
    we call the PT_AddEntry in pagetable.c.

PT_AddEntry:
    Takes in a pid, page address, and write permissions. It first checks if a pagetable for that
    pid exists, If it doesn't, one is created. If it does exist we check if it is on primary memory.
    if it is on primary memory we add the entry to the pagetable and give it relevant values from
    the input. If it isn't on primary memory then we call PT_GetFreeFrame and load the pagetable from
    disk to the frame PT_GetFreeFrame returns and proceed as if it was always on physical memory.

PT_GetFreeFrame:
    Store the return of Memsim_FirstFreePFN (from the starter code). That is >= 0, meaning theres a
    free frame on primary memory then return that page frame number. If ther is no free frame on physical
    memory return the output of PT_Evict.

PT_Evict:
    Starts by going through each pagetable. If that pagetable has an entry on physical memory then it
    skips over that one. If that pagetable has no entry in physical memory move that page table to 
    secondary memory and return the frame that was freed. If there is an entry is the page to be evited
    it gets moved to secondary memory and returns the frame that was freed.

Store:
    Takes in the pid, virtual address and value to be stored. If the page table exists for that pid then
    we convert the virtual address to a physical address and store the value there. If not then an error
    message is returned

Load: 
    Takes in the pid and the virtual address. If the page table exists for that pid then we convert the 
    virtual address to a physical address and print the value there.

Disk:
    Everything on the disk is stored as binary values in disk.txt