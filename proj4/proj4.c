// Alexander Shoop - CS 3013 Project 4
// Received parallelization guidance and assistance from Batyr Nurbekov

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

// maximum number of threads that can be created
#define MAXTHREAD 16

#define BUFFSIZE 1024
#define CHUNKLIM 8192
#define RANGE 1
// message types for child thread to send
#define STARTS_WITH_PRINTABLE_CHAR 2
#define ENDS_WITH_PRINTABLE_CHAR 3
#define STARTS_AND_ENDS_WITH_PRINTABLE_CHAR 4
#define DOESNT_START_OR_END_WITH_PRINTABLE_CHAR 5
#define STRING_EXTENDS_ACROSS_CHUNK 6

// message struct
struct msg {
	int type;
	int value1;
	int value2;
	int value3;
	int value4;
};

FILE *fp;
// array of threads, based on user inputted number of threads
pthread_t t[MAXTHREAD + 1];
struct msg records[MAXTHREAD + 1];
char *thisPCHfile;
struct stat someStat;

// prototype functions
void updateCurrentCountStats(int wordLen, int *strings_4, int *max_string);
int countWords(int lower_val, int upper_val, int *strings_4, int *max_string, int *start_word_length, int *end_word_length);
void *init_thread(void *arg);
void thread_range(int inputThread);
void combineResults(int inputThread, int *strings_4, int *max_string);

int main(int argc, char *argv[]) {
	int size_of_file = 0, strings_4 = 0, temp_max = 0, max_string = 0;
	int file_desc, cnt, i, custom_byte_size, second_arg = 0, mmap_set = 0, parallel_set = 0;
	int inputThread = 0;
	char *token;

	// general usage check
	if (argc < 2) {
		fprintf(stderr, "Error: usage is ./proj4 SRC_FILE [SIZE|mmap]\n");
		exit(1);
	}

	// open the source file
	if ((file_desc = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "Error: file open issue\n");
		exit(1);
    }

    if (argc > 2) {
	    // check if optional 2nd argument is an integer
	    if (isdigit(*argv[2]) > 0) {
	    	custom_byte_size = atoi(argv[2]);
	    	printf("this is custom chunk %d\n", custom_byte_size);
	    	if (custom_byte_size > 8192) {
	    		printf("Cannot have chunk size limit of greater than 8192 bytes\n");
	    	}
	    	else {
		    	second_arg = 1;
		    }
	    }

	    if (strcmp("mmap", argv[2]) == 0) {
	    	second_arg = 1;
	    	mmap_set = 1;
	    }

	    if (argv[2][0] == 'p') {
		    token = strtok(argv[2], "p");
			if (token != NULL) {
			    inputThread = atoi(token);
			    if (inputThread > MAXTHREAD) {
			    	fprintf(stderr, "Error: Can't have more than 16 parallel threads\n");
			    	exit(1);
			    }

			    if (second_arg == 0) {
			    	second_arg = 1;
			    	mmap_set = 1;
			    	parallel_set = 1;
			    }
			}
		}
	}

    // for case where user does not specify custom argument
    if (second_arg == 0) {
    	char str[BUFFSIZE];
    	//printf("we're in no second arg\n");
		while ((cnt = read(file_desc, str, BUFFSIZE)) > 0) {
			// keep track of file size 
			size_of_file += cnt;
	        for (i = 0; i < cnt; i++) {
	        	// check if current character is a printable-character or if current character is a space, newline, tab, etc.
	        	if (isprint(str[i]) > 0 || isspace(str[i]) > 0) {
	        		// keep track of a temporary maximum string length variable
	        		temp_max++;
	        		if (temp_max == 4) {
	        			strings_4++;
	        		}
	        	}
	        	else {
	        		if (temp_max > max_string) {
	        			max_string = temp_max;
	        		}
	        		temp_max = 0;
	        	}
	    	}
    	}

    	// check if we're at end of file
	    if ((cnt = read(file_desc, str, BUFFSIZE)) == 0) {
	    	if (temp_max > max_string) {
	    		max_string = temp_max;
	    	}
	    }
    }

    // for case where user only specifies custom chunk size
    if (second_arg == 1 && mmap_set == 0 && parallel_set == 0) {
    	char str[custom_byte_size];
    	printf("we're in second arg chunk\n");
    	while ((cnt = read(file_desc, str, custom_byte_size)) > 0) {
			// keep track of file size 
			size_of_file += cnt;
	        for (i = 0; i < cnt; i++) {
	        	// check if current character is a printable-character or if current character is a space, newline, tab, etc.
	        	if (isprint(str[i]) > 0 || isspace(str[i]) > 0) {
	        		// keep track of a temporary maximum string length variable
	        		temp_max++;
	        		if (temp_max == 4) {
	        			strings_4++;
	        		}
	        	}
	        	else {
	        		if (temp_max > max_string) {
	        			max_string = temp_max;
	        		}
	        		temp_max = 0;
	        	}
	    	}
    	}

    	// check if we're at end of file
	    if ((cnt = read(file_desc, str, BUFFSIZE)) == 0) {
	    	if (temp_max > max_string) {
	    		max_string = temp_max;
	    	}
	    }
    }

    // for case where user only specifies mmap functionality
    if (second_arg == 1 && mmap_set == 1 && parallel_set == 0) {
    	//printf("we're in second arg mmap\n");

    	if (fstat(file_desc, &someStat) < 0) {
			fprintf(stderr, "Error: fstat error\n");
			exit(1);
    	}

    	if ((thisPCHfile = (char *) mmap (NULL, someStat.st_size, PROT_READ, MAP_SHARED, file_desc, 0))	== (char *) -1)	{
			fprintf(stderr, "Error: could not map file\n");
			exit(1);
		}

		// traverse entire file contents
		for (i = 0; i < someStat.st_size; i++) {
			// check if current character is a printable-character or if current character is a space, newline, tab, etc.
        	if (isprint(thisPCHfile[i]) > 0 || isspace(thisPCHfile[i]) > 0) {
        		// keep track of a temporary maximum string length variable
        		temp_max++;
        		if (temp_max == 4) {
        			strings_4++;
        		}
        	}
        	else {
        		if (temp_max > max_string) {
        			max_string = temp_max;
        		}
        		temp_max = 0;
        	}
		}

		// end of file check
		if (temp_max > max_string) {
			max_string = temp_max;
		}

		size_of_file = someStat.st_size;

		// Now clean up
	    if (munmap(thisPCHfile, someStat.st_size) < 0){
			fprintf(stderr, "Error: could not unmap memory\n");
			exit(1);
	    }

    }

    // for case where user specifies using mmap with parallel threads
    if (second_arg == 1 && mmap_set == 1 && parallel_set == 1) {

    	// printf("we're in second arg mmap parallel threads\n");
    	// printf("Number of inputThread is %d\n", inputThread);

    	if (fstat(file_desc, &someStat) < 0) {
			fprintf(stderr, "Error: fstat error\n");
			exit(1);
    	}

    	if ((thisPCHfile = (char *) mmap (NULL, someStat.st_size, PROT_READ, MAP_SHARED, file_desc, 0))	== (char *) -1)	{
			fprintf(stderr, "Error: could not map file\n");
			exit(1);
		}

		// pthread_create related
		thread_range(inputThread);

		// pthread_join related stuff
		for (i = 1; i < inputThread + 1; i++) {
			(void)pthread_join(t[i], NULL);
		}

		//loop through the array with results and combine the results
		combineResults(inputThread, &strings_4, &max_string);

		size_of_file = someStat.st_size;

		// Now clean up
	    if (munmap(thisPCHfile, someStat.st_size) < 0){
			fprintf(stderr, "Error: could not unmap memory\n");
			exit(1);
	    }

    }

    close(file_desc);

	printf("File size: %d bytes\n", size_of_file);
	printf("Strings of at least length 4: %d\n", strings_4);
	printf("Maximum string length: %d bytes\n", max_string);
	return 0;
}

// update the count of strings of length 4 and the maximum length
void updateCurrentCountStats (int wordLen, int *strings_4, int *max_string) {
	if (wordLen > 3) {
		(*strings_4)++;
		if (wordLen > *max_string) {
			*max_string = wordLen;
		}
	}
}

// counts the number of full strings 
int countWords(int lower_val, int upper_val, int *strings_4, int *max_string, int *start_word_length, int *end_word_length) {
	int i;
	int wordLen = 0;

	// Flag that indicates that the length of the word is being currently counted
	int isCurrentlyCountingWordLen = 0;
	int isCurrentlyCountingStartingWordLen = 0;
	int doesChunkStartWithWord = 0;
	int doesChunkEndWithWord = 0;
	int doesStringExtendAcrossChunk = 0;
	int result;

	*strings_4 = 0;

	for (i = lower_val; i < upper_val + 1; i++) {
		if (isprint(thisPCHfile[i]) > 0 || isspace(thisPCHfile[i]) > 0) { 
			//if the the sequence of printable characters is continued
			if (isCurrentlyCountingWordLen) { 
				wordLen++;
			}
			//if new printable character sequence is detected
			else { 
				if (i == lower_val) {
					isCurrentlyCountingStartingWordLen = 1;
				}

				wordLen = 1;
				isCurrentlyCountingWordLen = 1;
			}
		}
		else {
			// if the end of the printable character sequence is detected
			if (isCurrentlyCountingWordLen) { 
				if (isCurrentlyCountingStartingWordLen) {
					isCurrentlyCountingStartingWordLen = 0;

					*start_word_length = wordLen;
					doesChunkStartWithWord = 1;
				}
				else {
					updateCurrentCountStats(wordLen, strings_4, max_string);
				}

				isCurrentlyCountingWordLen = 0;
			}
		}
	}

	// if the end of the printable character sequence is detected at the end of chunk
	if (isCurrentlyCountingWordLen) { 
		if (isCurrentlyCountingStartingWordLen) {
			*start_word_length = wordLen;

			doesStringExtendAcrossChunk = 1;
		}
		else {
			*end_word_length = wordLen;

			doesChunkEndWithWord = 1;
		}
	}

	if (doesStringExtendAcrossChunk) {
		result = STRING_EXTENDS_ACROSS_CHUNK;
	}
	else if (doesChunkEndWithWord && doesChunkStartWithWord) {
		result = STARTS_AND_ENDS_WITH_PRINTABLE_CHAR;
	}
	else if (doesChunkEndWithWord) {
		result = ENDS_WITH_PRINTABLE_CHAR;
	}
	else if (doesChunkStartWithWord) {
		result = STARTS_WITH_PRINTABLE_CHAR;
	}
	else {
		result = DOESNT_START_OR_END_WITH_PRINTABLE_CHAR;
	}

	return result;
}

// function that thread actually does
void *init_thread(void *arg) {
	int lower_val, upper_val, strings_4, max_string, msg_type, start_word_length, end_word_length;
	long threadId = (long)arg;
	struct msg result_msg;

	lower_val = records[threadId].value1;
	upper_val = records[threadId].value2;

	// count words between upper bound and lower bound
	msg_type = countWords(lower_val, upper_val, &strings_4, &max_string, &start_word_length, &end_word_length);

	result_msg.type = msg_type;
	result_msg.value1 = strings_4;
	result_msg.value2 = max_string;
	result_msg.value3 = start_word_length;
	result_msg.value4 = end_word_length;

	records[threadId] = result_msg;
}

void thread_range(int inputThread) {
	int lower_bound, upper_bound, multiplier;
	long i;
	struct msg message;

	if (someStat.st_size % inputThread == 0) {
		multiplier = someStat.st_size / inputThread;
	}
	else {
		multiplier = someStat.st_size / inputThread + 1;
	}

	//create a thread , passing in upper and lower bounds it should use
	for (i = 1; i < inputThread + 1; i++) {
		lower_bound = (i - 1) * multiplier + 1;
		upper_bound = i * multiplier;

		if (upper_bound > someStat.st_size) {
			upper_bound = someStat.st_size;
		}

		message.type = RANGE;
		message.value1 = lower_bound - 1;
		message.value2 = upper_bound - 1;

		records[i] = message;

		if (pthread_create(&t[i], NULL, init_thread, (void *)i) != 0) {
			perror("pthread_create");
			exit(1);
		}
	}
}

// combine the results found by parallel threads
void combineResults(int inputThread, int *strings_4, int *max_string) {
	int i, resultType, tempWordLen;
	int spanningWordLen = 0;
	int isCurrentlyCountingSpanningWordLen = 0;

	int chunkStartsWithWord;
	int chunkEndsWithWord;
	int previousChunkLength = 0;
	int previousChunkEndedWithWord = 0;

	for (i = 1; i < inputThread + 1; i++) {
		resultType = records[i].type;

		if (resultType == STRING_EXTENDS_ACROSS_CHUNK) {
			if (!isCurrentlyCountingSpanningWordLen) {
				isCurrentlyCountingSpanningWordLen = 1;
			}
			spanningWordLen += records[i].value3;
		}
		else {
			switch (resultType) {
				case STARTS_AND_ENDS_WITH_PRINTABLE_CHAR:
					chunkStartsWithWord = 1;
					chunkEndsWithWord = 1;
					break;
				case STARTS_WITH_PRINTABLE_CHAR:
					chunkStartsWithWord = 1;
					chunkEndsWithWord = 0;
					break;
				case ENDS_WITH_PRINTABLE_CHAR:
					chunkStartsWithWord = 0;
					chunkEndsWithWord = 1;
					break;
				default:
					chunkStartsWithWord = 0;
					chunkEndsWithWord = 0;
					break;
			}

			if (chunkStartsWithWord) {
				if (isCurrentlyCountingSpanningWordLen) {
					spanningWordLen += records[i].value3;

					updateCurrentCountStats(spanningWordLen, strings_4, max_string);
				}
				else
				{
					if(previousChunkEndedWithWord) {
						tempWordLen = previousChunkLength + records[i].value3;

						updateCurrentCountStats(tempWordLen, strings_4, max_string);
					}
					else {
						updateCurrentCountStats(records[i].value3, strings_4, max_string);
					}
				}
			}

			if (!chunkStartsWithWord) {
				if (isCurrentlyCountingSpanningWordLen) {
					updateCurrentCountStats(spanningWordLen, strings_4, max_string);
				}
				else
				{
					if(previousChunkEndedWithWord) {
						updateCurrentCountStats(previousChunkLength, strings_4, max_string);
					}
				}
			}

			if (chunkEndsWithWord) {
				previousChunkEndedWithWord = 1;
				previousChunkLength = records[i].value4;
			}

			if (!chunkEndsWithWord) {
				previousChunkEndedWithWord = 0;
			}

			//add values calculated by thread to the results
			*strings_4 += records[i].value1;
			if (records[i].value2 > *max_string) {
				*max_string = records[i].value2;
			}

			spanningWordLen = 0; //make the spanning word length equal to zero
								 //if the chunk with no spanning word is encountered
		}
	}

	if (isCurrentlyCountingSpanningWordLen) {
		updateCurrentCountStats(spanningWordLen, strings_4, max_string);
	}
	else
	{
		if(previousChunkEndedWithWord) {
			updateCurrentCountStats(previousChunkLength, strings_4, max_string);
		}
	}
}
