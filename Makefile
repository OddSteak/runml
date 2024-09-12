CC=cc
CFLAGS=-Wall -W -std=c11

all: runml

runml: builds/runml

builds/runml: runml.c
	@mkdir -p builds
	$(CC) $(CFLAGS) -o builds/runml runml.c

test: builds/runml
	@echo "Running tests..."
<<<<<<< HEAD
	python3 test.py
=======
	python test.py
	rm ml-*
>>>>>>> 8307a29f55cbd663f6abdb948ea5b4ae94ffe314
	@echo "Completed tests"

.PHONY: all test runml
