#include <stdlib.h>
#include <stdio.h>
#include <argp.h>

#include "arguments.h"
#include "csv_parse.h"

int main (int argc,char** argv)
{
struct arguments args;
FILE *outstream;

/* Set argument defaults */
args.args[0] = NULL;
int v = args.verbose = 0;
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

/* Initialise unset arguments */
if(!args.iter)
  args.iter=csv_parse_row("10,100,1000,10000");
for(int i=0;args.iter[i];i++)
  {
printf("%d\n",args.iter[i]);
  }

/* Prepare output files */
outstream = stdout;
  
if(args.verbose)
  fprintf(stdout,"Initialising array...");

if(args.verbose)
  fprintf(stdout,"[OK]\n");



/* Clean up */
free(args.iter);

}
