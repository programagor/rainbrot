#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "list_tools.h"

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
      if( str[i]==',' )
	{
	  if(res[j]==0) /* We don't want zero entries */
	    {
	      free(res);
	      return(NULL);
	    }
	  else j++;
	}
      else res[j] = res[j]*10+str[i]-'0';
    }
  return(res);
}

void sort_list(uint64_t* list)
{
  /* Find list length */
  unsigned int l;
  for(l=2;list[l];l++);

  /* Replace duplicates with zeros */
  for(unsigned int a=0;a<l-1;a++)
    {
      for(unsigned int b=a+1;b<l;b++)
	{
	  if(list[a]==list[b]) list[b]=0;
	}
    }
  
  /* Now implement Bubble Sort, with zeros last */
  for(unsigned int a=l-1;a>0;a--)
    {
      for(unsigned int b=0;b<a;b++)
	{
	  if((list[b]>list[b+1] && list[b+1]!=0) || list[b]==0)
	    {
	      uint64_t swp=list[b];
	      list[b]=list[b+1];
	      list[b+1]=swp;
	    }
	}
    }
}
