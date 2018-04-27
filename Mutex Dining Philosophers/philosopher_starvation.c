/*
 * Author: Jonah Kubath
 * Class: CS5541 A5
 * Date: March 21, 2018
 * Summary:  This is the starvation implementation of the Dining Philosopher Problem.
 * Starvation will be caused when a philosopher does not wait after putting down the forks
 * to quickly pick the forks back up and start eating.  This will cause the other philosophers
 * to wait until the philosopher is done eating before they can eat.
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

//Mutex used to block when a philosopher has waited too long to eat
sem_t *change; //Used to lock the tables when making changes to the forks array

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


	printf("Main process done\n");
	return 0;

}


void *inThread(void *pid) {
	int threadIndex = (int) pid;
	int myFood = food;
	int left = threadIndex;
	int right = (threadIndex + 1) % philCount;

	while(myFood > 0) {
		//Lock for changes
		//sem_wait(change);

		if(phil[threadIndex] == HUNGRY && forks[left] == DOWN && forks[right] == DOWN){
			/* Pick up the forks to eat */
			phil[threadIndex] = EATING;
			forks[left] = UP;
			forks[right] = UP;

			//Unlocked for chagnes
			//sem_post(change);

			//Resources are available and not currently blocked
			eat(threadIndex, left, right, &myFood);

		}
		else {
			//sem_post(change);
			//Wait for resources to open
			sleep(2);
		}

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

	//sem_wait(change);
	forks[left] = DOWN;
	forks[right] = DOWN;
	//sem_post(change);

	printf("%d done\n", threadIndex);
}