#THREADED=-D__USE_THREADS
OPTS=-O2 -W -Wall $(THREADED)
PROGNAME="center-stl"

all: $(PROGNAME).o
	cc -Wall $(OPTS) -o $(PROGNAME) $(PROGNAME).o

$(PROGNAME).o: $(PROGNAME).c
	cc -Wall $(OPTS) -c $(PROGNAME).c

install: all
	install -m 0755 $(PROGNAME) /usr/local/bin

clean:
	rm -f *.o $(PROGNAME)
