cache: newHash.c coda.c imageToBlob.c
	gcc -Wall -o cache newHash.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand`

serverCache: ServerIIW.c newHash.c coda.c imageToBlob.c
	gcc -o serverCache detection.c src/pattern/51Degrees.c src/cityhash/city.c src/threading.c ServerIIW.c newHash.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand`  -lm -lpthread
