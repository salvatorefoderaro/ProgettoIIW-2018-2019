serverDetection: server.c newHash.c coda.c imageToBlob.c
	gcc -g -o serverDetection detection.c src/pattern/51Degrees.c src/cityhash/city.c src/threading.c server.c newHash.c coda.c imageToBlob.c -Ddetection `pkg-config --cflags --libs MagickWand` -lm -lpthread

server: server.c newHash.c coda.c imageToBlob.c
	gcc -g -o server detection.c src/pattern/51Degrees.c src/cityhash/city.c src/threading.c server.c newHash.c coda.c imageToBlob.c `pkg-config --cflags --libs MagickWand` -lm -lpthread -lrt