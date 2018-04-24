#include <stdint.h>
#include <math.h>
#include <complex.h>

long double complex mandelbrot(long double complex c, long double complex Z)
{
  return(Z*Z+c);
}

int8_t no_optimiser()
{
  return(-1);
}
