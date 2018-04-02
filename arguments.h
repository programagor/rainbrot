#ifndef ARGUMENTS_H
#define ARGUMENTS_H

#include <argp.h>


struct arguments
{
  char *args[1];
  int verbose;
  unsigned int re_size;
  unsigned int im_size;
  double re_min;
  double re_max;
  double im_min;
  double im_max;
  
  
};

extern struct argp argp;

enum keys
  {
    k_size=256,
    k_window,
    k_iterMin,
    k_iterMax
  };

#endif
