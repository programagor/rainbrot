#include <stdio.h>

#include "pointsys.h"

int main(int argc,char* argv)
{
	printf("Rainbrot 0.0.1\n");
	
	
	printf("Initialising iterator...\n");
	
	RawPtBox* masterBox=newPtBox();
	
	
	
	printf("[ok]\n");
	
	
	
	printf("Quitting...");

	endPtBox(masterBox);
	
	printf("return(0);\n");return(0);
}