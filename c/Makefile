CC=gcc
OPTIMISE=-O0
WARN=-Wall

all: compile link

compile:
	$(CC) $(WARN) $(OPTIMISE) -c asmtest.c

assemble:
	$(CC) $(WARN) $(OPTIMISE) -S asmtest.c

link:
	$(CC) $(WARN) $(OPTIMISE) -o asmtest asmtest.o
