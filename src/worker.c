#define _GNU_SOURCE
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <complex.h>
#include <time.h>
#include <sys/mman.h>

#include <mpfr.h>

#define PI_2_I 3.141592653589793238462643383279502884L*2.0*I

#include "worker.h"

/*
void worker_cleanup(void *arg_v)
{

}
*/

void* worker(void *arg_v)
{
  struct argw *arg=(struct argw*)arg_v;
  
  const uint32_t re_size = arg->re_size;
  const uint32_t im_size = arg->im_size;
  const double re_min = arg->re_min;
  const double re_max = arg->re_max;
  const double im_min = arg->im_min;
  const double im_max = arg->im_max;
  const double bail = arg->bail;
  const uint64_t runs = arg->runs;
  const long double a_std = arg->a_std;
  const long double b_std = arg->b_std;
  const long double a_mu = arg->a_mu;
  const long double b_mu = arg->b_mu;
  long double complex (*const function)(long double complex Z, const long double complex c) = arg->function;
  int8_t (*const optimiser)(const long double complex c) = arg->optimiser; /* 0 is no, 1 is yes, -1 is maybe */

  uint32_t l;
  for(l=1;arg->iter[l+1];l++); /* how many channels */
  
  //pthread_cleanup_push(worker_cleanup,arg_v);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  /* Run until number of runs is reached */
  while(1)
    {
      /* Generate a point that is outside of set */
      long double complex c,Z;
      uint64_t target_iter;
      uint64_t run;
      do
        {
          
          /* Use Box-Muller algorithm to get two normally distributed numbers */
          /* and then make one complex number from them */
          
          int U1,U2;
          /* rand is not thread safe, needs locking */
          pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
          pthread_mutex_lock(arg->lock_rand);
          mpfr_grandom();
          run=++*(arg->counter);
          pthread_mutex_unlock(arg->lock_rand);
          pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
          if(run>=runs)
            {
              /* We're done, quitting thread */
              pthread_exit(NULL);
            }
          if(fmod(run,runs/100.0)<1)
            {
              char timestr[20];
              time_t now = time (0);
              strftime (timestr, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));
              printf("[%s] Run: %10lu/%10lu (%3.0f%%)\n",timestr,run,runs,run*100.0/runs);
            }
          /* Muller-Box Algorithm */
          c=csqrtl(-2.0*clogl(U1*((double)1.0/RAND_MAX)))*cexpl(PI_2_I*(U2*((double)1.0/RAND_MAX)));
          /* c now has Gaussian distribution (on the Gaussian plane) */
              
          /* Scale axis-wise (non-comforming mapping) */
          c=(creall(c)*a_std+a_mu)+((long double complex)I)*(cimagl(c)*b_std+b_mu);
          
          target_iter=preiterator(c,function,optimiser,arg->iter[0],arg->iter[l],bail);
          
          pthread_testcancel();
          
        }
      while(target_iter==0);
      
      int64_t idx_x,idx_y; /* pixel coords */
      
      /* Now we have a point which is outside, can iterate and draw */

      /* Which buffer does this go to? */
      uint32_t k;
      for(k=0;target_iter>arg->iter[k+1] && k<l-1;k++);
      
      
      Z=c;
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      for(uint64_t i=0;i<target_iter;i++)
        {
          Z=function(Z,c); /* Iterate */
          
          
          /* increment buffer */
          idx_x=round((creall(Z) - re_min)*( re_size -1)/(re_max-re_min));
          idx_y=im_size - round((cimagl(Z) - im_min)*( im_size -1)/(im_max-im_min));
                  
          if(idx_x>=0 && idx_x < re_size && idx_y>=0 && idx_y < im_size)
            {
              pthread_mutex_lock(&arg->locks[k][idx_x]);
              arg->maps[k][im_size*idx_x+idx_y]++;
              pthread_mutex_unlock(&arg->locks[k][idx_x]);
            }
        }
      arg->hits[k]++;
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      
    }

    //pthread_cleanup_pop(0);
}

uint64_t preiterator
(
  const long double complex c,
  long double complex (*const function)(long double complex Z, const long double complex c),
  int8_t (*const optimiser)(const long double complex c),
  const uint64_t iter_min,
  const uint64_t iter_max,
  const double bail
)
{
  const int8_t res=optimiser(c);
  if(res==1)
    {
      return(0); /* We know c is inside, can return */
    }
  /* res== 0: We know c is outside, but does it last long enough? */
  /* res==-1: We don't know whether c is inside or outside, need full iteration cycle */
  const uint64_t iter=(res==0?iter_min:iter_max);
  uint64_t i;
  long double complex Z=c;
  for(i=0;i<iter;i++)
    {
      Z=function(Z,c);
      if(cabsl(Z)>bail)
        {
          return(i<iter_min?0:i);
        }
    }
  return(0);
}
