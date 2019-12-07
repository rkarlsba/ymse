OPTS=-O2 -W -Wall
LIBS=-lreadline
all: compile link

compile:
	cc $(OPTS) -c messup.c

link:
	cc $(OPTS) $(LIBS) -o messup messup.o

clean:
	rm -f *.so core.* core messup
