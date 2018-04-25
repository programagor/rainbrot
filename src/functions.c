#include <stdint.h>
#include <math.h>
#include <complex.h>

long double complex no_mapping(long double complex Z)
{
	return(Z);
}




long double complex mandelbrot(long double complex c, long double complex Z)
{
  return(Z*Z+c);
}

int8_t mandelbrot_optimiser(long double complex c)
{
  /* Outside of the 2-disk */
  if(cabs(c)>2) return(0);
  
  /* Inside te cardioid */
  //if() return(1);
  
  return(-1);
}




/* Used as sink, always returns UNCERTAIN = no optimisation */
int8_t no_optimiser()
{
  return(-1);
}
