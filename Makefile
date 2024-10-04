all: build_rmatch run build_rmatch_so

build_rmatch: 
	gcc rmatch.c -Wall -Wextra -Ofast -o rmatch -Werror
build_rmatch_so:
	gcc rmatch.so.c -fPIC -shared -o librmatch.so -Ofast -Werror


run:
	./rmatch


