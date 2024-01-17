#include "PPintrin.h"
#include <iostream>
using namespace std;
// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  __pp_vec_float result,x;
  __pp_vec_int y, zero, one;
  __pp_vec_float threshold = _pp_vset_float(9.999999f);
  __pp_mask maskAll, maskdone, maskthreshold;
  zero = _pp_vset_int(0);
  one = _pp_vset_int(1);
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    // All ones
    //maskAll = _pp_init_ones( (N - i) % (VECTOR_WIDTH + 1) );
    int size = VECTOR_WIDTH;
    if(N-i < VECTOR_WIDTH)  size = N - i;
    maskAll = _pp_init_ones(size);
    maskdone = _pp_init_ones(0);
    //Load value
    _pp_vload_float(x, values + i, maskAll);
    _pp_vset_float(result, 1.f, maskAll);
    _pp_vload_int(y, exponents + i, maskAll);
    _pp_vgt_int(maskdone, y, zero, maskAll);
    while(_pp_cntbits(maskdone)){
      _pp_vmult_float(result, result, x, maskdone);
      _pp_vsub_int(y, y, one, maskdone);
      _pp_vgt_int(maskdone, y, zero, maskAll);
    }

    _pp_vgt_float(maskthreshold, result, threshold, maskAll);
    _pp_vset_float(result, 9.999999f, maskthreshold);
    _pp_vstore_float(output + i, result, maskAll);
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //
  float ret = 0.0;
  __pp_vec_float x, tmp;
  __pp_mask maskAll;
  tmp = _pp_vset_float(0.f);
  int shift = VECTOR_WIDTH;
  // O(N / VECTOR_WIDTH)
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    maskAll = _pp_init_ones(N-i);

    _pp_vload_float(x, values + i, maskAll);

    _pp_vadd_float(tmp, tmp, x, maskAll);    
  }

  // O(log2(VECTOR_WIDTH))
  while(shift != 1) {
    _pp_hadd_float(tmp, tmp);
    
    _pp_interleave_float(tmp, tmp);

    shift >>= 1;
  }
  ret = tmp.value[0];
  //_pp_vstore_float(output + i, result, maskAll);
  return ret;
}