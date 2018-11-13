#ifndef WORKER_H
#define WORKER_H

#include <stdint.h>
#include <complex.h>
#include <pthread.h>

void* worker(void *arg);

uint64_t preiterator(
		     long double complex c,
		     long double complex (*function)(long double complex c, long double complex Z),
		     int8_t (*optimiser)(long double complex c),
		     uint64_t iter_min,
		     uint64_t iter_max,
		     double bail);

struct argw
{
  pthread_mutex_t *lock_rand;
  pthread_mutex_t **locks;
  uint64_t **maps;
  uint64_t *counter;
  uint64_t *hits;
  uint32_t re_size;
  uint32_t im_size;
  double re_min;
  double re_max;
  double im_min;
  double im_max;
  uint64_t *iter;
  double bail;
  uint64_t runs;
  void (*function)(mpfr_t *Z, const mpfr_t *c, mpfr_t *param, mpfr_t *temp);
  int8_t (*optimiser)(const mpfr_t *c, mpfr_t *param, mpfr_t *temp);
  long double a_std;
  long double b_std;
  long double a_mu;
  long double b_mu;
  uint32_t dimensions;
  uint32_t temps;
  mpfr_prec_t precision;
};

#endif
