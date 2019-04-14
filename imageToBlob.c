/*
`pkg-config --cflags --libs MagickWand`
*/ 

#include <stdio.h>
#include "imageToBlob.h"

void ThrowWandException(MagickWand *wand){ 
  char *description;

  ExceptionType
  severity;

  description=MagickGetException(wand,&severity);
  (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description);
  description=(char *) MagickRelinquishMemory(description);
}

char *getBlob(char *fileName, int w, int h, int quality, int colorSpace, int *size){

    MagickWand *output;  /* the appended output image */

    MagickBooleanType status;
    PixelWand *color;
    size_t lunghezza;
    MagickWandGenesis();

    output = NewMagickWand();

    status = MagickReadImage(output, "xc:blue" );
    if (status == MagickFalse)
      ThrowWandException(output);

    status = MagickSetImageFormat(output,"jpg");
    if (status == MagickFalse)
      ThrowWandException(output);

    if (w != 0 && h != 0){
    status = MagickResizeImage(output, w, h, 0);
    if (status == MagickFalse)
      ThrowWandException(output);
    }

    if (quality != 0){
    status = MagickSetImageCompressionQuality(output, 10);
    if (status == MagickFalse)
      ThrowWandException(output);
    }

    if (colorSpace != 0){
    status = MagickSetImageColorspace(output, 3);
    if (status == MagickFalse)
      ThrowWandException(output);
    }

    MagickResetIterator(output);

    unsigned char *test = MagickGetImageBlob(output, (size_t*)size);
    printf("\nLa dimensione è: %d indirizzo: %x\n", *size, test);
    output = DestroyMagickWand(output);

    MagickWandTerminus();

    return test;
}

/* void main(void){
  int size;
  getBlob("Aaa", 0, 0, 0, 3, &size);
} */