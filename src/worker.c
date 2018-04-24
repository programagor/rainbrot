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
      pthread_mutex_lock(arg->lock_iter);
      if(*(arg->counter) >= arg->runs)
	{
	  pthread_mutex_unlock(arg->lock_iter);
	  /* We're done, quitting thread */
	  for(uint32_t x=0;x<arg->re_size;x++)
	    {
	      free(buff[x]);
	    }
	  free(buff);
	  pthread_exit(NULL);
	}
      *(arg->counter)+=1;
      pthread_mutex_unlock(arg->lock_iter);
      /* Runs still needs to be done, continuing */

      /* Generate a point that is outside of set */
      long double complex c;
      int8_t inside; /* 0 is no, 1 is yes, -1 is maybe */
      do
	{
	  /* Use Box-Muller algorithm to get two normally distributed numbers */
	  /* and then make one complex number from them */
	  
	  int U1,U2;
	  /* rand is not thread safe, needs locking */
	  pthread_mutex_lock(arg->lock_rand);
	  do
	    {
	      U1=rand();
	    }
	  while(U1==0);
	  U2=rand();
	  pthread_mutex_unlock(arg->lock_rand);

	  c=csqrtl(-2.0*clogl(U1*((double)1.0/RAND_MAX)))*cexpl(PI_2_I*(U2*((double)1.0/RAND_MAX)));
	  inside=preiterator(c,arg->function,arg->optimiser,arg->iter[0],arg->iter[l],arg->bail);
	}
      while(inside==1);
      
    }
}

uint64_t preiterator(long double complex c, long double complex (*function)(long double complex c, long double complex Z), int8_t (*optimiser)(long double complex c), uint64_t iter_min,uint64_t iter_max, double bail)
{
  int8_t res=optimiser(c);
  if(res!=-1)
    {
      return(res);
    }
  else
    {
      uint64_t i;
      long double complex Z=0;
      for(i=0;i<iter_max;i++)
	{
	  Z=function(c,Z);
	  if(cabs(Z)>bail) return(0);
	}
      return(1);
    }
}
