/* fileio.c */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#define BUFSIZE 1024

main(int argc, char *argv[])
{
    char buf[BUFSIZE];
    int fdIn, cnt, i;

    if (argc < 2) {
	fdIn = 0;  /* just read from stdin */
    }
    else if ((fdIn = open(argv[1], O_RDONLY)) < 0) {
	fprintf(stderr, "file open\n");
	exit(1);
    }

    // copy input to stdout
    while ((cnt = read(fdIn, buf, BUFSIZE)) > 0) {
        write(1, buf, cnt);
    }
    if (fdIn > 0)
        close(fdIn);
}
