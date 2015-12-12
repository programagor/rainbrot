#ifndef H_POINTSYS
#define H_POINTSYS

#include <complex.h>

typedef long double _Complex ptType;

typedef struct{
	unsigned int n;
	ptType* array;
}Zslice;

typedef struct{
	Zslice* nodes;
} RawPtBox;

RawPtBox* newPtBox();

void endPtBox(RawPtBox*);

#endif
