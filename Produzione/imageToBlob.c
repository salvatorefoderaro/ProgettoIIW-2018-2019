#include "imageToBlob.h"

void ThrowWandException(MagickWand *wand){ 
  
  char *description;
  ExceptionType 
  severity;
  description=MagickGetException(wand,&severity);
  (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description);
  description=(char *) MagickRelinquishMemory(description);
  fprintf(stderr, "Errore nell'elaborazione dell'immagine\n");
  pthread_exit((void*)-1);
}

char *getBlob(char *fileName, int w, int h, int quality, int *size, char *fileType){
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
    status = MagickReadImage(output, fileName);
    free(requestedFile);
    if (status == MagickFalse)
      ThrowWandException(output);

    // Imposto il formato dell'immagine
    status = MagickSetImageFormat(output,fileType);
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

    MagickResetIterator(output);

    // Carico l'immagine in memoria
    unsigned char *test = MagickGetImageBlob(output, (size_t*)size);

    // Libero la memoria dall'oggetto output
    output = DestroyMagickWand(output);

    return test;
}