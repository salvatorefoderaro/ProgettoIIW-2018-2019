cache: hashTableCoda.c coda.c imageToBlob.c
	gcc -Wall -o cache hashTableCoda.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand`

server: ServerIIW.c
	gcc -Wall -o server ServerIIW.c -lpthread
