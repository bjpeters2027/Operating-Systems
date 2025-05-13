all: mmu

mmu:
	make instructions.o
	make memsim.o
	make pagetable.o
	make disk.o
	make input.o
	gcc -c mmu.c -o mmu.o
	gcc -o mmu mmu.o disk.o pagetable.o memsim.o instructions.o input.o

instructions.o: instructions.c instructions.h
	gcc -c instructions.c -o instructions.o

memsim.o: memsim.c memsim.h
	gcc -c memsim.c -o memsim.o

pagetable.o: pagetable.c pagetable.h
	gcc -c pagetable.c -o pagetable.o

disk.o: disk.c disk.h
	gcc -c disk.c -o disk.o

input.o: input.c input.h
	gcc -c input.c -o input.o

clean:
	rm *.o
	rm mmu.exe