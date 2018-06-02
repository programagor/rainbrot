#include <stdint.h>
#include <math.h>
#include <complex.h>

long double complex no_mapping(long double complex Z)
{
	return(Z);
}




long double complex mandelbrot(long double complex Z, long double complex c)
{
  return(Z*Z+c);
}

long double complex ship(long double complex Z, long double complex c)
{
  return(cpowl(cabsl(creall(Z))+cabsl(cimagl(Z))*I,2)+c);
}

int8_t mandelbrot_optimiser(long double complex Z)
{
  /* Outside of the 2-disk */
  if(cabs(Z)>2) return(0);
  
  /* Inside te cardioid */
  long double x=creall(Z);
  long double y=cimagl(Z);
  long double p=sqrtl(powl(x-0.25L,2)+powl(y,2));
  if(x<p-2*powl(p,2)+0.25L) return(1);
  /* Inside the 2-period bulb */
  if(powl(x+1,2)+powl(y,2)<0.0625L) return(1);
  
  return(-1);
}

long double complex julia(long double complex Z)
{
  return(Z*Z+(0.687 + 0.312*I));
}


/* Used as sink, always returns UNCERTAIN = no optimisation */
int8_t no_optimiser()
{
  return(-1);
}
