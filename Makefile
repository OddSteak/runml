CC=gcc
CFLAGS=-Wall -W -pedantic -ansi -std=c11

default: runml.c
	$(CC) $(CFLAGS) -o builds/runml runml.c
