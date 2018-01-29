#include <stdio.h>
#include "bitslab.h"
int main() {
  printf("Test with main.\n");
  printf("bitAnd Result: %d\n", bitAnd(6,5));
  printf("getByte result: %x\n", getByte(0x12345678,0));
  printf("negate results: %d\n", negate(-11));
  printf("isPositive result: %d\n", isPositive(-11));
  printf("tmin result: %d\n", tmin());
  printf("bang result: %d\n", bang(0));
  printf("fitsbits result: %d\n", fitsBits(5, 3));
}
