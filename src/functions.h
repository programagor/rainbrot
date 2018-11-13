#include <math.h>
#include <complex.h>

#include <mpfr.h>

long double complex no_mapping(long double complex Z);


void mandelbrot(mpfr_t *Z, const mpfr_t *c, mpfr_t *temp);

long double complex ship      (long double complex Z, const long double complex c);

long double complex julia(long double complex Z, const long double complex c);

int8_t mandelbrot_optimiser(const long double complex c);

int8_t no_optimiser(const long double complex Z);
