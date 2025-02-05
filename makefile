

all: field
	gcc field.c -o field

field: field.c football.o
	gcc field.c football.o -o field 

football.o: football.c football.h
	gcc -c football.c