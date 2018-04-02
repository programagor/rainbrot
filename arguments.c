#include <argp.h>

#include "arguments.h"

/*
  OPTIONS.  Field 1 in ARGP
  Order of fields: {NAME, KEY, FLAGS, DOC}.
*/
static struct argp_option options[] =
{
  {"verbose",'v',0,0,"Produce verbose output"},
  {"size",k_size,"WIDTHxHEIGHT",0,"Size of image in pixels"},
  {"window",k_window,"RE_MIN,IM_MIN,RE_MAX,IM_MAX",0,"Displayed area of the Gauss plane"},
  {0}
};


/*
   PARSER. Field 2 in ARGP.
   Order of parameters: KEY, ARG, STATE.
*/
static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  struct arguments *arguments = state->input;

  switch (key)
  {
  case 'v':
    arguments->verbose = 1;
    break;
  case k_size:
    if(2!=sscanf(arg,"%dx%d",
		 &(arguments->re_size),
		 &(arguments->im_size)))
      argp_usage(state);
    break;
  case k_window:
    if(4!=sscanf(arg,"%lf,%lf,%lf,%lf",
		 &(arguments->re_min),
		 &(arguments->im_min),
		 &(arguments->re_max),
		 &(arguments->im_max)))
      argp_usage(state);
    break;
  case ARGP_KEY_ARG:
    if (state->arg_num >= 1)
      {
	argp_usage(state);
      }
    arguments->args[state->arg_num]=arg;
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 1)
      {
	argp_usage(state);
      }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

/*
   ARGS_DOC. Field 3 in ARGP.
   A description of the non-option command-line arguments
     that we accept.
*/
static char args_doc[] = "OUTFILE";

/*
  DOC.  Field 4 in ARGP.
  Program documentation.
*/
static char doc[] =
  "rainbrot -- A program to generate histogram of probabilities that certain region of Gauss plane will be the solution of an iteration of a random sampled point using a specified complex iterative equation.";


/*
   The ARGP structure itself.
*/
struct argp argp = {options, parse_opt, args_doc, doc};
