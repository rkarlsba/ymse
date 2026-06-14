/*
 * vim:ts=4:sw=4:sts=4:et:ai
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUFSIZE 16384

char buf[BUFSIZE];

//char *in_filename = "ihjac1.out";
//char *out_filename = "ihjac1.new";

void help() {
    fprintf(stderr, "Syntax: fikse-autolaser-fil [-f ] -i input_file -o output_file\n\n");
    fprintf(stderr, "Having -i and -o the input and output files and -f for --force to overwrite an\n");
    fprintf(stderr, "existing output file\n");
    exit(1);
}

void die(const char* format, ...)
{
    char dst[1024*16];
    va_list argptr;
    va_start(argptr, format);
    vsprintf(dst, format, argptr);
    va_end(argptr);
    printf("ERROR: %s", dst);
    help();
}

void debug(const char* format, ...)
{
    char dst[1024*16];
    va_list argptr;
    va_start(argptr, format);
    vsprintf(dst, format, argptr);
    va_end(argptr);
    printf("DEBUG: %s", dst);
}

void demoronize(char *buf, ssize_t size) {
    int i;
    for (i=0;i<size;i++) {
        char x=buf[i];
        x = x^0x63;
        buf[i] = x;
    }
}

int main(int argc, char *argv[]) {
	int fin,fout,bread,bwritten;
    int force_flag = 0;
    int c;
    char *in_filename = NULL;
    char *out_filename = NULL;
    struct stat statbuf;

    while ((c = getopt (argc, argv, "fhi:o:?")) != -1) {
        switch (c) {
            case 'f':
                force_flag = 1;
                break;
            case 'i':
                in_filename = optarg;
                break;
            case 'o':
                out_filename = optarg;
                break;
            case '?':
            case 'h':
                help();
                break;
            default:
                die("Invalid argument\n");
                break;
        }
    }

    if (in_filename == NULL || out_filename == NULL) {
        die("I need intput and output files\n");
    }

    if (!force_flag) {
        if (stat(out_filename,&statbuf) == 0) {
            die("Won't overwrite an output file without -f\n");
        }
    }

	fin = open(in_filename, O_RDONLY);
	if (fin < 0) {
		die("Error opening file '%s': %s\n", in_filename, strerror(errno));
	}

	fout = open(out_filename, O_CREAT|O_WRONLY, 0644);
	if (fout < 0) {
		die("Error opening file '%s': %s\n", out_filename, strerror(errno));
	}

    do {
        // ssize_t read(int fildes, void *buf, size_t nbyte);
        bread = read(fin, buf, BUFSIZE);
        if (bread < 0) {
            die("Error reading file '%s': %s\n", in_filename,  strerror(errno));
        }
        demoronize(buf, BUFSIZE);
        // ssize_t write(int fildes, const void *buf, size_t nbyte);
        bwritten = write(fout, buf, bread);
        if (bwritten != bread) {
            die("Error writing file '%s' (fd == %d, bread == %d, bwritten == %d): %s\n", out_filename, fout, bread, bwritten, strerror(errno));
        }
    } while (bread == BUFSIZE);

    close(fout);
    close(fin);
}
