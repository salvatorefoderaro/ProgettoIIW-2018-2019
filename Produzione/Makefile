cache: newHash.c coda.c imageToBlob.c
	gcc -Wall -o cache newHash.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand`

serverCache: ServerIIW.c newHash.c coda.c imageToBlob.c
	gcc -o serverCache ServerIIW.c newHash.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand`