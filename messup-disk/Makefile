OPTS=-O2 -W -Wall
all: compile link

compile:
	cc $(OPTS) -c messup.c

link:
	cc -o messup messup.o

clean:
	rm -f *.so core.* core messup
