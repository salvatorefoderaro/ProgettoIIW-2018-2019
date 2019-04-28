#ifndef _IMAGETOBLOB_H 
#define _IMAGETOBLOB_H

#define imagePath "htdocs/img/"
#include "MagickWand/MagickWand.h"
#include <string.h>

void ThrowWandException(MagickWand *wand);
#define SwapWands(a,b) { MagickWand *tmp=a; a=b; b=tmp; }
char *getBlob(char *fileName, int w, int h, int quality, int colorSpace, int *size);

#endif