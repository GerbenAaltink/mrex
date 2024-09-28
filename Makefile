all: rmatch run

rmatch: rmatch.c rmatch.h
	gcc rmatch.c -Wall -Wextra -Ofast -o rmatch -Winline -fdump-ipa-inline

run:
	./rmatch 