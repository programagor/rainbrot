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
  const uint64_t runs = arg->runs;
  void (*function)(mpfr_t *Z, mpfr_t *c, mpfr_t *param, mpfr_t *temp) = arg->function;
  int8_t (*optimiser)(mpfr_t *c, mpfr_t *param, mpfr_t *temp) = arg->optimiser; /* 0 is no, 1 is yes, -1 is maybe */
  const uint32_t dimensions = arg->dimensions;
  const uint32_t temps = arg->temps;
  const mpfr_prec_t precision = arg->precision;

  uint32_t l;
  for(l=1;arg->iter[l+1];l++); /* how many channels */
  
  /* Initialise numbers */
  mpfr_t *Z=malloc(dimensions*sizeof(mpfr_t));
  mpfr_t *c=malloc(dimensions*sizeof(mpfr_t));
  mpfr_t *temp=malloc(temps*sizeof(mpfr_t));
  
  for(uint32_t i=0;i<dimensions;i++)
    {
      mpfr_init2(Z[i],precision);
      mpfr_init2(c[i],precision);
    }
  for(uint32_t i=0;i<temps;i++)
    {
      mpfr_init2(temp[i],precision);
    }
  // TODO: cleanup
  
  mpfr_set_si(temp[0],-37,MPFR_RNDN);
  mpfr_printf("temp=%e\n",*temp[0]);
  
  //pthread_cleanup_push(worker_cleanup,arg_v);
  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
  /* Run until number of runs is reached */
  while(1)
    {
      /* Generate a point that is outside of set */
      uint64_t target_iter;
      uint64_t run;
      do
        {
          
          /* Use Box-Muller algorithm to get two normally distributed numbers */
          /* and then make one complex number from them */
          
          /* rand is not thread safe, needs locking */
          pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
          pthread_mutex_lock(arg->lock_rand);
          for(uint32_t i=0;i+1<dimensions;i+=2)
            {
              if(i+1==dimensions)
                {
                  mpfr_grandom(c[i],NULL,*arg->prng_state,MPFR_RNDN);
                  printf("c[%d]=(",i);
                  mpfr_printf("%e)\n",c[i]);
                }
              else
                {
                  mpfr_grandom(c[i],c[i+1],*arg->prng_state,MPFR_RNDN);
                  printf("c[%d]=(",i);
                  mpfr_printf("%e)\n",c[i]);
                  printf("c[%d]=(",i+1);
                  mpfr_printf("%e)\n",c[i+1]);
                }
            }
          run=++*(arg->counter);
          pthread_mutex_unlock(arg->lock_rand);
          pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
      mpfr_printf("Trying:  (%e)+(%e)i\n",c[0],c[1]);
          
          if(fmod(run,runs/100.0)<1)
            {
              char timestr[20];
              time_t now = time (0);
              strftime (timestr, 100, "%Y-%m-%d %H:%M:%S", localtime (&now));
              printf("[%s] Run: %10lu/%10lu (%3.0f%%)\n",timestr,run,runs,run*100.0/runs);
            }
            
          
          if(run>=runs)
            {
              /* We're done, quitting thread */
              pthread_exit(NULL);
            }
      mpfr_printf("Trying:  (%e)+(%e)i\n",c[0],c[1]);
          
          /* Scale c per axis (non-comforming mapping) */
          //c=(creall(c)*a_std+a_mu)+((long double complex)I)*(cimagl(c)*b_std+b_mu);
          for(uint32_t i=0;i+1<dimensions;i++)
            {
              mpfr_fma(c[i],c[i],arg->prng_std[i],arg->prng_mu[i],MPFR_RNDN);
            }
      mpfr_printf("Scaling: (%e)+(%e)i\n",c[0],c[1]);
          
          for(uint32_t i=0;i<dimensions;i++)
            {
              mpfr_set(Z[i],c[i],MPFR_RNDN);
            }
          target_iter=preiterator(Z,c,function,optimiser,arg->iter[0],arg->iter[l],*arg->bail,NULL,temp,dimensions);
          
          pthread_testcancel();
          
        }
      while(target_iter==0);
      printf("Went iterating\n");
      
      int64_t idx_x,idx_y; /* pixel coords */
      
      /* Now we have a point which is outside, can iterate and draw */

      /* Which buffer does this go to? */
      uint32_t k;
      for(k=0;target_iter>arg->iter[k+1] && k<l-1;k++);
      
      
      Z=c;
      pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
      for(uint64_t i=0;i<target_iter;i++)
        {
          function(Z, c, NULL, temp); /* Iterate */
          
          
          /* increment buffer */
          long double Z_real=mpfr_get_ld (Z[0],MPFR_RNDN);
          long double Z_imag=mpfr_get_ld (Z[1],MPFR_RNDN);
          idx_x=round((Z_real - re_min)*( re_size -1)/(re_max-re_min));
          idx_y=im_size - round((Z_imag - im_min)*( im_size -1)/(im_max-im_min));
          //TODO: implement cameras with proper multidimensional mpfr support
          
          
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
  mpfr_t *Z,
  mpfr_t *c,
  void (*function)(mpfr_t Z[], mpfr_t c[], mpfr_t *param, mpfr_t *temp),
  int8_t (*optimiser)(mpfr_t *c, mpfr_t *param, mpfr_t *temp),
  const uint64_t iter_min,
  const uint64_t iter_max,
  mpfr_t bail,
  mpfr_t *param,
  mpfr_t *temp,
  uint32_t dimensions
)
{
  const int8_t res=optimiser(c, param, temp);
  if(res==1)
    {
      return(0); /* We know c is inside, can return */
    }
  /* res== 0: We know c is outside, but does it last long enough? */
  /* res==-1: We don't know whether c is inside or outside, need full iteration cycle */
  const uint64_t iter=(res==0?iter_min:iter_max);
  
  for(uint64_t i=0;i<iter;i++)
    {
      function(Z, c, param, temp);
      mpfr_printf("(%e)+(%e)i\n",Z[0],Z[1]);
      mpfr_set_d(temp[0],0,MPFR_RNDN);
      for(uint32_t j=0;j<dimensions;j++)
        {
          mpfr_fma(temp[0],Z[j],Z[j],temp[0],MPFR_RNDN);
        }
      if(mpfr_cmp (temp[0], bail)>0)
        {
          return(i<iter_min?0:i);
        }
    }
  return(0);
}
