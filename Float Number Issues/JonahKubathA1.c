/*
* Name: Jonah JonahKubath
* Assignment: A1 Number Demonstrations
* Date: 1/22/2018
*
* Description: Work with values and notice the odd behavior that Binary
* can cause.
*/

#include <stdio.h>

int main(void){
  //Problem 1:
  printf("Problem 1:\n");
  printf("Expected - 3.6000000000\n");
  float temp = 3.6;

  printf("%0.10f\n", temp);

  //Problem 2:
  printf("\nProblem 2:\n");
  printf("Expected - 0.1\n");
  temp = (1.0/10.0);
  printf("%0.10f\n", temp);

  //Problem 3:
  printf("\nProblem 3:\n");
  double temp1 = 1/2;
  printf("Expected - 0.5\n");
  printf("%e\n", temp1);

  temp1 = (1.0/2.0);
  printf("Expected - 0.5\n");
  printf("%e\n", temp1);

  //Problem 4:
  printf("\nProblem 4:\n");
  printf("Expected - 9999999.4499999999\n");
  temp1 = 9999999.4499999999;
  printf("Double - %e\n", temp1);

  printf("Float - %f\n", (float) temp1);

  //Problem 5:
  printf("\nProblem 5:\n");
  printf("Expected - 900,000,000\n");
  int temp2 = 30000*30000;
  printf("%d\n", temp2);

  printf("Expected - 1,600,000,000\n");
  temp2 = 40000*40000;
  printf("%d\n", temp2);

  printf("Expected - 2,500,000,000\n");
  temp2 = 50000*50000;
  printf("%d\n", temp2);

  printf("Expected - 3,600,000,000\n");
  temp2 = 60000*60000;
  printf("%d\n", temp2);

  //problem 6:
  printf("\nProblem 6:\n");
  printf("Expected - 100,000,000,000,000,000,000\n");
  float temp3 = 1e20;
  printf("%f\n", temp3);

  printf("Expected - 100,000,000,003,500,000,000\n");
  temp3 = (1e20 + 3500000000);
  printf("%f\n", temp3);

  printf("Expected - 103,500,000,000,000,000,000\n");
  temp3 = (1e20 + (3500000000 * 1000000000));
  printf("%f\n", temp3);

  temp3 = 1e20;
  for(int i = 0; i < 1000000000; i++){
    temp3 += 3500000000;
  }
  printf("%f\n", temp3);



  return 0;
}
