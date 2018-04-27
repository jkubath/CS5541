/*
* Author: Jonah Kubath
* Assignment 2 CS5541 Bitslab
* 2/7/2018
* Summary: This is the test file that calls the methods in bitslab.c.
* Usage: Tests can be made by changing the numbers in the functions.  For the
* float functions, change the float defined at line 19 and 37 so that the actual
* prints will match.  A union is used to interpret the float as the encoding.
*/
#include <stdio.h>
#include "bitslab.h"

union myUnion {
  float f;
  unsigned int i;
};

int main() {
  union myUnion data;
  data.f = 20;

  printf("Test with main.\n");
  // printf("bitAnd Result: %d\n", bitAnd(-1,0));
  printf("getByte result: %x\n", getByte(0x12345678,3));
  // printf("negate results: %d\n", negate(0));
  // printf("isPositive result: %d\n", isPositive(0));
  // printf("tmin result: %d\n", tmin());
  // printf("bang result: %d\n", bang(0));
  // printf("fitsbits result: %d\n", fitsBits(-4, 3));
  printf("Logical shift: %08x\n", logicalShift(0xff54ac76,8));
  // printf("Bits count: %d\n", bitCount(-1));
  // printf("Divpwr2: %d\n", divpwr2(36, 3));
  // printf("isLessOrEqual: %d\n", isLessOrEqual(2, -1));
  // printf("float_neg: 0x%08x\n", float_neg(data.i));
  // data.f = data.f * -1;
  // printf("Actual neg : 0x%08x\n",  (data.i));
  //
  // data.f = 20;
  // printf("float_i2f: 0x%08x\n", float_i2f(data.f));
  // printf("Actual float : 0x%08x\n",  data.i);
  //
  //
  // printf("float_twice: 0x%08x\n", float_twice(data.i));
  // data.f = data.f * 2;
  // printf("Actual x 2 : 0x%08x\n", data.i);
}
