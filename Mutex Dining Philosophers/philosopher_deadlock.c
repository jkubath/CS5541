/*
 * Author: Jonah Kubath
 * Class: CS5541 A5
 * Date: March 21, 2018
 * Summary:  This is the deadlock implementation of the Dining Philosopher Problem.
 * This program will deadlock because after picking up a fork, the philosopher will
 * pause before picking up the next fork.  This will allow time for the other philosophers
 * to pick up their forks.  Everyone will then have one fork and never put it down causing deadlock.
*/

void *inThread(void *pid);
void eat(int theadIndex, int left, int right, int *threadFood);

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>

//Defines for making code easier to read
//Philosophers
#define EATING 0
#define HUNGRY 1
#define DONE 2
//forks
#define DOWN 0
#define UP 1

//Global variables used to change parameters
int philCount = 5; //Number of philosophers
int food = 10; //Total food for each person
int eatLength = 5; //Length to eat when both forks are available

//Forks
int forks[5] = {0, 0, 0, 0, 0}; //Forks start down
//Philosophers
int phil[5] = {1, 1, 1, 1, 1}; //All philosophers start by being hungry

//Count the number of times that the threads are waiting for resources
int waiting = 0;
int deadlockValue = 20;


int main(int argc, char **argv) {

	//Array to hold the threads of the philosophers
	pthread_t phil[philCount];

	/* Create the threads */
	for(int i = 0; i < philCount; i++){
		//Create the thread and call inThread()
		if(pthread_create(&phil[i], NULL, inThread,(void *) (size_t) i)){
			fprintf(stderr, "Error creating thread %d\n", i);
		}
	}

	/* Join the threads */
	for(int i = 0; i < philCount; i++){
		if(pthread_join(phil[i], NULL)) {
			fprintf(stderr, "Error joining thread %d\n", i);
		}
	}

	if(waiting > deadlockValue){
		printf("Error: Deadlock\n");
	}


	printf("Main process done\n");
	return 0;

}


void *inThread(void *pid) {
	int threadIndex = (int) pid;
	int myFood = food;

	int left = threadIndex;
	int right = (threadIndex + 1) % philCount;

	printf("%d found a seat\n", threadIndex);

	while(myFood > 0) {
		//Don't Lock for changes
		//sem_wait(change);

		/* Pick up the forks to eat */
		phil[threadIndex] = EATING;
		forks[left] = UP;

		//Sleep for 3 seconds - causing deadlock
		sleep(3);

		while(forks[right] != DOWN){
			waiting++;
			//printf("%d - is waiting for the right fork\n", threadIndex);
			sleep(1);

			//Break out of deadlock
			if(waiting > deadlockValue){
				return NULL;
			}

		}
		//Reset waiting as deadlock has not been reached
		waiting = 0;

		forks[right] = UP;

		//Don't Unlock for chagnes
		//sem_post(change);

		//This philosopher has finished eating
		if(myFood == 0){
			phil[threadIndex] = DONE;
			printf("%d has finished eating\n", threadIndex);
		}

		/* Used in testing to make reading output easier */
		//sleep(3);

		}

	return NULL;
}


void eat(int threadIndex, int left, int right, int *threadFood) {
	int totalEat = 0;

	while(totalEat < eatLength){
		//Eat
		(*threadFood)--;
		totalEat++;
		printf("%d - Eating %d\tforks(%d, %d)\n", threadIndex, *threadFood, left, right);

	}

	//Put the forks down
	phil[threadIndex] = HUNGRY;
	forks[left] = DOWN;
	forks[right] = DOWN;

	printf("%d done\n", threadIndex);
}