#include <stdint.h>
#include <math.h>
#include <complex.h>

#include <mpfr.h>

void no_mapping(mpfr_t *Z, const mpfr_t *c, mpfr_t *temp)
{

}




void mandelbrot(mpfr_t *Z, const mpfr_t *c, mpfr_t *temp)
{
  // requires 1 temp variable
  // Z=Z*Z+c
  // C=a+bi
  // Z=c+di
  // (c+di)(c+di)+a+bi=
  // cc+2cdi-dd+a+bi=
  // (a+cc-dd)+(b+2cd)i
  
  // 2. temp=c[0]+Z[0]^2-Z[1]^2
  // 2.1. temp[0]=Z[0]*Z[0]-Z[1]*Z[1]
  //mpfr_fmms(temp[0],Z[0],Z[0],Z[1],Z[1],0);
  // 2.2. temp[0]=temp[0]+c[0]
  //mpfr_add(temp[0],temp[0],c[0],0);
  
  // 2.1. temp[0]=Z[1]*Z[1]-c[0]
  mpfr_fms(temp[0],Z[1],Z[1],Z[0],MPFR_RNDN);
  // 2.2. temp[0]=Z[0]*Z[0]-temp[0]
  mpfr_fms(temp[0],Z[0],Z[0],temp[0],MPFR_RNDN);
  
  // 3. Z[1]=c[1]+2*Z[0]*Z[1]
  // 3.1. Z[1]=Z[1]*2
  mpfr_mul_ui(Z[1],Z[1],2,MPFR_RNDN);
  // 3.2. Z[1]=Z[0]*Z[1]+c[1]
  mpfr_fma(Z[1],Z[0],Z[1],c[1],MPFR_RNDN);
  
  // 4. Z[0]=temp
  mpfr_set(Z[0],temp[0],MPFR_RNDN);
  
}

long double complex ship(long double complex Z, const long double complex c)
{
  // Z=(abs(real(Z))+abs(imag(Z))*i)+C
  // C=a+bi
  // Z=c+di
  // (abs(c)+abs(d)i)^2+a+bi=
  // abs(c)^2+2*abs(c)*abs(d)*i-abs(d)^2+a+bi=
  // (a+abs(c)^2-abs(d)^2)+(b+2*abs(c)*abs(d))i
  
  
  
  return(cpowl(fabsl(creall(Z))+fabsl(cimagl(Z))*I,2)+c);
}

int8_t mandelbrot_optimiser(const long double complex Z)
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
