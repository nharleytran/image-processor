CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra

cs220_paint: driver.o cs220_paint.o 
	$(CC) -o cs220_paint driver.o cs220_paint.o -lm

png2txt: pnglite.o png2txt.o cs220_paint.o 
	$(CC) -o png2txt pnglite.o png2txt.o cs220_paint.o -lm -lz

txt2png: pnglite.o txt2png.o cs220_paint.o 
	$(CC) -o txt2png pnglite.o txt2png.o cs220_paint.o -lm -lz

driver.o: driver.c cs220_paint.h
	$(CC) $(CFLAGS) -c driver.c  -lm

cs220_paint.o: cs220_paint.c cs220_paint.h
	$(CC) $(CFLAGS) -c cs220_paint.c  -lz -lm

pnglite.o : pnglite.c pnglite.h 
	$(CC) $(CFLAGS) -c pnglite.c -lm -lz

png2txt.o: png2txt.c cs220_paint.h 
	$(CC) $(CFLAGS) -c png2txt.c  -lz -lm

txt2png.o: txt2png.c cs220_paint.h 
	$(CC) $(CFLAGS) -c txt2png.c  -lz -lm

clean:
	rm -f *.o cs220_paint

