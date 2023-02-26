#THREADED=-D__USE_THREADS
OPTS=-g -O2 $(THREADED)

all: memstress.o
	cc -Wall $(OPTS) -o memstress memstress.o

memstress.o: memstress.c
	cc -Wall $(OPTS) -c memstress.c

install: all
	install -m 0755 memstress /usr/local/bin

clean:
	rm -f *.o memstress
