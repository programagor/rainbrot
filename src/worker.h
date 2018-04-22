#ifndef WORKER_H
#define WORKER_H

#include <stdint.h>
#include <pthread.h>

void* worker(void *arg);

struct argw
{
  pthread_mutex_t *locks;
  uint64_t **maps;
  uint64_t *counter;
  uint32_t re_size;
  uint32_t im_size;
  double re_min;
  double re_max;
  double im_min;
  double im_max;
  uint64_t *iter;
  double bail;
  uint64_t runs;
};

#endif
