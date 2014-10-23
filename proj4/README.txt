README for Alexander Shoop CS 3013 Project 4

To compile:
make

To clean:
make clean

To run:
./proj4 SOME_FILE [CUSTOM_CHUNK | mmap | pN]

You input some source file within the same directory (such as proj4.c or proj4) to get 
the file size, the number of strings of length above 4, and the maximum string length.

For the optional arguments, you can put a custom chunk size between 1 and 8192 bytes.
You can also request using the memory map method with the word "mmap".
If you type the letter 'p' and then a number between 1 and 16, you can specify to use
the memory map method but with parellel threads, where the number after 'p' is the 
number of requested threads.

Unfortunately I failed to properly implement the parallel threads method.
I received some guidance and assistance from Batyr Nurbekov, but it still will not
properly print out the maximum string length.

I also was unable to accomplish the performance analysis part of the project.

Thank you.
