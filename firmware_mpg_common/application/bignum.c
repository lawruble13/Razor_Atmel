#include "bignum.h"


/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


bigUInt bigUInt_from_u32(u32 toConvert){
  bigUInt result;
  result.digits = malloc(1);
  result.digits[0] = 0;
  result.length = 0;
  while(toConvert){
    result.digits = realloc(result.digits, result.length+1);
    result.digits[result.length] = toConvert%10;
    toConvert /= 10;
    result.length++;
  }
  if(!result.length){
    result.length=1;
  }
  return result;
}

void bigUInt_copy(bigUInt* dest, const bigUInt src){
  if(dest->digits == NULL){
    dest->digits = malloc(src.length);
  } else {
    dest->digits = realloc(dest->digits, src.length);
  }
  for (u32 i = 0; i < src.length; i++){
    dest->digits[i] = src.digits[i];
  }
  dest->length = src.length;
}

bool bigUInt_equals(bigUInt num1, bigUInt num2){
  if(num1.length != num2.length){
    return FALSE;
  }
  for(u32 i = 0; i < num1.length; i++){
    if(num1.digits[i] != num2.digits[i]){
      return FALSE;
    }
  }
  return TRUE;
}

bool bigUInt_lessthan(bigUInt num1, bigUInt num2){
  if(num1.length != num2.length){
    if(num1.length < num2.length){
      return TRUE;
    } else {
      return FALSE;
    }
  }
  for(u32 i = num1.length; i > 0; i--){
    if(num1.digits[i-1] > num2.digits[i-1]){
      return FALSE;
    }
  }
  if(num1.digits[0] < num2.digits[0]){
    return TRUE;
  } else {
    return FALSE;
  }
}

bool bigUInt_greaterthan(bigUInt num1, bigUInt num2){
  return bigUInt_lessthan(num2, num1);
}

bigUInt bigUInt_add(u32 numArgs, bigUInt* nums){
  bigUInt result = {NULL,0};
  if(numArgs == 0){
    return result;
  }
  bigUInt_copy(&result, nums[0]);
  for(u32 i = 1; i < numArgs; i++){
    bigUInt_add_2_p(&result, nums[i]);
  }
  return result;
}

void bigUInt_add_2_p(bigUInt* base, const bigUInt addend){
  bigUInt result = {NULL,0};
  u32 mlen = (base->length > addend.length) ? (base->length) : (addend.length);
  result.digits=calloc(mlen+1, sizeof(u8));
  result.length=mlen+1;
  for(u32 i = 0; i < mlen; i++){
    if(i < base->length){
      result.digits[i] += base->digits[i];
    }
    if(i < addend.length){
      result.digits[i] += addend.digits[i];
    }
    if(result.digits[i] > 9){
      result.digits[i+1] += result.digits[i] / 10;
      result.digits[i] %= 10;
    }
  }
  for(u32 i = result.length; i > 0; i--){
    if(result.digits[i-1] == 0){
      result.length--;
      result.digits=realloc(result.digits,result.length);
    } else {
      break;
    }
  }
  bigUInt_copy(base,result);
  free(result.digits);
}

bigUInt bigUInt_multiply(u32 numArgs, bigUInt* nums){
  bigUInt result = {NULL,0};
  if(numArgs == 0){
    return result;
  }
  bigUInt counter = bigUInt_from_u32(0);
  bigUInt one = bigUInt_from_u32(1);
  bigUInt curterm;
  bigUInt_copy(&result, nums[0]);
  for(u32 i = 1; i < numArgs; i++){
    bigUInt_copy(&curterm, result);
    for( ; bigUInt_lessthan(counter, nums[i]); bigUInt_add_2_p(&counter, one)){
      bigUInt_add_2_p(&result, curterm);
    }
  }
  free(curterm.digits);
  free(one.digits);
  free(counter.digits);
  return result;
}

bigUInt bigUInt_multiply_2(bigUInt base, bigUInt factor){
  bigUInt* partials = calloc(factor.length,sizeof(bigUInt));
  for(int i = 0; i < factor.length; i++){
    bigUInt part;
    part.length = base.length+i+2;
    part.digits = calloc(part.length, sizeof(bigUInt));
    for(int j = 0; j < base.length; j++){
      part.digits[j+i] += (factor.digits[i] * base.digits[j]);
      part.digits[j+i+1] += part.digits[j+i]/10;
      part.digits[j+i] %= 10;
    }
    bigUInt_copy(&partials[i], part);
    free(part.digits);
  }
  for(int i = 1; i < factor.length; i++){
    bigUInt_add_2_p(&partials[0], partials[i]);
    free(partials[i].digits);
  }
  bigUInt result = {NULL, 0};
  bigUInt_copy(&result, partials[0]);
  free(partials);
  return result;
}

bigUInt bigUInt_subtract(const bigUInt minuend, const bigUInt subtrahend){
  if(!bigUInt_greaterthan(minuend, subtrahend)){
    return bigUInt_from_u32(0);
  }
  bigUInt base;
  bigUInt_copy(&base, minuend);
  for(u32 i = 0; i < subtrahend.length; i++){
    if(base.digits[i] >= subtrahend.digits[i]){
      base.digits[i] -= subtrahend.digits[i];
    } else {
      u32 j=i+1;
      base.digits[i] += 10;
      while(base.digits[j] == 0){
        base.digits[j] += 9;
        j++;
      }
      base.digits[j]--;
      base.digits[i] -= subtrahend.digits[i];
    }
  }
  for(u32 i = base.length; i > 0; i--){
    if(base.digits[i-1] == 0){
      base.length--;
      base.digits = realloc(base.digits,base.length);
    } else {
      break;
    }
  }
  return base;
}