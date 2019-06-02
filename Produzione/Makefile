serverDetection: ServerIIW.c newHash.c coda.c imageToBlob.c
	gcc -g -o serverCache detection.c src/pattern/51Degrees.c src/cityhash/city.c src/threading.c ServerIIW.c newHash.c coda.c imageToBlob.c -Ddetection `pkg-config --cflags --libs MagickWand` -lm -lpthread

server: ServerIIW.c newHash.c coda.c imageToBlob.c
	gcc -g -o serverCache detection.c src/pattern/51Degrees.c src/cityhash/city.c src/threading.c ServerIIW.c newHash.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand` -lm -lpthread