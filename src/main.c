#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

/* Directory creation, file manipulation */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "arguments.h"
#include "list_tools.h"


#define STR_MAXLEN 80

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

  /* Parse command-line arguments */
  argp_parse(&argp, argc, argv, 0, 0, &args);
  int v=args.verbose;
  if(v) setbuf(stdout, NULL); /* to allow incomplete lines */

  /* Initialise unset arguments */
  if(!args.iter)
    args.iter=csv_parse_row("10,100,1000,10000");

  /* Prepare output files */
  /* Folder first */
  char dirname[STR_MAXLEN];
  dirname[STR_MAXLEN-1]='\0';
  snprintf(dirname,STR_MAXLEN-1,"%s_%ux%u(%lf+%lfi_%lf+%lfi)-%lf",
	   args.args[0],
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
      return(-1);
    }
  else
    {
      if(v)
	fprintf(stdout,"Directory already exists, entering\n");
    }


  /* Start creating files, and make an array of handles to the files */
  /* How many files? */
  unsigned int l;
  for(l=2;args.iter[l+1];l++);
  /* Create array of file pointers */
  FILE **files=calloc( l, sizeof(FILE*) );

  if(v)
    fprintf(stdout,"Initialising canvas arrays... ");
  /* Create canvas arrays */
  uint64_t ***canvas=calloc(l,sizeof(uint64_t**));//[l][args.re_size][args.im_size];
  for(unsigned int i=0;i<l;i++)
    {
      canvas[i]=calloc(args.re_size,sizeof(uint64_t*));
      for(uint32_t x=0;x<args.re_size;x++)
	{
	  canvas[i][x]=calloc(args.im_size,sizeof(uint64_t));
	}
    }
  
  if(v)
    fprintf(stdout,"done\n");


  if(v)
    fprintf(stdout,"Initialising files:\n");
  
  /* Map individual files */
  for(unsigned int i=0; args.iter[i+1]; i++)
    {
      char fname[STR_MAXLEN];
      char fpath[2*STR_MAXLEN]; /* filename with path */
      fname[STR_MAXLEN-1]='\0';
      fpath[2*STR_MAXLEN-1]='\0';
      snprintf(fname,STR_MAXLEN-1,"%lu-%lu",args.iter[i],args.iter[i+1]-1);
      snprintf(fpath,2*STR_MAXLEN-1,"%s/%s",dirname,fname);
      if(v)
	fprintf(stdout," - File %s\t... ",fname);
      if(stat(fpath,&st)==-1) /* File does not exist */
	{
	  /* Create file */
	  files[i]=fopen(fpath,"wb");
	  for(uint32_t x=0;x<args.re_size;x++)
	    {
	      for(uint32_t y=0;y<args.im_size;y++)
		{
		  /* Fill new file and canvas with zeros */
		  canvas[i][x][y]=0;
		}
	      fwrite(canvas[i][x],sizeof(uint64_t),args.im_size,files[i]);
	    }
	  fflush(files[i]);
	  if(v)
	    fprintf(stdout,"created.\n");
	}
      else if((st.st_mode & S_IFMT) == S_IFREG) /* It exists, and is a regular file of the right size */
	{
	  fprintf(stdout,"exists, ");
	  if(st.st_size != (off_t)(sizeof(uint64_t))*args.re_size*args.im_size)
	    {
	      if(v)
		fprintf(stdout,"corrupted, erasing.\n");
	      fprintf(stderr,"File %s has unexpected size. Erasing... ",fname);
	      remove(fpath);
	      fprintf(stderr,"done.\n");
	      i--;
	    }
	    else{
	      files[i]=fopen(fpath,"rb");
	      if(v)
		fprintf(stdout,"accessed.\n");
	    }
	}
      else
	{
	  if(v)
	    fprintf(stdout,"exists, inaccessible.\n");
	  fprintf(stderr,"Error encountered accessing file %s, quitting!\n",fname);
	  return 0;
	}
    }

 
  if(v)
    fprintf(stdout,"Task done, quitting\n");
  /* Clean up */
  free(args.iter);
  free(files); 

}
