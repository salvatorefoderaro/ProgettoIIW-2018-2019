#ifndef _IMAGETOBLOB_H 
#define _IMAGETOBLOB_H

#include "MagickWand/MagickWand.h"

void ThrowWandException(MagickWand *wand);
#define SwapWands(a,b) { MagickWand *tmp=a; a=b; b=tmp; }
char *getBlob(char *fileName, int w, int h, int quality, int colorSpace, int *size);

#endif