#include "pointsys.h"

#include <stdlib.h>

RawPtBox* newPtBox(){
	RawPtBox* box=malloc(sizeof(RawPtBox));
	box->nodes=NULL;
	return box;
};

void endPtBox(RawPtBox* box){
	/* TODO: Remove list */
	free(box);
};