CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra

practice_main: practice_main.o image.o ppm.o 
	$(CC) -o practice_main practice_main.o image.o ppm.o  -lm 

practice_main.o: practice_main.c image.h ppm.h
	$(CC) $(CFLAGS) -c practice_main.c -lm

image.o: image.c image.h
	$(CC) $(CFLAGS) -c image.c  -lm

ppm.o: ppm.c ppm.h
	$(CC) $(CFLAGS) -c ppm.c  -lz -lm

valgrind:
	valgrind ./practice_main

clean:
	rm -f *.o cs220_paint
