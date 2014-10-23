// Alex Shoop CS 3013 project 3 "life" program

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

// msg struct
struct msg {
	int iFrom; /* who sent the message (0 .. number-of-threads) */ 
	int type; /* its type */
	int value1; /* first value */
	int value2; /* second value */
};

// global variables
struct msg array_of_msgs[MAXTHREAD + 1];
sem_t psem_array[MAXTHREAD + 1], csem_array[MAXTHREAD + 1];
int **oddGen, **evenGen, **segmentGen;
FILE *gen_file;
int gens;
char print, input;
char thisLine[50];
char file_input_lines[50];
char *token;
// x is number of rows, y is number of columns
int x = 0, y = 0;
int genCounter = 0;


// helper functions

// puts the input file in the Board
void putFileInBoard(FILE *file, int **B, unsigned int sizeX, unsigned int sizeY) {
	int i, j, digit;
	for (i = 0; i < sizeX; i++) {
		fgets(file_input_lines, 50, file);
		for (j = 0; j < sizeY; j++) {
			if (j == 0) {
				token = strtok(file_input_lines, " ");
				if (token != NULL) {
					digit = atoi(token);
					if (digit == 1) {
						B[i][j] = 1;
					}
					else if (digit == 0) {
						B[i][j] = 0;
					}
				}
			}
			else {
				token = strtok(NULL, " ");
				if (token != NULL) {
					digit = atoi(token);
					if (digit == 1) {
						B[i][j] = 1;
					}
					else if (digit == 0) {
						B[i][j] = 0;
					}
				}
			}
		}
	}
}

// count the number of neighbors 
int countNearestNeighbor(int **Board, unsigned int sizeX, unsigned int sizeY, unsigned int x, unsigned int y) {
	int counter = 0;

	// at the first element space (ie top left)
	if (x == 0 && y == 0) {
		if (Board[x][y+1] == 1) {
			counter++;
		}
		if (Board[x+1][y+1] == 1) {
			counter++;
		}
		if (Board[x+1][y] == 1) {
			counter++;
		}
	}
	// at the last element space (ie bottom right)
	else if (x == (sizeX - 1) && y == (sizeY - 1)) {
		if (Board[x][y-1] == 1) {
			counter++;
		}
		if (Board[x-1][y-1] == 1) {
			counter++;
		}
		if (Board[x-1][y] == 1) {
			counter++;
		}
	}
	// at element space top right
	else if (x == 0 && y == (sizeY - 1)) {
		if (Board[x+1][y] == 1) {
			counter++;
		}
		if (Board[x+1][y-1] == 1) {
			counter++;
		}
		if (Board[x][y-1] == 1) {
			counter++;
		}
	}
	// at element space bottom left
	else if (x == (sizeX - 1) && y == 0) {
		if (Board[x-1][y] == 1) {
			counter++;
		}
		if (Board[x-1][y+1] == 1) {
			counter++;
		}
		if (Board[x][y+1] == 1) {
			counter++;
		}
	}
	// at any element in top-most row
	else if (x == 0 && y < (sizeY - 1)) {
		if (Board[x][y-1] == 1) {
			counter++;
		}
		if (Board[x+1][y-1] == 1) {
			counter++;
		}
		if (Board[x+1][y] == 1) {
			counter++;
		}
		if (Board[x+1][y+1] == 1) {
			counter++;
		}
		if (Board[x][y+1] == 1) {
			counter++;
		}
	}
	// at any element in right-most column
	else if (x < (sizeX - 1) && y == (sizeY - 1)) {
		if (Board[x-1][y] == 1) {
			counter++;
		}
		if (Board[x-1][y-1] == 1) {
			counter++;
		}
		if (Board[x][y-1] == 1) {
			counter++;
		}
		if (Board[x+1][y-1] == 1) {
			counter++;
		}
		if (Board[x+1][y] == 1) {
			counter++;
		}
	}
	// at any element bottom-most row
	else if (x == (sizeX - 1) && y < (sizeY - 1)) {
		if (Board[x][y-1] == 1) {
			counter++;
		}
		if (Board[x-1][y-1] == 1) {
			counter++;
		}
		if (Board[x-1][y] == 1) {
			counter++;
		}
		if (Board[x-1][y+1] == 1) {
			counter++;
		}
		if (Board[x][y+1] == 1) {
			counter++;
		}
	}
	// at any element left-most column
	else if (x < (sizeX - 1) && y == 0) {
		if (Board[x-1][y] == 1) {
			counter++;
		}
		if (Board[x-1][y+1] == 1) {
			counter++;
		}
		if (Board[x][y+1] == 1) {
			counter++;
		}
		if (Board[x+1][y+1] == 1) {
			counter++;
		}
		if (Board[x+1][y] == 1) {
			counter++;
		}
	}
	// at any element space not in above conditions
	else if (x < (sizeX - 1) && y < (sizeY - 1)) {
		if (Board[x-1][y-1] == 1) {
			counter++;
		}
		if (Board[x-1][y] == 1) {
			counter++;
		}
		if (Board[x-1][y+1] == 1) {
			counter++;
		}
		if (Board[x][y-1] == 1) {
			counter++;
		}
		if (Board[x][y+1] == 1) {
			counter++;
		}
		if (Board[x+1][y-1] == 1) {
			counter++;
		}
		if (Board[x+1][y] == 1) {
			counter++;
		}
		if (Board[x+1][y+1] == 1) {
			counter++;
		}
	}
	return counter;
} 

// MAKE SURE ARGUMENT X IS THE SELECT NUMBER OF ROWS FOR THREAD
// plays certain amount of rows for one generation, by creating a new array from the old array
int **playOne(unsigned int x, unsigned int y, int **Old) {
	unsigned int i,j, temp;
	int **New = malloc(x * sizeof(int *));
	if (New) for (i = 0; i < x; i++) {
		New[i] = malloc(y * sizeof(int));
	}

	for (i = 0; i < x; i++) {
		for (j = 0; j < y; j++) {
			temp = countNearestNeighbor(Old, x, y, i, j);
			// inside occupied cell
			if (Old[i][j] == 1) {
				// if neighbors are 0, 1, or 4 to 8
				if (temp < 2 || temp > 3) {
					New[i][j] = 0;
				}
				// if neighbors are 2 or 3
				else if (temp == 2 || temp == 3) {
					New[i][j] = Old[i][j];
				}
			}
			// inside unoccupied cell
			else if (Old[i][j] == 0) {
				if (temp == 3) {
					New[i][j] = 1;
				}
			}
		}
	}
	return New;
}

// prints out the board
void printBoard(int **B, unsigned int x, unsigned int y) {
	int j, k;
	for (j = 0; j < x; j++) {
		for (k = 0; k < y; k++) {
			printf("%d ", B[j][k]);
		}
		printf("\n");
	}
}

// this function puts a given message into a mailbox given by the iTo ID
void SendMsg(int iTo, struct msg *pMsg) {
	sem_wait(&psem_array[iTo]);
	array_of_msgs[iTo].iFrom = pMsg->iFrom;
	array_of_msgs[iTo].type = pMsg->type;
	array_of_msgs[iTo].value1 = pMsg->value1;
	array_of_msgs[iTo].value2 = pMsg->value2;
	sem_post(&csem_array[iTo]);
}

// this functions gets a message from a given mailbox iFrom ID 
void RecvMsg(int iFrom, struct msg *pMsg) {
	sem_wait(&csem_array[iFrom]);
	pMsg->iFrom = array_of_msgs[iFrom].iFrom;
	pMsg->type = array_of_msgs[iFrom].type;
	pMsg->value1 = array_of_msgs[iFrom].value1; 
	pMsg->value2 = array_of_msgs[iFrom].value2;
	if (iFrom % 2 == 1) {
		printf("Generation %d:\n", genCounter);
		printBoard(oddGen, x, y);
		printf("\n");
	}
	else {
		printf("Generation %d:\n", genCounter);
		printBoard(evenGen, x, y);
		printf("\n");
	}
	sem_post(&psem_array[iFrom]);
}

// function that thread actually does
void *init_thread(void *arg) {
	long mailboxID = (long)arg;
	int i, j, thisGen;
	struct msg thisMessage, newMessage;
	RecvMsg(mailboxID, &thisMessage);
	int minrange, maxrange, rows;

	// these values determine what rows that worker thread uses to create the rows for next generation
	minrange = thisMessage.value1;
	maxrange = thisMessage.value2;
	rows = (maxrange - minrange) + 1;

	printf("thread %d is gonna make rows of next gen\n", mailboxID);
	for (thisGen = 1; thisGen <= gens; thisGen++) {
		// we are at odd generation
		if (thisGen % 2 == 1) {
			// play 1 generation, therefore setting the next gen which is evenGen
			segmentGen = playOne(rows, y, oddGen);
			for (i = 0; i < rows; i++) {
				for (j = 0; j < y; j++) {
					evenGen[minrange - 1][j] = segmentGen[i][j];
				}
				minrange++;
			}
			thisMessage.type = GENDONE;
			SendMsg(mailboxID, &thisMessage);
		}
		// we are at even generation
		else {
			// play 1 generation, therefore setting the next gen which is evenGen
			segmentGen = playOne(maxrange, y, evenGen);
			for (i = 0; i < rows; i++) {
				for (j = 0; j < y; j++) {
					oddGen[minrange - 1][j] = segmentGen[i][j];
				}
				minrange++;
			}
			thisMessage.type = GENDONE;
			SendMsg(mailboxID, &thisMessage);
		}
	}
	thisMessage.type = ALLDONE;
	SendMsg(0, &thisMessage);
	printf("Everything is done\n");

	// send message to parent's mailbox
	//SendMsg(0, &newMessage);
	// receive message (necessary to avoid infinite loop)
	//RecvMsg(0, &newMessage);
}

int main(int argc, char *argv[]) {
	struct msg someMessage;
	long i, j;
	int modulus = 0, max = 0;
	int min = 1;
	int len;

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

	// array of threads, based on user inputted number of threads
	pthread_t t[inputThread];

	// input file
	gen_file = fopen(argv[2], "r");
	if (!gen_file) {
		fprintf(stderr, "Error: unable to open file.\n");
		exit(1);
	}

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

	while (fgets(thisLine, 50, gen_file) != NULL) {
		len = strlen(thisLine);

		if (y == 0) {
			for (i = 0; i < len; i++) {
				char c = thisLine[i];
				if (c != ' ') {
					y++;
				}
			}
			// to get rid of counting the newline character
			y--;
		}
		// x is number of rows
		x++;
	}

	// sets up oddGen
	oddGen = malloc(x * sizeof(int *));
	if (oddGen) for (i = 0; i < x; i++) {
		oddGen[i] = malloc(y * sizeof(int));
		if (!oddGen[i]) exit (1);
	}
	for (i = 0; i < x; i++) {
		for (j = 0; j < y; j++) {
			oddGen[i][j] = 0;
		}
	}

	// sets up evenGen
	evenGen = malloc(x * sizeof(int *));
	if (evenGen) for (i = 0; i < x; i++) {
		evenGen[i] = malloc(y * sizeof(int));
		if (!evenGen[i]) exit (1);
	}
	for (i = 0; i < x; i++) {
		for (j = 0; j < y; j++) {
			evenGen[i][j] = 0;
		}
	}

	fclose(gen_file);
	gen_file = fopen(argv[2], "r");

	if (x > MAXGRID || y > MAXGRID) {
		fprintf(stderr, "Error: number of rows/columns of inputted file exceeds MAXGRID size which is 40\n");
		exit(1);
	}
	printf("\nnumber of rows is %d and number of columns is %d\n", x, y);

	// create a producer and consumer semaphore for each mailbox
	for (i = 0; i < (MAXTHREAD + 1); i++) {
		if (sem_init(&psem_array[i], 0, 1) < 0) {
        	perror("sem_init");
        	exit(1);
    	}
    	if (sem_init(&csem_array[i], 0, 0) < 0) {
        	perror("sem_init");
        	exit(1);
    	}
	}
	printf("Producer & Consumer semaphore created for each mailbox\n");

	printf("gens allocated\n");

	putFileInBoard(gen_file, oddGen, x, y);
	fclose(gen_file);
	printf("Succssfully put file in board\n");
	// populating each mailbox
	for (i = 1; i < inputThread + 1; i++) {

		if (i > 1) {
			min = max + 1;
		}

		max = 0;
		max = (min - 1) + (x / inputThread);

		modulus = (x % inputThread);

		if (i  <= modulus) {
			max += 1;
		}

		someMessage.iFrom = i;
		someMessage.type = RANGE;
		someMessage.value1 = min;
		someMessage.value2 = max;

		SendMsg(i, &someMessage);
	}
	printf("Populated mailboxes\n");

	// pthread_create related stuff
	for (i = 1; i < inputThread + 1; i++) {
		if (pthread_create(&t[i], NULL, init_thread, (void *)i) != 0) {
			perror("pthread_create");
			exit(1);
		}
	}

	// pthread_join and sem_destroy related stuff
	for (i = 1; i < inputThread + 1; i++) {
		(void)pthread_join(t[i], NULL);
	}
	for (i = 0; i < (MAXTHREAD + 1); i++) {
		(void)sem_destroy(&psem_array[i]);
		(void)sem_destroy(&csem_array[i]);
	}
	free(oddGen);
	free(evenGen);
	return 0;
}
