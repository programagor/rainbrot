#include <math.h>
#include <complex.h>

long double complex no_mapping(long double complex Z);


long double complex mandelbrot(long double complex Z, const long double complex c);
long double complex ship      (long double complex Z, const long double complex c);

long double complex julia(long double complex Z, const long double complex c);

int8_t mandelbrot_optimiser(const long double complex c);

int8_t no_optimiser(const long double complex Z);
