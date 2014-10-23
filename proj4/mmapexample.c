// Demonstrates how to use mmap
//
// Modified by Craig Wills from:
// Copyright (c) Michael Still, released under the terms of the GNU GPL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
    int fd;
    char *pchFile;
    struct stat sb;

    if(argc < 2) {
	fprintf(stderr, "Usage: %s <input>\n", argv[0]);
	exit(1);
    }
    

    /* Map the input file into memory */
    if ((fd = open (argv[1], O_RDONLY)) < 0) {
	perror("Could not open file");
	exit (1);
    }

    if(fstat(fd, &sb) < 0){
	perror("Could not stat file to obtain its size");
	exit(1);
    }

    if ((pchFile = (char *) mmap (NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0)) 
	== (char *) -1)	{
	perror("Could not mmap file");
	exit (1);
    }

    // pchFile is now a pointer to the entire contents of the file...
    // do processing of the file contents here

    // Now clean up
    if(munmap(pchFile, sb.st_size) < 0){
	perror("Could not unmap memory");
	exit(1);
    }

    close(fd);
}
