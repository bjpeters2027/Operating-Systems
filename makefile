

all: field.o football.o
	gcc field.o football.o -lm -o field

field.o: field.c football.o
	gcc -c field.c

football.o: football.c football.h
	gcc -c football.c