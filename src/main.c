#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

#include <string.h>

/* Directory creation, file manipulation */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/* Memory management */
#include <sys/mman.h>

/* Thread control */
#include <pthread.h>

/* Arbitrary precision */
#include <mpfr.h>

#include "arguments.h"
#include "list_tools.h"
#include "worker.h"
#include "functions.h"

#define STR_MAXLEN 80

#define CRD2IDX(X,Y) ((Y)*args.re_size+(X))

int main (int argc,char** argv)
{
  struct arguments args;

  /* Set argument defaults */
  args.args[0] = NULL;
  args.verbose = 0;
  args.re_size = 400;
  args.im_size = 400;
  args.re_min = -2;
  args.re_max =  2;
  args.im_min = -2;
  args.im_max =  2;
  args.iter = NULL;
  args.bail = 40;
  args.runs = 10000000L;
  args.threads = 4;
  args.seed=time(NULL);
  args.function="mandelbrot";
  args.a_std=-1;

  /* Parse command-line arguments */
  argp_parse(&argp, argc, argv, 0, 0, &args);
  int v=args.verbose;
  if(v) setbuf(stdout, NULL); /* to allow incomplete lines */
  srand(args.seed);
  void (*function)(mpfr_t *Z, const mpfr_t *c, mpfr_t *param, mpfr_t *temp);
  int8_t (*optimiser)(const mpfr_t *c, mpfr_t *param, mpfr_t *temp);
  if(strcmp(args.function,"mandelbrot")==0)
    {
      function=mandelbrot;
      optimiser=mandelbrot_optimiser;
    }
  else if(strcmp(args.function,"ship")==0)
    {
      function=ship;
      optimiser=no_optimiser;
    }
  else if(strcmp(args.function,"julia")==0)
    {
      function=julia;
      optimiser=no_optimiser;
    }
  else
    {
      /* TODO: Implement loading functions from external .o files */
      fprintf(stderr,"Couldn't load file functions/%s.o (not implemented)\n",args.function);
      return(1);
    }
  /* If standard deviation and mean of the starting points isn't specified, default to the center of window */
  if(args.a_std<0)
    {
      args.a_std=(args.re_max-args.re_min);
      args.b_std=(args.im_max-args.im_min);
      args.a_mu=(args.re_max+args.re_min)/2;
      args.b_mu=(args.im_max+args.im_min)/2;
    }

  /* Initialise unset arguments */
  if(!args.iter)
    args.iter=csv_parse_row("10,100,1000,10000");

  /* Prepare output files */
  /* Folder first */
  char dirname[STR_MAXLEN];
  dirname[STR_MAXLEN-1]='\0';
  snprintf(dirname,STR_MAXLEN-1,"%s_%ux%u_(%lg)+(%lgi)_(%lg)+(%lgi)-%lg",
           args.function,
           args.re_size,
           args.im_size,
           args.re_min,
           args.im_min,
           args.re_max,
           args.im_max,
           args.bail);

  if(v)
    fprintf(stdout,"Outputting files into working directory: ./%s\n",dirname);

  struct stat st = {0};
  if(stat(dirname, &st) == -1)
    {
      mkdir(dirname,0755);
      if(v)
        fprintf(stdout,"Creating directory\n");
    }
  else if((st.st_mode & S_IFMT) != S_IFDIR)
    {
      fprintf(stderr,"Can't create directory: File of the same name already exists.\n");
      return(1);
    }
  else
    {
      if(v)
        fprintf(stdout,"Directory already exists, entering\n");
    }


  /* Start creating files, and make an array of handles to the files */
  /* How many files? */
  int l;
  for(l=1;args.iter[l+1];l++);

  if(v)
    fprintf(stdout,"Initialising files:\n");
  
  /* Map individual files */
  off_t fsize = (off_t)(sizeof(uint64_t))*args.re_size*args.im_size;
  int files[l];
  uint64_t *maps[l];
  uint64_t *zeros=calloc(args.re_size,sizeof(uint64_t)); /* String full of zeros, to write into new files in batches */
  if(!zeros)
  {
    fprintf(stderr,"Can't create a string full of zeros, quitting\n");
    return(1);    
  }        
  for(int i=0; i<l; i++)
    {
      char fname[STR_MAXLEN];
      char fpath[2*STR_MAXLEN]; /* filename with path */
      fname[STR_MAXLEN-1]='\0';
      fpath[2*STR_MAXLEN-1]='\0';
      snprintf(fname,STR_MAXLEN-1,"%lu-%lu.rain",args.iter[i],args.iter[i+1]-1);
      snprintf(fpath,2*STR_MAXLEN-1,"%s/%s",dirname,fname);
      if(v)
        fprintf(stdout,"  - %u. file (%s)\t... ",i+1,fname);
      if(stat(fpath,&st)==-1) /* File does not exist */
        {
          /* Create file */
          files[i]=open(fpath,O_RDWR|O_CREAT,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
          for(uint64_t x=0;x<args.re_size;x++)
            {
              if(-write(files[i],zeros,args.im_size*sizeof(uint64_t))==-1) /* Create file of the right size, full of zeros */
                {
                  if(v)
                    fprintf(stdout,"corrupted.\n");
                  fprintf(stderr,"Quitting");
                  return(1);
                }
            }
          if(v)
            fprintf(stdout,"created.\n");
          close(files[i]);
          files[i]=open(fpath,O_RDWR);
        }
      else if((st.st_mode & S_IFMT) == S_IFREG) /* It exists */
        {
          if(v)
            fprintf(stdout,"exists, ");
          if(st.st_size != fsize) /* but has the wrong size */
            {
              if(v)
                fprintf(stdout,"corrupted, erasing.\n");
              fprintf(stderr,"File %s has unexpected size. Erasing... ",fname);
              remove(fpath); /* delete file */
              fprintf(stderr,"done.\n");
              i--; /* Retry */
            }
            else{
              /* File exists and is of the right size */
              files[i]=open(fpath,O_RDWR);
              if(v)
                fprintf(stdout,"accessed.\n");
            }
        }
      else
        {
          if(v)
            fprintf(stdout,"exists, inaccessible.\n");
          fprintf(stderr,"Error encountered accessing file %s, quitting!\n",fname);
          return(1);
        }
      /* Finally, when the file is ready, map it to memory */
      if((maps[i]=mmap(NULL,fsize,PROT_READ|PROT_WRITE,MAP_SHARED,files[i],0))==MAP_FAILED)
        {
          fprintf(stderr,"Can't create mapping, quitting\n");
          return(1);
        }
      madvise(maps[i],fsize,MADV_RANDOM);
    }
  free(zeros);
  /* Now, everything is ready. Let's roll! */

  /* Create threads */
  struct argw argw[args.threads];
  uint64_t counter=0; /* Keeps track of how many runs processed */
  uint64_t hits[l];
  pthread_mutex_t lock_rand;
  pthread_mutex_t *locks[l]; /* One lock for each file */
  pthread_mutex_init(&lock_rand,NULL);
  for(int i=0;i<l;i++)
    {
      locks[i]=malloc(args.re_size*sizeof(pthread_mutex_t));
      for(uint32_t x=0;x<args.re_size;x++)
        {
          pthread_mutex_init(&locks[i][x],NULL);
        }
      hits[i]=0;
    }
  if(v)
    fprintf(stdout,"Starting %hu worker%s:\n",args.threads,args.threads>1?"s":"");

  pthread_t thr[args.threads];
  for(uint16_t t=0;t<args.threads;t++)
    {
      if(v)
        fprintf(stdout,"  - %hu. worker\t... ",t+1);
      argw[t]=
        (struct argw) {
          &lock_rand,
          locks,
          maps,
          &counter,
          hits,
          args.re_size,
          args.im_size,
          args.re_min,
          args.re_max,
          args.im_min,
          args.im_max,
          args.iter,
          args.bail,
          args.runs,
          function,
          optimiser,
          args.a_std,
          args.b_std,
          args.a_mu,
          args.b_mu
        };
      pthread_create(&thr[t],NULL,worker,(void*)&argw[t]);
      if(v)
        fprintf(stdout,"done.\n");
    }

  if(v)
    fprintf(stdout,"All workers up and running\n");

  
  /* Wait for threads to complete */
  for(uint16_t t=0;t<args.threads;t++)
    {
      pthread_join(thr[t],NULL);
    }

  if(v)
    fprintf(stdout,"All workers finished\n");
  if(v)
    fprintf(stdout,"Hits per file:\n");
  uint64_t hits_total=0;
  for(int i=0; i<l; i++)
    {
      char fname[STR_MAXLEN];
      fname[STR_MAXLEN-1]='\0';
      snprintf(fname,STR_MAXLEN-1,"%lu-%lu",args.iter[i],args.iter[i+1]-1);
      if(v)
        printf("  - %d. file (%s):\t %lu hits\n",i+1,fname,hits[i]);
      hits_total+=hits[i];
    }
  fprintf(stdout,"Total hits: %lu\n",hits[0]);
  
  if(v)
    fprintf(stdout,"Task done, cleaning up...\n");
  /* Clean up */

  pthread_mutex_destroy(&lock_rand);
  for(int i=0;i<l;i++)
    {
      for(uint32_t x=0;x<args.re_size;x++)
        {
          pthread_mutex_destroy(&locks[i][x]);
        }
      free(locks[i]);
      if(v)
        fprintf(stdout,"Syncing file %d... ",i);
      msync(maps[i],fsize,MS_SYNC);
      munmap(maps[i],fsize);
      close(files[i]);
      if(v)
        fprintf(stdout,"done.\n ");
    }
  free(args.iter);
  if(v)
    fprintf(stdout,"Quitting\n");

}
