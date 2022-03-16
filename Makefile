CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra

project: project.o image.o ppm.o texture_synthesis.o	
	$(CC) -o project project.o image.o ppm.o  -lm 

project.o: project.c image.h ppm.h texture_synthesis.h
	$(CC) $(CFLAGS) -c project.c -lm

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c  -lm

ppm.o: ppm.c ppm.h
	$(CC) $(CFLAGS) -c ppm.c  -lz -lm

texture_synthesis.o: texture_synthesis.h texture_synthesis.c image.h
	$(CC) $(CFLAGS) -c texture_synthesis.c  -lz -lm

valgrind:
	valgrind ./project

clean:
	rm -f *.o

