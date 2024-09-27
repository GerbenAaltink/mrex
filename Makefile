all: build run

build:
	gcc main.c -Wall -Wextra -Ofast -o mrex

run:
	./mrex 