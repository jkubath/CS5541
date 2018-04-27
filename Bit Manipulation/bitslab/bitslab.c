#include <stdio.h>
/*
 * CS5541 Bits Lab (Assignment 2)
 *
 * Jonah Kubath
 * 2/5/2018
 *
 * The bang function was taken from an in class work sheet
 * The bit count was taken from https://stackoverflow.com/questions/3815165/how-to-implement-bitcount-using-only-bitwise-operators
 * The float_i2f rounding section was taken from
 *  https://github.com/wky/ICS-labs/blob/master/data/bits.c
 *
 * bitslab.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to Assignment 2 by
editing the collection of functions in this source file.

INTEGER CODING RULES:

  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code
  must conform to the following style:

  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>

  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function.
     Note that '=' is not counted; you may use as many of these as you
     want without penalty.
  2. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 */


#endif
/*
 * bitAnd - x&y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Points: 2
 */
int bitAnd(int x, int y) {
  /*
  * When ~() is applied to variables it changes the individual items to ~x
  * and also changes the logic.  This allows the 'and' to be changed to 'or'
  */
  return ~(~x | ~y);
}
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Points: 2
 */
int getByte(int x, int n) {
  /*
  * This shifts the current x to the left to remove the high bits.
  * Once the top two bytes are at the highest order, the high orders bits are
  * shifted to the right until they are the lowest order.
  */
  x = x << ((3 + (~n + 1)) << 3);
  x = x >> 24;
  //Forgot to mask off for a negative one shift
  x = x & 0xFF;
  return x;
}
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Points: 3
 */
int logicalShift(int x, int n) {
  /*The goal of this function is to perform an arithmetic shift and then mask
  * off the added 1's if the value would be negative
  */
  x = x >> n;
  int mask = 1 << 31;
  //Shifted one to many bits
  mask = mask >> (n - 1);
  mask = ~mask;
  return (x & mask);
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Points: 4
 */
int bitCount(int x) {
  /*
  *Summary: The basic principle is to count bits by power of twos.
  * The first steps counts right bit in pairs of two by masking off
  * the left bit with 5 (0101).  The left bit is covered by shifting the
  * number to the right one bit (divide by 2) and then using the same mask.
  * This method is continued with the sequence of bits going up by a power of 2
  * each time.  1, 2, 4, 8, 16
  *Example: 395 in binary
  *0000000110001011 (0 0 0 0 0 0 0 1 1 0 0 0 1 0 1 1)
  *After the first step I have:
  *0000000101000110 (0+0 0+0 0+0 0+1 1+0 0+0 1+0 1+1) = 00 00 00 01 01 00 01 10
  *In the second step I have:
  *0000000100010011 ( 00+00   00+01   01+00   01+10 ) = 0000 0001 0001 0011
  *In the fourth step I have:
  *0000000100000100 (   0000+0001       0001+0011   ) = 00000001 00000100
  *In the last step I have:
  *0000000000000101 (       00000001+00000100       )
  */
  unsigned firstMask = (0x55 << 8) | (0x55);
  firstMask = firstMask | (firstMask << 16);
  unsigned secondMask = (0x33 << 8) | (0x33);
  secondMask = secondMask | (secondMask << 16);
  unsigned thirdMask = (0x0F << 8) | (0x0F);
  thirdMask = thirdMask | (thirdMask << 16);
  unsigned fourthMask = 0xFF;
  fourthMask = fourthMask | (fourthMask << 16);
  unsigned fifthMask = (0xFF << 8) | (0xFF);
  //fifthMask = fifthMask | (fifthMask << 16;)
  int new = 0;
  //Mask off every other bit
  new = (x & firstMask) + ((x >> 1) & firstMask);
  //Mask off half of the byte - 2 bits
  new = (new & secondMask) + ((new >> 2) & secondMask);
  //Mask off an entire byte - 4 bits
  new = (new & thirdMask) + ((new >> 4) & thirdMask);
  //Mask off two bytes - 8 bits
  new = (new & fourthMask) + ((new >> 8) & fourthMask);
  //Mask off 4 bytes - 16 bits
  new = (new & fifthMask) + ((new >> 16)& fifthMask);

  return new;
}
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Points: 4
 */
int bang(int x) {
  /*
  * The middle section is meant to retrieve the most significant bit.  This
  * will be 1 with a negative, the flip of x, or a carry from the second most
  * left bit.  Once we have that bit, it is flipped again and anded with 1.
  * The only number to give a 0 to the left side of the & is 0.  This is because
  * when 0 is flipped it gives all 1's.  Add one to it and it becomes all 0's
  * with a 1 being carried out.  The 0 will then be flipped again and shifted.
  * We now have a 1 in the least significant bit & with 1 will return a 1 only
  * when x is 0.
  */
  return (~((x | (~x + 1)) >> 31) & 1);
}
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Points: 1
 */
int tmin(void) {
  /*
  * The smallest negative number will have a 1 in the most significant bit
  * and 0's in the rest of the number.
  */
  return (1 << 31);
}
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Points: 2
 */
int fitsBits(int x, int n) {
  /*
  * Essentially dividing number by the number of bits to test.
  * If the number is 0 after the bit shift then the x can be held in n bits
  * if the number is 1 after the bit shift then it is larger than the n bits can
  * hold.
  */
  return !(~(x >> n));
}
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Points: 2
 */
int divpwr2(int x, int n) {
    /*
    * The mask is to determine which way to round.  When the number is positive,
    * the number must round down.  When the number is negative, the number must
    * rount upwards.
    * --Determine if the number is negative because then it will need to round
    * down.  Then determine if any of the bits are set from 0 to n-1.  These
    * bits would be a remainder when divided.  If any of them are set, the
    * negative must be rounded up which is equivalent to setting the 0 bit to 1.
    */
    int negative = (x & (1 << 31)) >> 31;
    int mask = !(!(x & ~(-1 << n)));
    mask = mask & negative;
    x = x >> n;
    return (x + mask);
}
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Points: 2
 */
int negate(int x) {
  /*
  * Flip the bits and add one to perform the two's complement
  */
  return (~x + 1);
}
/*
 * isPositive - return 1 if x > 0, return 0 otherwise
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Points: 3
 */
int isPositive(int x) {
  /*
  * Left side of the or: If the number is negative will return 1
  * right side of the or: is the number is 0 will return 1
  * Since this should return 0, we use the not.
  */
  return !((x & (0x1 << 31)) | !x);
}
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Points: 3
 */
int isLessOrEqual(int x, int y) {
  /*
  * Subtract x from y by taking the two's complement and adding it to y.
  * Shift the result to get the sign bit.
  * if the result was negative (x > y) - sign bit is 1, return 0
  * if the result was positive (x <= y) - sign bit is 0, return 1;
  */
  y = y + (~x + 1);
  y = y >> 31;
  return !y;
}
/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Points: 4
 */
unsigned float_neg(unsigned uf) {
  /*
  * Set the mask to the sign bit.  Use the XOR to flip the sign bit
  */
  unsigned mask = 1 << 31;
  return (uf ^ mask);
}
/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Points: 4
 */
unsigned float_i2f(int x) {
  /*
  * Sign: 1 bit
  * Exp : 8 bits
  * Frac: 24 << 24 - count
  * Summary: If the number is 0 or -0, return the number.
  * The number will then be changed to positive to avoid shifting in unwanted
  * 1's.  Shift the positive number to the right until a leading 1 is left.
  * Find the frac field by shifting the number to the right.  If the integer is
  * larger than 2^8, than the frac field cannot hold that many bits so it must
  * be rounded.  Round the number when the first bit to be rounded is 1 and
  * any of the other lower bits are 1 or the 0 bit position is 1 (number is odd).
  */
  if(!x)
    return 0;
  //Find the sign bit
  unsigned oneMask = 1 << 31;
  unsigned sign = x & oneMask;

  //If the number is negative, switch to positive
  int positive = x;
  if(sign){
    positive = (~x + 1);
  }

  //Find the exponent
  int temp = positive;
  int count = 0;
  while((temp >> count)){
    count = count + 1;
  }
  //Number of exponents to get a leading 1
  count = count - 1;

  //Shift the positive over until a leading one in the sign bit
  while(!(positive & oneMask)){
    positive = positive << 1;
  }

  //Remove the sign bit as it is assumed for normalized values
  //Find the frac field
  //Remove the sign bit as it is assumed for normalized values
  unsigned frac = (positive & (~oneMask)) >> 8;

  //If the 8th bit is set and any of the other bits are set, round up
  //If the frac field is odd, and the number == 0.5 then round to nearest even
  if((positive & 0x80) && ((positive & 0x7F) || frac & 0x1))
    frac = frac + 1;


  //Exp field
  unsigned expField = count + 127;

  return (sign | (expField << 23) | frac);

}
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Points: 4
 */
unsigned float_twice(unsigned uf) {
  /*
  * If the number is 0 or -0, return the number.  If the number is INF, -INF, or
  * NaN, return the number.  If the number is normalized, 1 can be added to the
  * exp field.  If the number passes that, then it is denormalized.  If the fracField
  * has a 1 in the highest bit, then this will be lost when it is shifted to the left.
  * This shift will cause a 1 to be in the exp field, which is what we want.
  */
  unsigned notFirstBit = 1 << 31;
  unsigned expFieldMask = ((~0) & ~notFirstBit);
  unsigned fracFieldMask = expFieldMask >> 8;

  //Is the number 0 or -0
  if(!(uf & ~notFirstBit))
    return uf;
  //Mask off the exp field
  unsigned expField = uf & expFieldMask;
  unsigned fracField = uf & fracFieldMask;
  expField = expField >> 23;

  //The number is infinity or negative infinity or NaN
  if(!(~(expField | ((1 << 31) >> 23))))
    return uf;


  //Exp field is not 0, so the number is normalized
  if(expField){
    //Add one to the exponent field
    return ((uf & notFirstBit) | ((expField + 1) << 23) | fracField);
  }
  //The number is denormalized
  //Move the frac field over 1
  return ((uf & notFirstBit) | (fracField << 1));
  }
