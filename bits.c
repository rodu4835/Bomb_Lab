/* 
 * CS:APP Data Lab 
 * 
 * <Ronald Durham rodu4835>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
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
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2018 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */
/* We do not support C11 <threads.h>.  */
/* 
 * bitAnd - x&y using only ~ and | 
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
         
int bitAnd(int x, int y) {
    /* The only time bitAnd returns 1 is when both x,y = 1
     * Application of DeMorgan's Law gets rid of the AND opperator.
     */
  return ~(~x | ~y);
}

/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */

int getByte(int x, int n) {
    /* 
     * Shifting n left by 3 gives the binary number for the amount of bits that x needs to be shifted right to align the last two digits that we are looking for at the end.
     * after shifting, using the AND operator with 0xff will give us the final digits we are looking for. 
     */
   /* int shift_1 = n << 3;
    int shift_2 = x >> shift_1;
    return shift_2 & 0xff; 
    */
    return (x >> (n << 3)) & 0xff;
}

/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3 
 */
int logicalShift(int x, int n) {
    /* use 1 shifted left 31 times to generate an equal length starting with 1
     * then shift that right by n to give n amount of 1s and the rest zeros
     * shift that left 1 to remove the original 1
     * shift x right by n
     * compare the new x to the negated shift 3 to logical shift x with leading 0s
     */
    
    /* int shift_1 = 1 << 31;
    int shift_2 = shift_1 >> n;
    int shift_3 = shift_2 << 1;
    int shift_4 = x >> n;
  return shift_4 & ~shift_3;
  */
    return (x >> n) & ~(((1 << 31) >> n) << 1);
}

/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    /* int mask = 0x1;
    int sum1 = ((x) & mask) + ((x>>1) & mask) + ((x>>2) & mask) + ((x>>3) & mask);
    int sum2 = sum1 + ((x>>4) & mask) + ((x>>5) & mask) + ((x>>6) & mask) + ((x>>7) & mask);
    int sum3 = sum2 + ((x>>8) & mask) + ((x>>9) & mask) + ((x>>10) & mask) + ((x>>11) & mask);
    int sum4 = sum3 + ((x>>12) & mask) + ((x>>13) & mask) + ((x>>14) & mask) + ((x>>15) & mask);
    int sum5 = sum4 + ((x>>16) & mask) + ((x>>17) & mask) + ((x>>18) & mask) + ((x>>19) & mask);
    int sum6 = sum5 + ((x>>20) & mask) + ((x>>21) & mask) + ((x>>22) & mask) + ((x>>23) & mask);
    int sum7 = sum6 + ((x>>24) & mask) + ((x>>25) & mask) + ((x>>26) & mask) + ((x>>27) & mask);
    int sum8 = sum7 + ((x>>28) & mask) + ((x>>29) & mask) + ((x>>30) & mask) + ((x>>31) & mask);
    return sum8;
    */
    int m1, m2, m3, sum;
    
    m1 = 0x11 | (0x11 << 8);                      /* (0001 0001) | (00001 0001 0000 0000) = 0001 0001 0001 0001                                                                                                                                   */
    m2 = m1 | (m1 << 16);                         /* (0001 0001 0001 0001) | (0001 0001 0001 0001 0000 0000 0000 0000) = 0001 0001 0001 0001 0001 0001 0001 0001                                                                                  */
    sum = x & m2;                                 /* (xxxx xxxx xxxx xxxx xxxx xxxx xxxx xxxx) & (0001 0001 0001 0001 0001 0001 0001 0001) = 000x 000x 000x 000x 000x 000x 000x 000x                                                              */
    sum = sum + ((x >> 1) & m2);                  /* (000x 000x 000x 000x 000x 000x 000x 000x) + ((0xxx 0xxx 0xxx 0xxx 0xxx 0xxx 0xxx 0xxx) & 0001 0001 0001 0001 0001 0001 0001 0001) =  000x2 000x2 000x2 000x2 000x2 000x2 000x2 000x2         */
    sum = sum + ((x >> 2) & m2);                  /* (000x2 000x2 000x2 000x2 000x2 000x2 000x2 000x2) + ((00xx 00xx 00xx 00xx 00xx 00xx 00xx 00xx) & 0001 0001 0001 0001 0001 0001 0001 0001) =  000x3 000x3 000x3 000x3 000x3 000x3 000x3 000x3 */
    sum = sum + ((x >> 3) & m2);                  /* (000x3 000x3 000x3 000x3 000x3 000x3 000x3 000x3) + ((000x 000x 000x 000x 000x 000x 000x 000x) & 0001 0001 0001 0001 0001 0001 0001 0001) =  000x4 000x4 000x4 000x4 000x4 000x4 000x4 000x4 */
    sum = sum + (sum >> 16);                      /* 000x4 000x4 000x4 000x4 000x4 000x4 000x4 000x4 + (0000 0000 0000 0000 000x4 000x4 000x4 000x4) = 000x8 000x8 000x8 000x8                                                                    */
    m3 = 0xf | (0xf << 8);                        /* (0000 0000 0000 1111) | (0000 1111 0000 0000) = 0000 1111 0000 1111                                                                                                                          */
    sum = (sum & m3) + ((sum >> 4) & m3);         /* ((000x8 000x8 000x8 000x8) & (0000 1111 0000 1111)) + ((0000 000x8 000x8 000x8) & 0000 1111 0000 1111) =  (0000 000x8 0000 000x8) + (0000 000x8 0000 000x8) = 0000 000x16 0000 000x16        */
    sum = sum + (sum >> 8);                       /* (0000 000x16 0000 000x16) + (0000 0000 0000 000x16) = 0000 000x16 0000 000x32                                                                                                                */
    return sum & 0x3F;                            /* (0000 000x16 0000 000x32) & (0000 0000 0011 1111)  = x32                                                                                                                                    */
                                                  /* in this case each time x goes from x2 to x3 to x4...x32 that is the amount of 1s that have been added up from before, not representing a hexidecimal*/
}

/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int bang(int x) {
    /* First we create a variable that represents the opposite of x and add 1
     * Then using the OR operator, combine x and its opposite to give us our byte
     * by shifting right 31, the binary is filled with the left most digit
     * We then negate the shift and compare with the AND operator to see if there is a 1 or a 0 in the 32nd digit. 
     */
   /* int opposite_of = (~x)+1;
    int combine = (x|opposite_of);
    int shift_1 = combine >> 31;
    int negate_shift = ~shift_1;
    int check_final_int = negate_shift & 1;
  return check_final_int;
  */
    return ~((x | (~x + 1)) >> 31) & 1;
}

/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
   /* 1 shifted left 31 times is the minimum twos complement integer
    * i.e. 1000 0000 0000 0000 0000 0000 0000 0000
    */
  return 1 << 31;
}

/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */

int fitsBits(int x, int n) {
        /*
         x = 5 or 0101
         mask = x >> 31 = 0000 0000 0000 0000 0000 0000 0000 0000
         (~x & mask) = (~0101 & mask) = (1010 & mask) = 0000 0000 0000 0000 0000 0000 0000 0000
         (x & ~mask) = (x & ~0000 0000 0000 0000 0000 0000 0000 0000) = (0101 & 1111 1111 1111 1111 1111 1111 1111 1111) = 0000 0000 0000 0000 0000 0000 0000 0101
         (~x & mask) + (x & ~mask) = 0000 0000 0000 0000 0000 0000 0000 0101 = 5
         n = 3 or 0011
         (n + ~0) = (0011 + 1111) = 0010 or 2
         5 >> 2 = 0101 >> 2 = 0001
         !(0001) = 0
         */ 
    
    int mask = x >> 31;
    int case1 = (~x & mask);
    int case2 = (x & ~mask);
    int full_num = (case1 + case2);
    int shift_amount = (n + ~0);
    int result = full_num >> shift_amount;
  return !result;
}

/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    
    /*
    sign = x >> 31 = 0000 0000 0000 0000 0000 0000 0000 1111 >> 31 = 0000 0000 0000 0000 0000 0000 0000 0000
    mask = (1 << n) + (~0) = (0001 << 1) + ~(0000) = 0000 0000 0000 0000 0000 0000 0000 0010 + 1111 1111 1111 1111 1111 1111 1111 1111 = 0001 0000 0000 0000 0000 0000 0000 0000 0001
    sign & mask = (0000 0000 0000 0000 0000 0000 0000 0000) & (0001 0000 0000 0000 0000 0000 0000 0000 0001) = 0000 0000 0000 0000 0000 0000 0000 0000
    x + (sign & mask) = 0000 0000 0000 0000 0000 0000 0000 1111 + 0000 0000 0000 0000 0000 0000 0000 0000 = 0000 0000 0000 0000 0000 0000 0000 1111
    x >> n = 0000 0000 0000 0000 0000 0000 0000 1111 >> 1 = 0000 0000 0000 0000 0000 0000 0000 0111 = 7
    
   */
    
    int sign = x >> 31;
    int mask = (1 << n) + (~0);
	return  (x + (sign & mask)) >> n;
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    /* flip byte and add 1
    */
  return (~x)+1;
}

/* 
 * isPositive - return 1 if x > 0, return 0 otherwise 
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    /* bangs x to result in 0 if number and 1 if 0;
     * shifts x to the right to single out first byte determining if x was positive (leading 0) or negative (leading 1)
     * using OR compares either 0-0, 1-0, or 0-1. (positive, negative, or zero respectively)
     * bang result to return 1 for positive x, and 0 for negative or zero x. 
    */
  return !(x >> 31 | !x);
}

/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
   
    /*
    int signofX = (x >> 31) & 1;
    int signofY = (y >> 31) & 1;
    int signsame = !(signofX ^ signofY);
    int XlessY = ((x + ~y)>>31) & 1;
    int pass = signsame & XlessY;
    int passORoneneg = pass | ((!signofY) & signofX);
  return passORoneneg;
  */
    return ((!(((x >> 31) & 1) ^ ((y >> 31) & 1))) & (((x + ~y)>>31) & 1)) | ((!((y >> 31) & 1)) & ((x >> 31) & 1));
}
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
  return 2;
}
/*
 * Extra credit
 */
/* 
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
 return 2;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
  return 2;
}
/* 
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
  return 2;
}
