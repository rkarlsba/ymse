CC=gcc
OPTIMISE=-O0
WARN=-Wall

all: compile link

compile:
	$(CC) $(WARN) $(OPTIMISE) -c asmtest.c
	$(CC) $(WARN) $(OPTIMISE) -c ortest.c

assemble:
	$(CC) $(WARN) $(OPTIMISE) -S asmtest.c
	$(CC) $(WARN) $(OPTIMISE) -S ortest.c

link:
	$(CC) $(WARN) $(OPTIMISE) -o asmtest asmtest.o
	$(CC) $(WARN) $(OPTIMISE) -o ortest ortest.o

clean:
	rm -f asmtest ortest *.o
