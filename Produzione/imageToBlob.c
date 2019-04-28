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
    MagickWand *output;
    MagickBooleanType status;

    // Aggiungo il file path ("htdocs/img/") per l'apertura dell'immagine
    char *requestedFile = malloc(strlen(fileName) + 11);
    memset(requestedFile, 0, strlen(fileName) + 11);
    strcat(requestedFile, imagePath);
    strcat(requestedFile, fileName);

    // Alloco in memoria dall'oggetto output
    output = NewMagickWand();

    // Leggo dal disco il file
    status = MagickReadImage(output, "template.jpg");
    free(requestedFile);
    if (status == MagickFalse)
      ThrowWandException(output);

    // Imposto il formato dell'immagine
    status = MagickSetImageFormat(output,"jpg");
    if (status == MagickFalse)
      ThrowWandException(output);

    // Imposto altezza e larghezza dell'immagine
    if (w != 0 && h != 0){
    status = MagickResizeImage(output, w, h, 0);
    if (status == MagickFalse)
      ThrowWandException(output);
    }

    // Imposto la qualità dell'immagine
    if (quality != 0){
    status = MagickSetImageCompressionQuality(output, quality);
    if (status == MagickFalse)
      ThrowWandException(output);
    }

    // Imposto lo spazio colore dell'immagine
    if (colorSpace != 0){
    status = MagickSetImageColorspace(output, colorSpace);
    if (status == MagickFalse)
      ThrowWandException(output);
    }

    MagickResetIterator(output);

    // Carico l'immagine in memoria
    unsigned char *test = MagickGetImageBlob(output, (size_t*)size);
    printf("\nLa dimensione è: %d indirizzo: %x\n", *size, test);

    // Libero la memoria dall'oggetto output
    output = DestroyMagickWand(output);

    return test;
}