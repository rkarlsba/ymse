OPTS=-O2 -W -Wall
LIBS=
all: compile link

compile:
	cc $(OPTS) -c iszeroed.c

link:
	cc $(OPTS) $(LIBS) -o iszeroed iszeroed.o

clean:
	rm -f *.{o,so} core.* core iszeroed
