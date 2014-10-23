// test program

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

// maximum number of threads that can be created
#define MAXTHREAD 10
#define RANGE 1
#define ALLDONE 2
#define GO 3
#define GENDONE 4 	// Generation done
#define MAXGRID 40

// global variables
//struct msg array_of_msgs[MAXTHREAD + 1];
//sem_t psem_array[MAXTHREAD + 1], csem_array[MAXTHREAD + 1];
int **oddGen, **evenGen;
FILE *gen_file;
int gens;
char print, input;
char thisLine[MAXGRID];
// char[x][y]
// x is number of rows, y is number of columns
int x = 0, y = 0;

int main(int argc, char *argv[]) {

	// general usage check
	if (argc < 4) {
		fprintf(stderr, "Error: usage is ./life threads filename generations [print] [input]\n");
		exit(1);
	}

	// the user inputted number of threads to create
	int inputThread = atoi(argv[1]);
	if (inputThread > MAXTHREAD) {
		fprintf(stderr, "Error: program cannot create more than 10 threads\n");
		exit(1);
	}
	else if (inputThread == 0) {
		printf("Defaulting to maximum number of threads which is 10\n");
		inputThread = 10;
	}

	printf("do i seg fault1?\n");

	// input file
	gen_file = fopen(argv[2], "r");
	if (gen_file == NULL) {
		fprintf(stderr, "Error: unable to open file %s.\n", argv[2]);
		exit(1);
	}

	printf("do i seg fault2?\n");

	// number of requested generations
	gens = atoi(argv[3]);
	if (gens == 0) {
		fprintf(stderr, "Error: number of generations must be greater than 0\n");
		exit(1);
	}

	// print all generations or not
	print = 'n';
	if (argc >= 6) {
		print = *argv[4];
	}
	// pause at each generation or not
	input = 'n';
	if (argc < 6) {
		input = *argv[5];
	}

	printf("do i seg fault3?\n");
	/*
	while (fgets(thisLine, MAXGRID, gen_file) != NULL) {
		char *token;

		// getting rid of whitespaces
		token = strtok(thisLine, " ");

		while (token != NULL) {
			// y  is number of columns
			y = strlen(token);
			token = strtok(NULL, " ");
		}
		// x is number of rows
		x++;
	}*/
	printf("\nnumber of rows is %d and number of columns is %d\n",x,y);
	//fclose(gen_file);
	
}
