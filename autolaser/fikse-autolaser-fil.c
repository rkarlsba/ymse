/*
 * vim:ts=4:sw=4:sts=4:et:ai
 */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

const ssize_t bufsize = 16384;
char buf[bufsize];

char *in_filename = "ihjac1.out";
char *out_filename = "ihjac1.new";

int main() {
	int fin,fout,bread,bwritten;

	fin = open(in_filename, O_RDONLY);
	if (fin < 0) {
		fprintf(stderr, "Error opening file '%s': %s\n", in_filename, strerror(errno));
		exit(1);
	}

	fout = open(out_filename, O_CREAT, 0644);
	if (fout < 0) {
		fprintf(stderr, "Error opening file '%s': %s\n", out_filename,  strerror(errno));
		exit(2);
	}

    do {
        // ssize_t read(int fildes, void *buf, size_t nbyte);
        bread = read(fin, buf, bufsize);
        if (bread < 0) {
            fprintf(stderr, "Error reading file '%s': %s\n", in_filename,  strerror(errno));
            exit(3);
        }
        // ssize_t write(int fildes, const void *buf, size_t nbyte);
        bwritten = write(fout, buf, bread);
        if (bwritten != bread) {
            fprintf(stderr, "Error writing file '%s' (fd == %d, bread == %d, bwritten == %d): %s\n", out_filename, fout, bread, bwritten, strerror(errno));
            exit(4);
        }
    } while (bread == bufsize);

    close(fout);
    close(fin);
}
