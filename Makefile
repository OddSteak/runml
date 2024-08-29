CC=gcc
CFLAGS=-Wall -W -pedantic -ansi -std=c11

all: runml

runml: builds/runml

builds/runml: runml.c
	@mkdir -p builds
	$(CC) $(CFLAGS) -o builds/runml runml.c

test: builds/runml
	@echo "Running tests..."
	python test.py
	@echo "Completed tests"

.PHONY: all test runml
