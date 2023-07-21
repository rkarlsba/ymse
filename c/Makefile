CC=gcc
C23=-std=c2x
OPTIMISE=-O0
WARN=-Wall

all: compile link

compile:
	$(CC) $(WARN) $(OPTIMISE) -c asmtest.c
	$(CC) $(WARN) $(OPTIMISE) -c ortest.c
	$(CC) $(C23) $(WARN) $(OPTIMISE) -c c23.c

assemble:
	$(CC) $(WARN) $(OPTIMISE) -S asmtest.c
	$(CC) $(WARN) $(OPTIMISE) -S ortest.c
	$(CC) $(C23) $(WARN) $(OPTIMISE) -S c23.c

link:
	$(CC) $(WARN) $(OPTIMISE) -o asmtest asmtest.o
	$(CC) $(WARN) $(OPTIMISE) -o ortest ortest.o
	$(CC) $(C23) $(WARN) $(OPTIMISE) -o c23 c23.o

clean:
	rm -f asmtest ortest *.o
