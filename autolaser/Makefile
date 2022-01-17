OPTS=-O2 -W -Wall
LIBS=
all: compile link

compile:
	cc $(OPTS) -c fikse-autolaser-fil.c

link:
	cc $(OPTS) $(LIBS) -o fikse-autolaser-fil fikse-autolaser-fil.o

clean:
	rm -f *.o *.so core.* core fikse-autolaser-fil ihjac1.new
