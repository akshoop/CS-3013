/*
*	Alex Shoop CS 3013 test1 program
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

main(int argc, char *argv[]) {
	// argc is number of arguments
	// argv is an array of strings

	int pid;
	int i;

	// print out all arguments
	printf("There are %d arguments\n", argc);

	for (i = 0; i < argc; i++) {
		printf("%s\n", argv[i]);
	}

	pid = fork();

	// fork error case
	if (pid < 0) {
		fprintf(stderr, "Fork error\n");
		exit(1);
	}

	// child process
	else if (pid == 0) {
		for (i = 0; i < 5; i++) {
			printf("child (%d) : %s\n", getpid(), argv[2]);
		}
		exit(0);
	}

	// parent process
	else {
		for (i = 0; i < 5; i++) {
			printf("parent (%d) : %s\n", getpid(), argv[1]);
		}
		exit(0);
	}
}
