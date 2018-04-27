/*
 * Author: Jonah Kubath
 * Class: CS5541 A5
 * Date: March 21, 2018
 * Summary:  This is the working implementation of the Dining Philosopher Problem
 * Resources: https://www.geeksforgeeks.org/operating-system-dining-philosopher-problem-using-semaphores/
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

	int threadIndex = (int) pid; //ID of the philosophers
	int myFood = food; //The total food the philosopher should eat
	int totalSkipped = 0; //How many times the philosopher was not able to eat due to resource availibility

	int left = threadIndex; //Left fork
	int right = (threadIndex + 1) % philCount; //Right fork

	printf("%d has found a seat\n", threadIndex);

	while(myFood > 0) {
		//Lock for changes
		sem_wait(change);

		if(phil[threadIndex] == HUNGRY && forks[left] == DOWN && forks[right] == DOWN){
			/* Pick up the forks to eat */
			phil[threadIndex] = EATING;
			forks[left] = UP;
			forks[right] = UP;

			//Unlocked for chagnes
			sem_post(change);

			//Resources are available and not currently blocked
			eat(threadIndex, left, right, &myFood);

		}
		else {
			//Relieve resources if previously blocked
			sem_post(change);

			totalSkipped++;
			//printf("\t%d skipped eating (count - %d)\n", threadIndex, totalSkipped);

			//One of the philosophers has been skipped a lot
			if(totalSkipped > 2){
				//Block so the current philosopher will eat next
				sem_wait(change);

				//printf("%d has blocked the sem\n", threadIndex);

				//Wait until the resources have cleared
				while(forks[left] != DOWN || forks[right] != DOWN){
					//Do nothing
					//printf("\t%d waiting for resources\n", threadIndex);
				}

				/* Pick up the forks and eat */
				phil[threadIndex] = EATING;
				forks[left] = UP;
				forks[right] = UP;

				//Release the blocked semaphore
				sem_post(change);

				//Then eat, we have been waiting
				eat(threadIndex, left, right, &myFood);

				//Reset totalSkipped
				totalSkipped = 0;

			}
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

	printf("%d - Eating\tforks(%d, %d)\n", threadIndex, left, right);

	while(totalEat < eatLength){
		//Eat
		(*threadFood)--;
		totalEat++;
		//printf("%d - Eating %d\tforks(%d, %d)\n", threadIndex, *threadFood, left, right);

	}

	//Put the forks down
	phil[threadIndex] = HUNGRY;
	forks[left] = DOWN;
	forks[right] = DOWN;

	//printf("%d eating slice done\n", threadIndex);
}
