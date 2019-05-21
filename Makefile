cache: hashTableCoda.c coda.c imageToBlob.c
	gcc -Wall -o -g cache hashTableCoda.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand`

server: ServerIIW.c
	gcc -Wall -o -g server ServerIIW.c -lpthread
