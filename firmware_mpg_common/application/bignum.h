#ifndef BIGNUM_H
#define BIGNUM_H

#include "configuration.h"

typedef struct bigUInt{
    // digits is an array of the digits of the number. The ones digit is at
    // array index 0 (i.e. bigUInt = sum(digits[i]*10^i,i=0:length-1)
    u8* digits;
    u32 length;
} bigUInt;

bigUInt bigUInt_from_u32(u32 toConvert);
void    bigUInt_copy(bigUInt* dest, const bigUInt src);
bool    bigUInt_equals(bigUInt num1, bigUInt num2);
bool    bigUInt_lessthan(bigUInt num1, bigUInt num2);
bool    bigUInt_greaterthan(bigUInt num1, bigUInt num2);
bigUInt bigUInt_add(u32 numArgs, bigUInt* nums);
void    bigUInt_add_2_p(bigUInt* base, const bigUInt addend);
bigUInt bigUInt_multiply(u32 numArgs, bigUInt* nums);
bigUInt bigUInt_subtract(const bigUInt minuend, const bigUInt subtrahend);

#endif