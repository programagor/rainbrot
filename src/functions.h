#include <mpfr.h>

long double complex no_mapping(mpfr_t *Z, const mpfr_t *c, mpfr_t *param, mpfr_t *temp);


void mandelbrot(mpfr_t *Z, const mpfr_t *c, mpfr_t *param, mpfr_t *temp);

void ship(mpfr_t *Z, const mpfr_t *c, mpfr_t *param, mpfr_t *temp);

void julia(mpfr_t *Z, const mpfr_t *c, mpfr_t *param, mpfr_t *temp);

int8_t mandelbrot_optimiser(const mpfr_t *c, mpfr_t *param, mpfr_t *temp);

int8_t no_optimiser(const mpfr_t *c, mpfr_t *param, mpfr_t *temp);
