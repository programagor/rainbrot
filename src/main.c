#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

/* Directory creation, file manipulation */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

/* Memory management */
#include <sys/mman.h>

/* Thread control */
#include <pthread.h>

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
  args.re_min = -1;
  args.re_max =  1;
  args.im_min = -1;
  args.im_max =  1;
  args.iter = NULL;
  args.bail = 40;
  args.runs = 10000000L;
  args.threads = 4;
  args.seed=time(NULL);
  args.function="mandelbrot";

  /* Parse command-line arguments */
  argp_parse(&argp, argc, argv, 0, 0, &args);
  int v=args.verbose;
  if(v) setbuf(stdout, NULL); /* to allow incomplete lines */
  srand(args.seed);
  long double complex (*function)(long double complex c, long double complex Z)=mandelbrot;
  int8_t (*optimiser)(long double complex c)=mandelbrot_optimiser;

  /* Initialise unset arguments */
  if(!args.iter)
    args.iter=csv_parse_row("10,100,1000,10000");

  /* Prepare output files */
  /* Folder first */
  char dirname[STR_MAXLEN];
  dirname[STR_MAXLEN-1]='\0';
  snprintf(dirname,STR_MAXLEN-1,"%s_%ux%u(%lf+%lfi_%lf+%lfi)-%lf",
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
      snprintf(fname,STR_MAXLEN-1,"%lu-%lu",args.iter[i],args.iter[i+1]-1);
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
  int64_t queue[args.threads]; /* Used to stop race condition at last run */
  uint64_t counter=0; /* Keeps track of how many runs processed */
  uint64_t run=0;
  pthread_mutex_t lock_iter;
  pthread_mutex_t lock_rand;
  pthread_mutex_t locks[l]; /* One lock for each file */
  pthread_mutex_init(&lock_iter,NULL);
  pthread_mutex_init(&lock_rand,NULL);
  for(int i=0;i<l;i++)
    {
      pthread_mutex_init(&locks[i],NULL);
    }
  if(v)
    fprintf(stdout,"Starting %hu worker%s:\n",args.threads,args.threads>1?"s":"");

  pthread_t thr[args.threads];
  for(uint16_t t=0;t<args.threads;t++)
    {
      if(v)
	fprintf(stdout,"  - %hu. worker\t... ",t+1);
      queue[t]=0;
      argw[t]=
        (struct argw) {
          &lock_iter,
          &lock_rand,
          locks,
          maps,
          &counter,
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
          args.threads,
          t,
          queue,
          &run
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
    fprintf(stdout,"Task done, quitting\n");
  /* Clean up */

  for(int i=0;i<l;i++)
    {
      msync(maps[i],fsize,MS_SYNC);
      munmap(maps[i],fsize);
      close(files[i]);
    }
  free(args.iter);

}
