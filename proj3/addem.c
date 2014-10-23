// Alex Shoop CS 3013 project 3 "addem" program
// Received guidance from Professor Wills, TA Bohao, and friend Austin Rose

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

// maximum number of threads that can be created
#define MAXTHREAD 10
#define RANGE 1
#define ALLDONE 2

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

// helper functions
// this function puts a given message into a mailbox given by the iTo ID
void SendMsg(int iTo, struct msg *pMsg) {

	if (iTo == 0) {
		sem_wait(&psem_array[iTo]);
		array_of_msgs[iTo].iFrom = pMsg->iFrom;
		array_of_msgs[iTo].type = pMsg->type;
		array_of_msgs[iTo].value1 = pMsg->value1;
		array_of_msgs[iTo].value2 += pMsg->value1;
		sem_post(&csem_array[iTo]);
	}
	else {
		sem_wait(&psem_array[iTo]);
		array_of_msgs[iTo].iFrom = pMsg->iFrom;
		array_of_msgs[iTo].type = pMsg->type;
		array_of_msgs[iTo].value1 = pMsg->value1;
		array_of_msgs[iTo].value2 = pMsg->value2;
		sem_post(&csem_array[iTo]);
	}
	
}

// this functions gets a message from a given mailbox iFrom ID 
void RecvMsg(int iFrom, struct msg *pMsg) {
	sem_wait(&csem_array[iFrom]);
	pMsg->iFrom = array_of_msgs[iFrom].iFrom;
	pMsg->type = array_of_msgs[iFrom].type;
	pMsg->value1 = array_of_msgs[iFrom].value1; 
	pMsg->value2 = array_of_msgs[iFrom].value2; 
	sem_post(&psem_array[iFrom]);
}

// function that thread actually does
void *init_thread(void *arg) {
	long mailboxID = (long)arg;
	int i;
	struct msg thisMessage, newMessage;
	RecvMsg(mailboxID, &thisMessage);

	// temp message which holds the message to calculate the addem sum as well as the type ALLDONE
	newMessage.iFrom = thisMessage.iFrom;
	newMessage.type = ALLDONE;
	newMessage.value1 = thisMessage.value1;
	newMessage.value2 = thisMessage.value2;

	// add up the numbers between value1 and value2
	for (i = (newMessage.value1 + 1); i <= newMessage.value2; i++) {
		newMessage.value1 += i; 
	}

	// send message to parent's mailbox
	SendMsg(0, &newMessage);
	// receive message (necessary to avoid infinite loop)
	RecvMsg(0, &newMessage);
}

int main(int argc, char *argv[]) {
	struct msg someMessage;
	long i;
	int modulus = 0, max = 0;
	int min = 1;

	// general usage check
	if (argc != 3) {
		fprintf(stderr, "Error: usage is ./addem NUMBER_OF_THREADS MAX_RANGE_NUMBER\n");
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

	// max number range when adding numbers up
	int maxrange = atoi(argv[2]);

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

	// populating each mailbox
	for (i = 1; i < inputThread + 1; i++) {

		if (i > 1) {
			min = max + 1;
		}

		max = 0;
		max = (min - 1) + (maxrange / inputThread);

		modulus = (maxrange % inputThread);

		if (i  <= modulus) {
			max += 1;
		}

		someMessage.iFrom = i;
		someMessage.type = RANGE;
		someMessage.value1 = min;
		someMessage.value2 = max;

		SendMsg(i, &someMessage);
	}

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

	// print final sum results
	printf("The total for 1 to %d using %d threads is %d\n", maxrange, inputThread, array_of_msgs[0].value2);
	return 0;
}
