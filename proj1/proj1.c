/*
*	Alex Shoop CS 3013 Project 1 "doit" program
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <string.h>
#include <unistd.h>

// maximum amount of individual characters allowed is 128
#define MAX_CHAR 128
// maximum amount of individual arguments allowed is 32
#define MAX_ARGS 32

// function to use fork
int forkThis(int somePID) {
	somePID = fork();

	if (somePID < 0) {
		fprintf(stderr, "Fork error\n");
		exit(1);
	}

	return somePID;
}

// function to get the total time from the struct timeval
long int convertMilli(struct timeval someStruct) {
	long int sec1 = someStruct.tv_sec * 1000;
	long int sec2 = someStruct.tv_usec / 1000;

	return sec1 + sec2;
}

// function to remove newline character at inputted string
char *removeNewLine(char *string) {
	int length = strlen(string);

	// check to see if the end of string is a newline character,
	// and if yes then replace will null character
	if (string[length - 1] == '\n' && length > 0) {
		string[length - 1] = '\0';
	}

	return string;
}

// function to parse keyboard inputted string
void parseString(char *argV[MAX_ARGS], char string[MAX_CHAR]) {
	// declare token to be used in strtok
	char *tok;
	int i = 0;

	// any time there's a SPACEBAR space, delimit it from the parsed string
	tok = strtok(string, " ");

	// this while loop retrieves each parsed string and places it
	// into each index in argvNew. This while loop also makes sure
	// that the last index in argvNew is NULL
	while (tok != NULL && i <= MAX_ARGS) {
		argV[i] = tok;
		argV[i+1] = NULL;
		tok = strtok(NULL, " ");
		i++;
	}
}

// function to print statistics
void printStats(char *argvDisplay[MAX_ARGS], long int presTime, long int pasTime) {
	// declare a struct rusage to be used by getrusage
	struct rusage aRusage;

	// Initial title saying what command was used 
	// and that the following are the system resources used
	printf("\nThe command %s", argvDisplay[0]);
	printf(" used the following system resources:\n\n");
	printf("____________________________________________________________________\n");

	// retrieves the CPU time used, for both user and system
	getrusage(RUSAGE_CHILDREN, &aRusage);
	long int usr_time = convertMilli(aRusage.ru_utime);
	long int sys_time = convertMilli(aRusage.ru_stime);

	// calculates elapsed wall clock time
	long int elapsedTime = presTime - pasTime;

	// retrieves the number of times process was preempted involuntarily
	long int getInvol = aRusage.ru_nivcsw;
	// retrieves the number of times process voluntarily gave up the CPU
	long int getVol = aRusage.ru_nvcsw;
	// retrieves the number of page faults
	long int getPageFlt = aRusage.ru_majflt;
	// retrieves the number of page faults that could be serviced by reclaiming memory
	long int getPageFltServ = aRusage.ru_minflt;

	// prints all the statistics used by the command
	printf("CPU time used (user): %ldms\n", usr_time);
	printf("CPU time used (system): %ldms\n", sys_time);
	printf("Elapsed wall clock time: %ldms\n", elapsedTime);
	printf("Number of times process was preempted involuntarily: %ld\n", getInvol);
	printf("Number of times process voluntarily gave up CPU: %ld\n", getVol);
	printf("Number of page faults: %ld\n", getPageFlt);
	printf("Number of page faults (could be satisfied by reclaiming memory): %ld\n\n", getPageFltServ);
}

int main(int argc, char *argv[]) {
	// argc is number of arguments
	// argv is array of strings

	// declare argvNew array of strings which will be used
	// by our processes
	char *argvNew[MAX_ARGS];

	// declare int to be used for fork function
	int randomPID;

	// these variables will be used to calculate elapsed wall clock time
	long int pastTime;
	long int presentTime;
	struct timeval tv1;
	struct timeval tv2;

	// declare the maximum character size for userInput string
	char userInput[MAX_CHAR];

	// for case where user wants to enter shell command input
	if (argc == 1) {

		// while loop which continuously asks for user for keyboard input,
		// until the user exits the program.
		// I put the condition argvNew != NULL to 
		// just simply have the while loop continuously loop
		while (1) {
			printf("Please type in a command along with any necessary parameters or path\n==>");

			// read user keyboard input, place into userInput
			fgets(userInput, MAX_CHAR, stdin);

			// removes the newline character of userInput string
			removeNewLine(userInput);

			// if the keyboard input is exit, then return to shell prompt
			if (strcmp(userInput, "exit") == 0) {
				exit(0);
			}

			// parses the entire userInput string
			parseString(argvNew, userInput);

			// if the keyboard input is cd, then change the current directory
			// to the one specified. Once complete, redo while loop
			if (strcmp(argvNew[0],"cd") == 0) {
				chdir(argvNew[1]);
				continue;
			}

			// Get wall clock time before fork command
			gettimeofday(&tv1, NULL);
			pastTime = convertMilli(tv1);

			// the pid variable is the result from forking
			int pid = forkThis(randomPID);

			// child process
			if (pid == 0) {

				if (execvp(argvNew[0], argvNew) < 0) {
					fprintf(stderr, "Execvp error\n");
					exit(1);
				}
				// execvp wipes the child process after it executes

			}

			// parent process
			else {

				// wait for child process to finish
				wait(0);

				// This gets the wall clock time after command gets executed
				gettimeofday(&tv2, NULL);
				presentTime = convertMilli(tv2);

				// prints statistics of inputted command
				printStats(argvNew, presentTime, pastTime);
			}
		}
	}

	// for case where user will type out command in addition to the doit program
	else {
		int i = 0;

		// retrieve the necessary command and parameters 
		while (argv[i] != NULL) {
			argvNew[i] = argv[i+1];
			argvNew[i+1] = NULL;
			i++;
		}

		// Get wall clock time before fork command
		gettimeofday(&tv1, NULL);
		pastTime = convertMilli(tv1);

		// the pid variable is the result from forking
		int pid = forkThis(randomPID);

		// child process
		if (pid == 0) {

			if (execvp(argvNew[0], argvNew) < 0) {
				fprintf(stderr, "Execvp error\n");
				exit(1);
			}
			// execvp wipes the child process after it executes
		}

		// parent process
		else {

			// wait for child process to finish
			wait(0);

			// This gets the wall clock time after command gets executed
			gettimeofday(&tv2, NULL);
			presentTime = convertMilli(tv2);

			// prints statistics of inputted command
			printStats(argvNew, presentTime, pastTime);
		}
	}
	exit(0);
}
