#include <stdio.h>
#include "MagickWand/MagickWand.h"

void ThrowWandException(MagickWand *wand)
{ char
  *description;

  ExceptionType
  severity;

  description=MagickGetException(wand,&severity);
  (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description);
  description=(char *) MagickRelinquishMemory(description);
}

#define SwapWands(a,b) { MagickWand *tmp=a; a=b; b=tmp; }

int main(int argc, char *argv[])
{
    MagickWand
      *red,     /* red image wand */
      *rose,    /* rose image wand */
      *output;  /* the appended output image */

    PixelWand *color;

    MagickBooleanType status;

    MagickWandGenesis();

    red = NewMagickWand();
    MagickSetSize(red,100,100);
    status = MagickReadImage(red, "xc:red" );
    if (status == MagickFalse)
      ThrowWandException(red);

    rose = NewMagickWand();
    status = MagickReadImage(rose, "rose:" );
    if (status == MagickFalse)
      ThrowWandException(rose);

    color=NewPixelWand();
    PixelSetColor(color, "white");
    status = MagickRotateImage(rose,color,-90.0);
    if (status == MagickFalse)
      ThrowWandException(rose);
    color = DestroyPixelWand(color);

    MagickSetLastIterator(red);
    MagickAddImage(red,rose);
    rose = DestroyMagickWand(rose);

    MagickSetFirstIterator(red);
    output = MagickAppendImages(red,MagickFalse);
    red = DestroyMagickWand(red);

    size_t lunghezza;
    MagickSetImageFormat(output,"jpg");
    
    // Tutti e tre i metodi sono funzionanti
    MagickResizeImage(output, 2000, 2000, 0);
    MagickSetImageCompressionQuality(output, 95);
    MagickSetImageColorspace(output, 3);
    
    MagickResetIterator(output);

    // Scrittura su un bufer di memoria
    unsigned char *test = MagickGetImageBlob(output, &lunghezza);
    printf("\nLa dimensione è: %d\n", lunghezza);

    // Scrittura su file
    FILE *write_ptr;
    write_ptr = fopen("test124.jpg","wb");
    fwrite(test,lunghezza,1,write_ptr);

    status = MagickWriteImage(output,"test.png");
    if (status == MagickFalse)
      ThrowWandException(output);

    output = DestroyMagickWand(output);

    MagickWandTerminus();
}

