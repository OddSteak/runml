CC=cc
CFLAGS=-Wall -W -std=c11

all: runml

runml: builds/runml

builds/runml: runml.c
	@mkdir -p builds
	$(CC) $(CFLAGS) -o builds/runml runml.c

test: builds/runml
	@echo "Running tests..."
	python test.py
	rm ml-*
	@echo "Completed tests"

.PHONY: all test runml
