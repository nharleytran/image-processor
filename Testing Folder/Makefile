CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -g

project: project.o image.o ppm.o texture_synthesis.o	
	$(CC) -o project project.o image.o ppm.o texture_synthesis.o -lm -g

project.o: project.c image.h ppm.h texture_synthesis.h
	$(CC) $(CFLAGS) -c project.c 

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c 

ppm.o: ppm.c ppm.h
	$(CC) $(CFLAGS) -c ppm.c 

texture_synthesis.o: texture_synthesis.h texture_synthesis.c image.h
	$(CC) $(CFLAGS) -c texture_synthesis.c  

valgrind:
	valgrind -v ./project

clean:
	rm -f *.o

