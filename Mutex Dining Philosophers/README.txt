Date: 04/07/2018
Class: CS5541
Assignment: A5 Dining Philosophers
Author(s): Jonah Kubath

gcc -Wall -std=c99 -o philosopher philosopher.c
gcc -Wall -std=c99 -o philosopher_deadlock philosopher_deadlock.c
gcc -Wall -std=c99 -o philosopher_starvation philosopher_starvation.c

make all
make use //Runs the working implementation
make use1 //Runs the deadlock program
make use2 //Runs the starvation program

Warnings: No warnings should be thrown

References:
	https://www.geeksforgeeks.org/operating-system-dining-philosopher-problem-using-semaphores/