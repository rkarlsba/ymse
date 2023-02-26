/*
 * vim:ts=4:sw=4:si:cindent:tw=80
 *
 * memstress v0.0.1 - a simple memory stress utility, written by Roy Sigurd
 * Karlsbakk <roy@karlsbakk.net>. I wrote this tool to stress a server that was
 * behaving rather badly, and was able to reproduce an error after some runs.
 * This tool allocates a number of chunks, and writes 0xaa and 0x55 to them for
 * each test pass. Start with -s <chunksize in MB> -c <chunk count> -p <passes>.
 * Default is 4 chunks of 1 GB each and no passes (just allocate, memset to
 * zero, and free).
 *
 * Licensed under GPL version 2. See http://www.gnu.org/licenses/gpl-2.0.html or
 * the LICENCE file for details.
 */
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GB 0x40000000
#define MAXCHUNKS 1024

/* Globals - they're ugly, but some times you need them... */
int interrupted = 0;

/* Signal handler for various trapped signals */
void sigtrap(int sig) {
	fprintf(stderr, "Received signal %d\n", sig);
	interrupted++;
}

/* Main program */
int main(int argc, char **argv) {
	uint64_t size = GB;
	int chunks = 4;
	int seconds = 5;
	int passes = 1;
#ifdef __USE_THREADS
	int threads = 0;
#endif
	int i;
	unsigned long long p;
	void *v[MAXCHUNKS];

	char *cvalue = NULL;
	char *pvalue = NULL;
	char *svalue = NULL;
#ifdef __USE_THREADS
	char *tvalue = NULL;
#endif

	char *errptr = NULL;
	int c;
	int pattern;

	opterr = 0;

	while ((c = getopt (argc, argv, "c:hp:s:")) != -1) {
		switch (c) {
			case 'c':
				cvalue = optarg;
				chunks = strtol(cvalue, &errptr, 0);
				if (chunks > MAXCHUNKS) {
					fprintf(stderr, "MAXCHUNKS is set to %d - change the source to go above this limit", MAXCHUNKS);
					exit(1);
				}
				break;
			case 'h':
				printf("Possible options include\n");
				printf("    -c <chunks>\n");
				printf("    -s <chunksize> (MB)\n");
				printf("    -p <passes> - set to -1 for infinite test\n");
				exit(1);
			case 'p':
				pvalue = optarg;
				passes = strtol(pvalue, &errptr, 0);
				break;
			case 's':
				svalue = optarg;
				size = strtol(svalue, &errptr, 0);
				size *= 1024;
				size *= 1024;
				break;
#ifdef __USE_THREADS
			case 't':
				svalue = optarg;
				threads = strtol(tvalue, &errptr, 0);
				break;
#endif
			default:
				abort ();
		}
	}

	signal(SIGTERM, sigtrap);
	signal(SIGINT, sigtrap);
	signal(SIGHUP, sigtrap);

	printf("Allocating %i chunks of %llu bytes, running %d passes testing. Please press ctrl+c to cancel within %i seconds",
			chunks, (long long unsigned)size, passes, seconds);
	fflush(stdout);
	for (i=0;!interrupted&&i<seconds;i++) {
		sleep(1);
		printf("."); fflush(stdout);
	}
	if (interrupted)
		exit(1);

	printf("\n\nAllocating and writing to data - one dot per chunk"); fflush(stdout);
	for (i=0;!interrupted&&i<chunks;i++) {
		v[i] = malloc(size);
		if (v == NULL) {
			fprintf(stderr, "\nOut of memory after %i chunks\n", i);
			exit(1);
		}
		memset(v[i], 0, size);
		printf("."); fflush(stdout);
	}
	if (interrupted)
		exit(1);
	printf("\nDone allocating\n"); fflush(stdout);
	for (p=0;!interrupted&&(passes==-1||p<passes);p++) {
		printf("\nRunning test pass %llu, one dot per chunk", p); fflush(stdout);
		if (passes % 2)
			pattern = 0xaaaaaaaa;
		else
			pattern = 0x55555555;
		for (i=0;!interrupted&&i<chunks;i++) {
			memset(v[i], pattern, size);
			printf("."); fflush(stdout);
		}
	}
	if (interrupted)
		printf("Interrupted - freeing memory");
	else
		printf("Done - freeing memory");
	fflush(stdout);

	for (i=0;i<chunks;i++)
		free(v[i]);
	printf(" - %llu passes completed\n", p);
	return 0;
}
