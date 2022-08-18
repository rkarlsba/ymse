OPTS=-O2 -W -Wall
LIBS=-lwiringPi
all: compile link

compile:
	cc $(OPTS) -c pi-tempctl.c

link:
	cc $(OPTS) $(LIBS) -o pi-tempctl pi-tempctl.o

clean:
	rm -f *.{o,so} core.* core pi-tempctl
