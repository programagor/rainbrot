#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <stdint.h>
#include <argp.h>


struct arguments
{
  char *args[1];
  int verbose;
  uint32_t re_size;
  uint32_t im_size;
  double re_min;
  double re_max;
  double im_min;
  double im_max;
  uint64_t *iter;
  double bail;
  uint64_t runs;
  uint16_t threads;
  unsigned int seed;
  char* function;
  long double a_std;
  long double b_std;
  long double a_mu;
  long double b_mu;
  uint64_t precision;
};

extern struct argp argp;



#endif
