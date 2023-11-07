CC=gcc
C23=-std=c2x
OPTIMISE=-O2
WARN=-Wall

all: compile link

compile:
	$(CC) $(WARN) $(OPTIMISE) -c memstuff.c

assemble:
	$(CC) $(WARN) $(OPTIMISE) -S memstuff.c

link:
	$(CC) $(WARN) $(OPTIMISE) -o memstuff memstuff.o

clean:
	rm -f memstuff *.o
