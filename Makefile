all: rmatch run

rmatch: rmatch.c rmatch.h rmatch.so.c
	gcc rmatch.c -Wall -Wextra -Ofast -o rmatch
	gcc rmatch.so.c -fPIC -shared -o librmatch.so -Ofast

run:
	./rmatch


