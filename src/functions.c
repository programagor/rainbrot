#include <stdint.h>

#include <mpfr.h>

void no_mapping(mpfr_t *Z __attribute__((unused)), mpfr_t *c __attribute__((unused)), mpfr_t *param __attribute__((unused)), mpfr_t *temp __attribute__((unused)))
{

}




void mandelbrot(mpfr_t *Z, mpfr_t *c, mpfr_t *param __attribute__((unused)), mpfr_t *temp)
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

void ship(mpfr_t *Z, mpfr_t *c, mpfr_t *param, mpfr_t *temp)
{
  
  mpfr_abs(Z[0],Z[0],MPFR_RNDN);
  mpfr_abs(Z[1],Z[1],MPFR_RNDN);
  
  mandelbrot(Z, c, param, temp);
}

int8_t mandelbrot_optimiser(mpfr_t *c, mpfr_t *param __attribute__((unused)), mpfr_t *temp)
{
  
  /* Outside of the 2-disk */
  mpfr_mul(temp[0],c[0],c[0],MPFR_RNDN);
  mpfr_fma(temp[0],c[1],c[1],temp[0],MPFR_RNDN);
  if(mpfr_cmp_ui(temp[0], 2)>0) return(0);
  
  /* Inside te cardioid */
  //long double x=creall(Z);
  //long double y=cimagl(Z);
  //long double p=sqrtl(powl(x-0.25L,2)+powl(y,2));
  //if(x<p-2*powl(p,2)+0.25L) return(1);
  /* Inside the 2-period bulb */
  //if(powl(x+1,2)+powl(y,2)<0.0625L) return(1);
  
  return(-1);
}

void julia(mpfr_t *Z __attribute__((unused)), mpfr_t *c __attribute__((unused)), mpfr_t *param __attribute__((unused)), mpfr_t *temp __attribute__((unused)))
{
  //return(Z*Z+(0.687 + 0.312*I));
}


/* Used as sink, always returns UNCERTAIN = no optimisation */
int8_t no_optimiser(mpfr_t *c __attribute__((unused)), mpfr_t *param __attribute__((unused)), mpfr_t *temp __attribute__((unused)))
{
  return(-1);
}
