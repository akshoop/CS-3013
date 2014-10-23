External document for Alex Shoop's CS 3013 project 1 program.

This program, called doit, allows the user to run any command after calling the program.
After executing said command, the program will print some system resources that the
command used. The printed statistics are listed below:
- CPU time used, for both user and system (in milliseconds)
- elapsed wall-clock time for command to execute (in milliseconds)
- number of times the process was preempted involuntarily
- number of times the process gave up the CPU voluntarily
- number of page faults
- number of page faults that could be satisfied by reclaiming memory
For example, typing and entering "./doit cat /etc/motd" without quotes will print the message
of the day and list the system resources that "cat" used.

This program also allows a shell program environment.
To start the shell program, start the program with no argument (ie. "./doit" without quotes).
A prompt will show up asking for a command.
Here you can type and enter any command just as before, such as "cat /etc/motd" or "ls" or "sleep 2".
The system resource statistics will be printed for each command.
Within this shell program, it will continuously ask for a new command.
To exit, just enter "exit".
Furthermore, this shell program lets the user change directories.
To do so, type and enter "cd FILEPATH" without quotes where FILEPATH is the specific path
you want to change directory to.

This program does not have much error checking, for example if you type nothing while in shell,
it will display an Execvp error.
