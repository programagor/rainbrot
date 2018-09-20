#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
  if(!buff)
    {
      fprintf(stderr,"Can't allocate space for buffer array\n");
      exit(1);
    }
  for(uint32_t x=0;x<arg->re_size;x++)
    {
      buff[x]=calloc(arg->im_size,sizeof(uint64_t));
      if(!buff[x])
    {
      fprintf(stderr,"Can't allocate space for buffer array\n");
      exit(1);
    }
    }
    
  /* Keep track of which rows were written to */
  uint8_t *dirty_rows=calloc(arg->re_size,sizeof(uint8_t));
  if(!dirty_rows)
    {
      fprintf(stderr,"Can't allocate space for dirty rows marker, quitting\n");
      exit(1);
    }
  
  
  
  /* Run until number of runs is reached */
  while(1)
    {
      
      /* Generate a point that is outside of set */
      long double complex c,Z;
      int8_t inside; /* 0 is no, 1 is yes, -1 is maybe */
      uint64_t run;
      do
        {
          /* Record positive runs (to avoid race condition at the end) */
          
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
          run=++*(arg->counter);
          pthread_mutex_unlock(arg->lock_rand);
          if(run>=arg->runs)
            {
              /* We're done, quitting thread */
              for(uint32_t x=0;x<arg->re_size;x++)
                {
                  free(buff[x]);
                }
              free(buff);
              free(dirty_rows);
              pthread_exit(NULL);
            }
          if(fmod(run,arg->runs/10.0)<1)
            {
              printf("Run: %10lu/%10lu\n",run,arg->runs);
            }
          /* Muller-Box Algorithm */
          c=csqrtl(-2.0*clogl(U1*((double)1.0/RAND_MAX)))*cexpl(PI_2_I*(U2*((double)1.0/RAND_MAX)));
          /* c now has Gaussian distribution (on the Gaussian plane) */
              
          /* Scale axis-wise (non-comforming mapping) */
          /* TODO: Adjustable standard deviation and mean of iterator samples */
          long double a1, a2, b1, b2; 
          a1=b1=1;
          a2=b2=0;
          c=(creall(c)*a1+a2)+((long double complex)I)*(cimagl(c)*b1+b2);
          
          inside=preiterator(c,arg->function,arg->optimiser,arg->iter[0],arg->iter[l],arg->bail);
          
          
        }
      while(inside==1);
      
      
      int64_t idx_x,idx_y; /* buff (2D array) coords */
      
      /* Now we have a point which is outside, can iterate and draw */
      Z=c;
      uint64_t i;
      for(i=0;abs(Z)<arg->bail&&i<arg->iter[l];i++)
        {
          Z=arg->function(Z,c); /* Iterate */
          
          
          /* increment buffer */
          idx_x=round((creall(Z) - arg->re_min)*( arg->re_size -1)/(arg->re_max-arg->re_min));
          idx_y=round((cimagl(Z) - arg->im_min)*( arg->im_size -1)/(arg->im_max-arg->im_min));
                  
          if(idx_x>=0 && idx_x < arg->re_size && idx_y>=0 && idx_y < arg->im_size)
            {
              buff[idx_x][idx_y]++;
              dirty_rows[idx_x]=1;
            }
        }
      
      
      /* Which buffer does this go to? */
      uint32_t k;
      for(k=0;i>arg->iter[k+1] && k<l-1;k++);
      pthread_mutex_lock(&arg->locks[k]);
      arg->hits[k]++;
      for(uint32_t x=0;x< arg->re_size;x++)
        {
          if(dirty_rows[x])
            {
              for(uint32_t y=0;y< arg->im_size;y++)
                {
                  if(buff[x][y])
                    {
                      arg->maps[k][arg->im_size*x+y]+=buff[x][y];
                    }
                }
            }
        }
      pthread_mutex_unlock(&arg->locks[k]);
      
      for(uint32_t x=0;x< arg->re_size;x++)
        {
          if(dirty_rows[x])
            {
              for(uint32_t y=0;y< arg->im_size;y++)
                {
                  buff[x][y]=0;
                }
              dirty_rows[x]=0;
            }
        }
    }
}

int8_t preiterator
(
  long double complex c,
  long double complex (*function)(long double complex c, long double complex Z),
  int8_t (*optimiser)(long double complex c),
  uint64_t iter_min,
  uint64_t iter_max,
  double bail
)
{
  int8_t res=optimiser(c);
  if(res==1)
    {
      return(res); /* We know c is inside, can return */
    }
  /* res== 0: We know c is outside, but does it last long enough? */
  /* res==-1: We don't know whether c is inside or outside, need full iteration cycle */
  uint64_t iter=(res==0?iter_min:iter_max);
  uint64_t i;
  long double complex Z=c;
  for(i=0;i<iter;i++)
    {
      Z=function(Z,c);
      if(cabs(Z)>bail)
        {
          return(i<iter_min?1:0);
        }
    }
  return(1);
}
