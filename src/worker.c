#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#define PI_2_I 3.141592653589793238462643383279502884L*2.0*I

#include "worker.h"

void* worker(void *arg_v)
{
  struct argw *arg=(struct argw*)arg_v;
  uint32_t l;
  for(l=1;arg->iter[l+1];l++); /* how many channels */

  /* Create thread-local buffer */
  uint64_t **buff=malloc(arg->re_size*sizeof(uint64_t*));
  for(uint32_t x=0;x<arg->re_size;x++)
    {
      buff[x]=calloc(arg->im_size,sizeof(uint64_t));
    }
  
  /* Run until number of runs is reached */
  while(1)
    {
      pthread_mutex_lock(&arg->locks[l]);
      if(*(arg->counter) >= arg->runs)
	{
	  pthread_mutex_unlock(&arg->locks[l]);
	  /* We're done, quitting thread */
	  for(uint32_t x=0;x<arg->re_size;x++)
	    {
	      free(buff[x]);
	    }
	  free(buff);
	  pthread_exit(NULL);
	}
      *(arg->counter)+=1;
      pthread_mutex_unlock(&arg->locks[l]);
      /* Runs still needs to be done, continuing */

      /* Generate a point that is outside of set */
      long double complex c;
      do
	{
	  /* Use Box-Muller algorithm to get two normally distributed numbers */
	  /* and then make one complex number from them */
	  
	  /* rand is not thread safe, needs locking */
	  pthread_mutex_lock(&arg->locks[l+1]);
	  int U1=rand();
	  int U2=rand();
	  pthread_mutex_unlock(&arg->locks[l+1]);

	  c=csqrtl(-2.0*clogl(U1*((double)1.0/RAND_MAX)))*cexpl(PI_2_I*(U2*((double)1.0/RAND_MAX)));
	  
	}
      while(0);
      
    }
}
