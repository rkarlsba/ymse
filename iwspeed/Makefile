#THREADED=-D__USE_THREADS
LIBS=-liw
OPTS=-g -O2 $(THREADED) $(LIBS)

all: iwspeed.o
	cc -Wall $(OPTS) -o iwspeed iwspeed.o

iwspeed.o: iwspeed.c
	cc -Wall $(OPTS) -c iwspeed.c

#install: all
#	install -m 0755 iwspeed /usr/local/bin

clean:
	rm -f *.o iwspeed
