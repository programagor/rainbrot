#include <pthread.h>
#include <stdio.h>

#include "worker.h"

void* worker(void *arg_v)
{
  struct argw *arg=(struct argw*)arg_v;
  uint32_t l;
  for(l=1;arg->iter[l+1];l++); /* how many channels */
  /* Run until number of runs is reached */
  while(1)
    {
      pthread_mutex_lock(&arg->locks[l]);
      if(*(arg->counter) >= arg->runs)
	{
	  pthread_mutex_unlock(&arg->locks[l]);
	  pthread_exit(NULL);
	}
      *(arg->counter)+=1;
      pthread_mutex_unlock(&arg->locks[l]);
      /* Work still needs to be done, continuing */
      
    }
}
