all: rmatch run

rmatch: rmatch.c rmatch.h
	gcc rmatch.c -Wall -Wextra -Ofast -o rmatch

run:
	./rmatch 