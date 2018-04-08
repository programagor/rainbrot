#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

/* Directory creation, file manipulation */
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>


#include "arguments.h"
#include "csv_parse.h"

int main (int argc,char** argv)
{
struct arguments args;
int retval=0;

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

/* Initialise unset arguments */
if(!args.iter)
  args.iter=csv_parse_row("10,100,1000,10000");

/* Prepare output files */
/* Folder first */
char dirname[80];
snprintf(dirname,80,"rain_%dx%d(%lf+%lfi_%lf+%lfi)-%lf",
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
    fprintf(stdout,"Entering directory\n");
}

if(v)
  fprintf(stdout,"Initialising files\n");



/* Clean up */
free(args.iter);

}
