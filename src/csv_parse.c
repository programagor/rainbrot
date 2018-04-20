#include <stdint.h>
#include <stdlib.h>

#include <csv_parse.h>

uint64_t* csv_parse_row(char *str)
{
  
  uint64_t* res; /* returned list of integers */
  unsigned int i, j, c = 1; /* i - counter over str 
			       j - counter over res
			       c - number of elements */
  for(i=0;str[i];i++)
    {
      if( str[i]==',' && (i==0||str[i-1]!=',') ) c++;
      else if(str[i]<'0'||str[i]>'9') return(NULL);
    }

  /* Allocate and initialise list of iter values (null terminated) */
  res = calloc( (c+1), sizeof(uint64_t) );
  
  /* Go through entries, fill list */
  for(i=0,j=0;str[i];i++)
    {
      if( str[i]==',' ) j++;
      else res[j] = res[j]*10+str[i]-'0';
    }
  return(res);
}

