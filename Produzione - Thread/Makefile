serverDetection: ServerIIW.c newHash.c coda.c imageToBlob.c
	gcc -g -o serverDetection -O3 detection.c src/pattern/51Degrees.c src/cityhash/city.c src/threading.c ServerIIW.c newHash.c coda.c imageToBlob.c -D detection `pkg-config --cflags --libs MagickWand` -lm -lpthread

server: ServerIIW.c newHash.c coda.c imageToBlob.c
	gcc -g -o server -O3 detection.c src/pattern/51Degrees.c src/cityhash/city.c src/threading.c ServerIIW.c newHash.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand` -lm -lpthread