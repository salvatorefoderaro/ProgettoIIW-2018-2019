all: hashTableCoda.c coda.c imageToBlob.c
	gcc -Wall -o cache hashTableCoda.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand`
