all: rmatch run

rmatch: rmatch.c
	gcc rmatch.c -Wall -Wextra -Ofast -o rmatch

run:
	./rmatch 